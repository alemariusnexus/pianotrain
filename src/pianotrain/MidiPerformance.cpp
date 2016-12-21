#include "MidiPerformance.h"
#include "MidiService.h"
#include <cstdint>
#include <climits>
#include <algorithm>

using std::min;
using std::max;





MidiPerformance::MidiPerformance(QObject* parent)
		: QObject(parent), nextStart(0), noteNameOctaveOffset(-1), chordActive(false)
{
	auto graceFunc = [](int32_t& graceNum, int32_t& graceDenom, int32_t lenNum, int32_t lenDenom) {
    	graceNum = lenNum;
    	graceDenom = lenDenom*2;

    	float relGraceLength = graceNum / (float) graceDenom;

    	if (relGraceLength > 0.125f)
		{
    		graceNum = 1;
    		graceDenom = 8;
		}
    };

	setTimingGracePeriodRelativeFunction(graceFunc);
}


void MidiPerformance::addAbsoluteNote(int8_t midiKey, int32_t start, int32_t lenNum, int32_t lenDenom)
{
	Note note;
	note.midiKey = midiKey;
	note.startTick = start;
	note.lenNum = lenNum;
	note.lenDenom = lenDenom;
	note.hit = false;
	note.missed = false;

	printf("Note %d starts at %d (grace: %d)\n", midiKey, start, gracePeriodFunc(lenNum, lenDenom));

	notes << note;
}


void MidiPerformance::startChord()
{
	if (!chordActive)
	{
		chordActive = true;
		chordMinLength = -1;
	}
}


void MidiPerformance::endChord()
{
	if (chordActive)
	{
		chordActive = false;

		if (chordMinLength >= 0)
		{
			nextStart += chordMinLength;
		}
	}
}


void MidiPerformance::addNote(int8_t midiKey, int32_t lenNum, int32_t lenDenom)
{
	bool chordify = !chordActive;

	if (chordify)
	{
		startChord();
	}

	int32_t start = nextStart;
	int32_t tickLen = noteLengthToTickCount(lenNum, lenDenom);

	if (midiKey >= 0)
	{
		addAbsoluteNote(midiKey, start, lenNum, lenDenom);
	}
	else
	{
		// It's a rest. Count it as taking up time, but don't add it as a note
	}

	chordMinLength = chordMinLength >= 0 ? min(chordMinLength, tickLen) : tickLen;

	if (chordify)
	{
		endChord();
	}
}


void MidiPerformance::addNote(const CString& name, int32_t lenNum, int32_t lenDenom)
{
	addNote(ConvertStringToMidiKey(name, noteNameOctaveOffset), lenNum, lenDenom);
}


void MidiPerformance::addNote(int8_t midiKey, int32_t lenDenom)
{
	addNote(midiKey, 1, lenDenom);
}


void MidiPerformance::addNote(const CString& name, int32_t lenDenom)
{
	addNote(name, 1, lenDenom);
}


void MidiPerformance::addRest(int32_t lenNum, int32_t lenDenom)
{
	addNote(-1, lenNum, lenDenom);
}


void MidiPerformance::addRest(int32_t lenDenom)
{
	addRest(1, lenDenom);
}


int32_t MidiPerformance::noteLengthToTickCount(int32_t num, int32_t denom) const
{
	return (MIDI_PERFORMANCE_LEN_WHOLE * num) / denom;
}


void MidiPerformance::setTimingGracePeriodFunction(const std::function<int32_t(int32_t, int32_t)>& func)
{
	gracePeriodFunc = func;
}


void MidiPerformance::setTimingGracePeriodRelativeFunction(const std::function<void(int32_t&, int32_t&, int32_t, int32_t)>& func)
{
	gracePeriodFunc = [this, func](int32_t lenNum, int32_t lenDenom) {
		int32_t graceNum;
		int32_t graceDenom;
		func(graceNum, graceDenom, lenNum, lenDenom);
		return noteLengthToTickCount(graceNum, graceDenom);
	};
}


void MidiPerformance::setTempo(int32_t bpm, int32_t baseNum, int32_t baseDenom)
{
	int32_t baseTickValue = (baseNum * MIDI_PERFORMANCE_LEN_WHOLE) / baseDenom;
	ticksPerMinute = bpm * baseTickValue;
}


void MidiPerformance::setNoteNameOctaveOffset(int8_t octaveOffset)
{
	noteNameOctaveOffset = octaveOffset;
}


void MidiPerformance::start()
{
	MidiService* midi = MidiService::getInstance();

	connect(midi, SIGNAL(messageReceived(uint8_t, uint8_t, uint8_t, uint64_t)),
			this, SLOT(midiMessageReceived(uint8_t, uint8_t, uint8_t, uint64_t)));

	perfStartTimestamp = GetMultimediaTimerMilliseconds();

	perfThread = std::thread(&MidiPerformance::performanceThreadMain, this);
}


