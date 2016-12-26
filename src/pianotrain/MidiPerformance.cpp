#include "MidiPerformance.h"
#include "MidiService.h"
#include <QtCore/QTimer>
#include <cstdint>
#include <climits>
#include <algorithm>

using std::min;
using std::max;





MidiPerformance::MidiPerformance(QObject* parent)
		: QObject(parent), nextStart(0), perfThread(nullptr), noteNameOctaveOffset(-1), chordActive(false)
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


MidiPerformanceThread::MidiPerformanceThread(MidiPerformance* perf, QObject* parent)
		: QThread(parent), perf(perf), stopped(false)
{
}


void MidiPerformance::addAbsoluteNote(int8_t midiKey, int32_t startNum, int32_t startDenom, int32_t lenNum, int32_t lenDenom, void* userData)
{
	Note note;
	note.midiKey = midiKey;
	note.startTick = noteLengthToTickCount(startNum, startDenom);
	note.lenNum = lenNum;
	note.lenDenom = lenDenom;
	note.hit = false;
	note.missed = false;
	note.userData = userData;

	//printf("Adding note %d at %d/%d for length %d/%d\n", midiKey, startNum, startDenom, lenNum, lenDenom);
	//fflush(stdout);

	notes << note;
}


void MidiPerformance::addAbsoluteRest(int32_t startNum, int32_t startDenom, int32_t lenNum, int32_t lenDenom, void* userData)
{
	// Currently, rests aren't used for anything, so just do nothing.
}


void MidiPerformance::advance(int32_t lenNum, int32_t lenDenum)
{
	nextStart += chordMinLength;
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
			advance(chordMinLength, MIDI_PERFORMANCE_LEN_WHOLE);
		}
	}
}


void MidiPerformance::addNote(int8_t midiKey, int32_t lenNum, int32_t lenDenom, void* userData)
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
		addAbsoluteNote(midiKey, start, MIDI_PERFORMANCE_LEN_WHOLE, lenNum, lenDenom, userData);
	}
	else
	{
		addAbsoluteRest(start, MIDI_PERFORMANCE_LEN_WHOLE, lenNum, lenDenom, userData);
	}

	chordMinLength = chordMinLength >= 0 ? min(chordMinLength, tickLen) : tickLen;

	if (chordify)
	{
		endChord();
	}
}


void MidiPerformance::addNote(const CString& name, int32_t lenNum, int32_t lenDenom, void* userData)
{
	addNote(ConvertStringToMidiKey(name, noteNameOctaveOffset), lenNum, lenDenom, userData);
}


void MidiPerformance::addNote(int8_t midiKey, int32_t lenDenom, void* userData)
{
	addNote(midiKey, 1, lenDenom, userData);
}


void MidiPerformance::addNote(const CString& name, int32_t lenDenom, void* userData)
{
	addNote(name, 1, lenDenom, userData);
}


void MidiPerformance::addRest(int32_t lenNum, int32_t lenDenom, void* userData)
{
	addNote(-1, lenNum, lenDenom, userData);
}