void MidiPerformance::hitNote(int8_t midiKey, uint64_t timestamp)
{
	Note* closestNote = nullptr;

	int32_t tick = getPerformanceTick(timestamp);

	//printf("> Note %d hit at %u\n", midiKey, tick);

	for (Note& note : notes)
	{
		if (!note.hit)
		{
			int32_t gracePeriod = gracePeriodFunc(note.lenNum, note.lenDenom);

			if (tick >= note.startTick-gracePeriod  &&  tick <= note.startTick+gracePeriod)
			{
				if (!closestNote  ||  abs(note.startTick - tick) < abs(closestNote->startTick - tick))
				{
					closestNote = &note;
				}
			}
		}
	}

	if (closestNote)
	{
		if (closestNote->midiKey == midiKey)
		{
			closestNote->hit = true;
			reportHitNote(*closestNote);
		}
		else
		{
			reportWrongNote(*closestNote, midiKey);
		}
	}
	else
	{
		reportExcessNote(midiKey, tick);
	}
}


void MidiPerformance::hitNote(int8_t midiKey)
{
	hitNote(midiKey, GetMultimediaTimerMilliseconds());
}


void MidiPerformance::releaseNote(uint8_t midiKey, uint64_t timestamp)
{
	// TODO: Implement
}


void MidiPerformance::releaseNote(uint8_t midiKey)
{
	releaseNote(midiKey, GetMultimediaTimerMilliseconds());
}


void MidiPerformance::midiMessageReceived(uint8_t status, uint8_t data1, uint8_t data2, uint64_t timestamp)
{
	int32_t channel = status & 0xF;
	int32_t cmd = status & 0xF0;

	if (cmd == 0x90)
	{
		// Note On event. Also Note Off if velocity == 0

		int32_t key = data1 & 0x7F;
		int32_t velocity = data2 & 0x7F;

		if (velocity > 0)
		{
			// Actual Note On

			hitNote(key, timestamp);
		}
		else
		{
			// Alternative implementation for Note Off

			releaseNote(key, timestamp);
		}
	}
	else if (cmd == 0x80)
	{
		// Note Off event. Not always implemented (Note On with velocity=0 is often used instead)

		int32_t key = data1 & 0x7F;
		int32_t velocity = data2 & 0x7F;

		releaseNote(key, timestamp);
	}
	/*else
	{
		printf("MIDI message: %02X - %02X %02X\n", status, data1, data2);
	}

	fflush(stdout);*/
}


void MidiPerformance::performanceThreadMain()
{
	bool notesOpen;

	do
	{
		notesOpen = false;

		int32_t currentTick = getPerformanceTick();

		emit currentTickUpdated(currentTick, MIDI_PERFORMANCE_LEN_WHOLE);

		for (Note& note : notes)
		{
			if (!note.hit  &&  !note.missed)
			{
				notesOpen = true;

				int32_t gracePeriod = gracePeriodFunc(note.lenNum, note.lenDenom);

				if (note.startTick+gracePeriod < currentTick)
				{
					note.missed = true;
					reportMissedNote(note);
				}
			}
		}

		SleepMilliseconds(1);
	} while (notesOpen);

	/*SleepMilliseconds(500);

	for (Note& note : notes)
	{
		printf("Note - hit:%s, missed:%s\n", note.hit ? "1" : "0", note.missed ? "1" : "0");
		fflush(stdout);
	}*/
}


int32_t MidiPerformance::getPerformanceTick(uint64_t timestamp) const
{
	float minutesSinceStart = (timestamp - perfStartTimestamp) / 60000.0f;
	return (int32_t) floorf(minutesSinceStart * ticksPerMinute + 0.5f);
}


int32_t MidiPerformance::getPerformanceTick() const
{
	return getPerformanceTick(GetMultimediaTimerMilliseconds());
}


void MidiPerformance::reportHitNote(const Note& note)
{
	uint32_t gracePeriod = gracePeriodFunc(note.lenNum, note.lenDenom);
	printf("Hit correct note %s at %d (%d - %d)\n", ConvertMidiKeyToString(note.midiKey).get(), getPerformanceTick(),
			note.startTick - gracePeriod, note.startTick + gracePeriod);
	fflush(stdout);
}


void MidiPerformance::reportMissedNote(const Note& note)
{
	uint32_t gracePeriod = gracePeriodFunc(note.lenNum, note.lenDenom);
	printf("Missed note %s at %d (%d - %d)!\n", ConvertMidiKeyToString(note.midiKey).get(), getPerformanceTick(),
			note.startTick - gracePeriod, note.startTick + gracePeriod);
	fflush(stdout);
}


void MidiPerformance::reportExcessNote(int8_t midiKey, int32_t tick)
{
	printf("Excess note %s at %d!\n", ConvertMidiKeyToString(midiKey).get(), tick);
	fflush(stdout);
}


void MidiPerformance::reportWrongNote(const Note& note, int8_t playedMidiKey)
{
	uint32_t gracePeriod = gracePeriodFunc(note.lenNum, note.lenDenom);
	printf("Hit wrong note %s at %d (%d - %d, correct: %s)\n", ConvertMidiKeyToString(playedMidiKey).get(), getPerformanceTick(),
			note.startTick - gracePeriod, note.startTick + gracePeriod, ConvertMidiKeyToString(note.midiKey).get());
	fflush(stdout);
}