void MidiPerformance::addRest(int32_t lenDenom, void* userData)
{
	addRest(1, lenDenom, userData);
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


void MidiPerformance::startAt(uint64_t startTimestamp)
{
	MidiService* midi = MidiService::getInstance();

	// Stop any performance thread that might still be running
	stop();

	connect(midi, SIGNAL(messageReceived(uint8_t, uint8_t, uint8_t, uint64_t)),
			this, SLOT(midiMessageReceived(uint8_t, uint8_t, uint8_t, uint64_t)), Qt::UniqueConnection);

	//perfStartTimestamp = GetMultimediaTimerMilliseconds();

	perfThread = new MidiPerformanceThread(this);
	perfThread->performanceStartTime = startTimestamp;

	connect(perfThread, SIGNAL(finished()), this, SLOT(perfThreadFinished()));

	perfThread->start();
}


void MidiPerformance::start()
{
	startAt(GetMultimediaTimerMilliseconds());
}


void MidiPerformance::stop()
{
	if (perfThread)
	{
		perfThread->stopped = true;
	}
}


bool MidiPerformance::isPerformanceRunning() const
{
	return perfThread != nullptr  &&  GetMultimediaTimerMilliseconds() >= perfThread->performanceStartTime;
}


void MidiPerformance::hitNote(int8_t midiKey, int32_t timeNum, int32_t timeDenom)
{
	if (!isPerformanceRunning())
	{
		return;
	}

	int32_t closestStartTick = -1;
	QList<Note*> closestNotes;

	int32_t tick = noteLengthToTickCount(timeNum, timeDenom);

	//printf("> Note %d hit at %d, %d\n", midiKey, timeNum, timeDenom);
	//fflush(stdout);

	for (Note& note : notes)
	{
		if (!note.hit)
		{
			int32_t gracePeriod = gracePeriodFunc(note.lenNum, note.lenDenom);

			if (tick >= note.startTick-gracePeriod  &&  tick <= note.startTick+gracePeriod)
			{
				int32_t noteDist = abs(note.startTick - tick);
				int32_t closestDist = abs(closestStartTick - tick);

				if (closestStartTick < 0  ||  noteDist < closestDist)
				{
					// This note is closer than the previous closest note
					closestNotes.clear();
					closestNotes << &note;
					closestStartTick = note.startTick;
				}
				else if (note.startTick == closestStartTick)
				{
					// This note occurs at the same time as the closest note -> multiple possible notes (chord)
					closestNotes << &note;
				}
			}
		}
	}

	if (closestStartTick >= 0)
	{
		bool hit = false;

		for (Note* note : closestNotes)
		{
			if (note->midiKey == midiKey)
			{
				note->hit = true;
				reportHitNote(tick, MIDI_PERFORMANCE_LEN_WHOLE, *note);
				hit = true;
				break;
			}
		}

		if (!hit)
		{
			reportWrongNote(tick, MIDI_PERFORMANCE_LEN_WHOLE, closestNotes, midiKey);
		}
	}
	else
	{
		reportExcessNote(tick, MIDI_PERFORMANCE_LEN_WHOLE, midiKey);
	}
}


void MidiPerformance::hitNote(int8_t midiKey, uint64_t timestamp)
{
	hitNote(midiKey, getPerformanceTick(timestamp), MIDI_PERFORMANCE_LEN_WHOLE);
}


void MidiPerformance::hitNote(int8_t midiKey)
{
	hitNote(midiKey, GetMultimediaTimerMilliseconds());
}


void MidiPerformance::releaseNote(uint8_t midiKey, int32_t timeNum, int32_t timeDenom)
{
	// TODO: Implement
}


void MidiPerformance::releaseNote(uint8_t midiKey, uint64_t timestamp)
{
	releaseNote(midiKey, getPerformanceTick(timestamp), MIDI_PERFORMANCE_LEN_WHOLE);
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


void MidiPerformanceThread::eventLoopTick()
{
	uint64_t now = GetMultimediaTimerMilliseconds();

	if (now >= performanceStartTime)
	{
		bool notesOpen = false;

		int32_t currentTick = perf->getPerformanceTick();

		if (!stopped)
		{
			perf->notifyCurrentTickUpdated(currentTick, MIDI_PERFORMANCE_LEN_WHOLE);

			for (MidiPerformance::Note& note : perf->notes)
			{
				if (!note.hit  &&  !note.missed)
				{
					//notesOpen = true;

					int32_t gracePeriod = perf->gracePeriodFunc(note.lenNum, note.lenDenom);

					if (note.startTick+gracePeriod < currentTick)
					{
						note.missed = true;
						perf->reportMissedNote(note);
					}
				}

				int32_t noteEnd = note.startTick + perf->noteLengthToTickCount(note.lenNum, note.lenDenom);

				if (noteEnd > currentTick)
				{
					notesOpen = true;
				}
			}
		}

		if (!notesOpen  ||  stopped)
		{
			this->exit(0);
			return;
		}
	}
	else if (stopped)
	{
		this->exit(0);
		return;
	}

	QTimer::singleShot(0, this, SLOT(eventLoopTick()));
}


void MidiPerformanceThread::run()
{
	QTimer::singleShot(0, this, SLOT(eventLoopTick()));

	exec();

	/*bool notesOpen;

	do
	{
		notesOpen = false;

		int32_t currentTick = perf->getPerformanceTick();

		perf->notifyCurrentTickUpdated(currentTick, MIDI_PERFORMANCE_LEN_WHOLE);

		for (MidiPerformance::Note& note : perf->notes)
		{
			if (!note.hit  &&  !note.missed)
			{
				//notesOpen = true;

				int32_t gracePeriod = perf->gracePeriodFunc(note.lenNum, note.lenDenom);

				if (note.startTick+gracePeriod < currentTick)
				{
					note.missed = true;
					perf->reportMissedNote(note);
				}
			}

			int32_t noteEnd = note.startTick + perf->noteLengthToTickCount(note.lenNum, note.lenDenom);

			if (noteEnd > currentTick)
			{
				notesOpen = true;
			}
		}

		SleepMilliseconds(1);
	} while (notesOpen  &&  !stopped);*/
}


int32_t MidiPerformance::getPerformanceTick(uint64_t timestamp) const
{
	if (!isPerformanceRunning())
	{
		return -1;
	}

	//float minutesSinceStart = (timestamp - perfStartTimestamp) / 60000.0f;
	float minutesSinceStart = (timestamp - perfThread->performanceStartTime) / 60000.0f;
	return (int32_t) floorf(minutesSinceStart * ticksPerMinute + 0.5f);
}


int32_t MidiPerformance::getPerformanceTick() const
{
	return getPerformanceTick(GetMultimediaTimerMilliseconds());
}


void MidiPerformance::reportHitNote(int32_t hitNum, int32_t hitDenom, const Note& note)
{
	emit noteHit(note.midiKey, hitNum, hitDenom, note.startTick, MIDI_PERFORMANCE_LEN_WHOLE, note.lenNum, note.lenDenom, note.userData);
}


void MidiPerformance::reportMissedNote(const Note& note)
{
	emit noteMissed(note.midiKey, note.startTick, MIDI_PERFORMANCE_LEN_WHOLE, note.lenNum, note.lenDenom, note.userData);
}


void MidiPerformance::reportExcessNote(int32_t hitNum, int32_t hitDenom, int8_t midiKey)
{
	emit noteExcess(midiKey, hitNum, hitDenom);
}


void MidiPerformance::reportWrongNote(int32_t hitNum, int32_t hitDenom, const QList<Note*>& possibleNotes, int8_t playedMidiKey)
{
	// TODO: Currently not implemented separately
	reportExcessNote(hitNum, hitDenom, playedMidiKey);
	//emit noteWrong(playedMidiKey, hitNum, hitDenom);
}


void MidiPerformance::notifyCurrentTickUpdated(int32_t num, int32_t denom)
{
	emit currentTickUpdated(num, denom);
}


void MidiPerformance::perfThreadFinished()
{
	// NOTE: This routine is executed in the main thread, NOT in the performance thread

	MidiPerformanceThread* thr = (MidiPerformanceThread*) sender();

	bool stopped = thr->stopped;

	if (perfThread == thr)
	{
		perfThread = nullptr;
	}

	thr->deleteLater();

	emit performanceFinished(stopped);
}
