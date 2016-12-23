#include "GuidoNoteMarker.h"
#include "guidoextensions.h"

#include <Guido/abstract/ARMusic.h>
#include <Guido/abstract/ARMusicalVoice.h>
#include <Guido/abstract/ARMusicalVoiceState.h>
#include <Guido/abstract/ARNote.h>
#include <Guido/abstract/ARNoteFormat.h>
#include <Guido/abstract/ARRestFormat.h>
#include <Guido/lib/GUIDOInternal.h>

#include <iostream>



//#define GNM_ENABLE_DEBUG




GuidoNoteMarker::GuidoNoteMarker(ARHandler ar, QObject* parent)
		: QObject(parent), ar(ar), perfMarkerNoteColor(QColor(0, 255, 0)), perfMarkerRestColor(QColor(0, 255, 0, 127)),
		  corrPlayColor(QColor(0, 255, 0)), missedNoteColor(QColor(255, 0, 0))
{
}


bool GuidoNoteMarker::findTagBounds (
		TYPE_TIMEPOSITION& startTimepos, TYPE_TIMEPOSITION& endTimepos,
		GuidoPos& startPos, GuidoPos& endPos,
		ARMusicalVoice* voice,
		const std::function<bool(ARMusicalObject*, TYPE_TIMEPOSITION, GuidoPos, void*)>& startFilter,
		const std::function<bool(ARMusicalObject*, TYPE_TIMEPOSITION, GuidoPos, void*)>& endFilter,
		bool chordMode,
		void* state
) {
	// Implementation is based on Guidolib's ARMusicalVoice::MarkVoice() method

	ARMusicalVoice::_readmode oldReadMode = voice->getReadMode();

	if (chordMode)
	{
		voice->setReadMode(ARMusicalVoice::CHORDMODE);
	}
	else
	{
		voice->setReadMode(ARMusicalVoice::EVENTMODE);
	}

	ARCustomMusicalVoiceState vst;

	voice->GetHeadPosition(vst);

	startPos = nullptr;
	endPos = nullptr;

#ifdef GNM_ENABLE_DEBUG
	printf("\n");
#endif

	// Read in the AR structure until we find the note positions where formatting should start/end
	while (vst.getVpos())
	{
		GuidoPos prevpos = vst.getVpos();
		const TYPE_TIMEPOSITION tp (vst.getCurtp());

		ARMusicalObject * o = voice->GetNext(vst.getVpos(), vst);

#ifdef GNM_ENABLE_DEBUG
		printf("Going through at %d, %d:   ", tp.getNumerator(), tp.getDenominator());
		o->print(std::cout);
#endif

		if (endFilter(o, tp, prevpos, state))
		{
			endTimepos = tp;
			endPos = prevpos;

#ifdef GNM_ENABLE_DEBUG
			printf("  -> new end!\n");
#endif

			break;
		}

		if (startFilter(o, tp, prevpos, state))
		{
			startTimepos = tp;
			startPos = prevpos;

#ifdef GNM_ENABLE_DEBUG
			printf("  -> new start!\n");
#endif
		}
	}

#ifdef GNM_ENABLE_DEBUG
	printf("\n");
	fflush(stdout);
#endif

	voice->setReadMode(oldReadMode);

	return startPos != nullptr;
}


void GuidoNoteMarker::formatNotes (
		TYPE_TIMEPOSITION startTimepos, TYPE_TIMEPOSITION endTimepos,
		GuidoPos startPos, GuidoPos endPos,
		ARMusicalVoice* voice,
		const QColor& color,
		QList<ARMusicalObject*>& formatList
) {
	if (startPos)
	{
		// !!! IMPORTANT !!!
		// Do NOT use setRGBColor() below. You would expect it to do the same as setColor(), but it doesn't. setRGBColor()
		// behaves strangely with chords, where it colors the note heads but not the stems. Don't ask me why...

		QString colorStr = buildGuidoColorString(color);

		ARNoteFormat * ntformat = new ARNoteFormat;
		ntformat->setRelativeTimePosition(startTimepos);
		ntformat->setColor(colorStr.toUtf8().constData());
		voice->AddElementAt(startPos, ntformat);
		formatList << ntformat;
	}
	if (endPos)
	{
		// NOTE: activePosEnd might be NULL when we're right at the end of the score. In that case, it doesn't seem necessary
		// to restore the old format. Also, trying to restore the old format in that case proved to be difficult because
		// Guidolib behaves weird with things like chords.

		ARNoteFormat * ntformat = new ARNoteFormat;
		ntformat->setRelativeTimePosition(endTimepos);
		voice->AddElementAt(endPos, ntformat);
		formatList << ntformat;
	}
}


void GuidoNoteMarker::formatRests (
		TYPE_TIMEPOSITION startTimepos, TYPE_TIMEPOSITION endTimepos,
		GuidoPos startPos, GuidoPos endPos,
		ARMusicalVoice* voice,
		const QColor& color,
		QList<ARMusicalObject*>& formatList
) {
	if (startPos)
	{
		// !!! IMPORTANT !!!
		// Do NOT use setRGBColor() below. You would expect it to do the same as setColor(), but it doesn't. setRGBColor()
		// behaves strangely with chords, where it colors the note heads but not the stems. Don't ask me why...

		QString colorStr = buildGuidoColorString(color);

		ARRestFormat * rformat = new ARRestFormat;
		rformat->setRelativeTimePosition(startTimepos);
		rformat->setColor(colorStr.toUtf8().constData());
		voice->AddElementAt(startPos, rformat);
		formatList << rformat;
	}
	if (endPos)
	{
		// NOTE: activePosEnd might be NULL when we're right at the end of the score. In that case, it doesn't seem necessary
		// to restore the old format. Also, trying to restore the old format in that case proved to be difficult because
		// Guidolib behaves weird with things like chords.

		ARRestFormat * rformat = new ARRestFormat;
		rformat->setRelativeTimePosition(endTimepos);
		voice->AddElementAt(endPos, rformat);
		formatList << rformat;
	}
}


void GuidoNoteMarker::setPerformanceMarkerColors(const QColor& noteColor, const QColor& restColor)
{
	perfMarkerNoteColor = noteColor;
	perfMarkerRestColor = restColor;
}


void GuidoNoteMarker::setCorrectPlayColor(const QColor& color)
{
	corrPlayColor = color;
}


bool GuidoNoteMarker::setPerformanceMarker(ARMusicalVoice* voice, int32_t num, int32_t denom)
{
	TYPE_TIMEPOSITION activeTimepos(num, denom);

	TYPE_TIMEPOSITION activeTimeposStart;
	TYPE_TIMEPOSITION activeTimeposEnd;

	GuidoPos activePosStart = nullptr;
	GuidoPos activePosEnd = nullptr;

	struct State
	{
		TYPE_TIMEPOSITION lastStartTimepos;
		GuidoPos lastStartPos;
	};

	State state;
	state.lastStartPos = nullptr;

	auto startFilter = [activeTimepos](ARMusicalObject* note, TYPE_TIMEPOSITION timepos, GuidoPos pos, void* vstate) {
		State* state = (State*) vstate;

		if (ARMusicalEvent::cast(note)  &&  timepos <= activeTimepos)
		{
			// Object lies before our active position

			// There may be multiple objects at the same time position. In that case, we will start before
			// the first of those objects
			if (!state->lastStartPos  ||  state->lastStartTimepos != timepos)
			{
				state->lastStartTimepos = timepos;
				state->lastStartPos = pos;
				return true;
			}
		}

		return false;
	};

	auto endFilter = [activeTimepos](ARMusicalObject* note, TYPE_TIMEPOSITION timepos, GuidoPos pos, void* vstate) {
		return timepos > activeTimepos;
	};

	findTagBounds(activeTimeposStart, activeTimeposEnd, activePosStart, activePosEnd, voice, startFilter, endFilter, true, &state);


	// Insert format tags into AR structure

	int vnum = voice->getVoiceNum();

	if (curPerfMarkerStartTimepos.contains(vnum)  &&  curPerfMarkerEndTimepos.contains(vnum))
	{
		if (activeTimeposStart == curPerfMarkerStartTimepos[vnum]  &&  activeTimeposEnd == curPerfMarkerEndTimepos[vnum])
		{
			return false;
		}
	}

	QList<ARMusicalObject*> newTags;
	formatNotes(activeTimeposStart, activeTimeposEnd, activePosStart, activePosEnd, voice, perfMarkerNoteColor, newTags);
	formatRests(activeTimeposStart, activeTimeposEnd, activePosStart, activePosEnd, voice, perfMarkerRestColor, newTags);

	// Clear the old markers now. Don't do it before adding the new ones, because pointers might become invalid!
	clearPerformanceMarker(voice);

	curPerfMarkerObjs << newTags;

	curPerfMarkerStartTimepos[vnum] = activeTimeposStart;
	curPerfMarkerEndTimepos[vnum] = activeTimeposEnd;

	return true;
}


bool GuidoNoteMarker::findNoteTagBounds (
		TYPE_TIMEPOSITION& startTimepos, TYPE_TIMEPOSITION& endTimepos,
		GuidoPos& startPos, GuidoPos& endPos,
		ARMusicalVoice* voice,
		const std::function<bool(ARNote*, TYPE_TIMEPOSITION)>& filter,
		bool chordMode
) {
	struct State
	{
		bool startFound;
		TYPE_TIMEPOSITION timeposBeforeChord;
		bool chordCommaActive;
	};

	State state;
	state.startFound = false;
	state.chordCommaActive = false;

	auto startFilter = [filter](ARMusicalObject* obj, TYPE_TIMEPOSITION timepos, GuidoPos pos, void* vstate) {
		State* state = (State*) vstate;

		if (obj->isARChordComma())
		{
			state->chordCommaActive = true;
		}
		else if (obj->isARNote())
		{
			ARNote* note = (ARNote*) ARNote::cast(obj);

			TYPE_TIMEPOSITION actualTimepos = timepos;

			if (state->chordCommaActive)
			{
				// Currently inside a chord -> use timepos of the "empty" note before the chord start and keep it
				actualTimepos = state->timeposBeforeChord;

#ifdef GNM_ENABLE_DEBUG
				printf("  -> note in chord\n");
#endif
			}
			else
			{
				// Not inside a chord -> use this note's timepos as new "before chord" value
				state->timeposBeforeChord = timepos;

#ifdef GNM_ENABLE_DEBUG
				printf("  -> note outside chord\n");
#endif
			}

			// This note "consumes" the chord comma
			state->chordCommaActive = false;

#ifdef GNM_ENABLE_DEBUG
			printf("  -> using %d, %d\n", actualTimepos.getNumerator(), actualTimepos.getDenominator());
#endif

			if (filter(note, actualTimepos))
			{
#ifdef GNM_ENABLE_DEBUG
				printf("  -> filter matches!\n");
#endif

				state->startFound = true;
				return true;
			}
		}
		else if (ARMusicalEvent::cast(obj))
		{
			state->chordCommaActive = false;
		}

		return false;
	};

	auto endFilter = [](ARMusicalObject* obj, TYPE_TIMEPOSITION timepos, GuidoPos pos, void* vstate) {
		State* state = (State*) vstate;
		return state->startFound;
	};

	return findTagBounds(startTimepos, endTimepos, startPos, endPos, voice, startFilter, endFilter, chordMode, &state);
}


void GuidoNoteMarker::clearPerformanceMarker()
{
	removeMusicalObjects(curPerfMarkerObjs);
	curPerfMarkerObjs.clear();

	curPerfMarkerStartTimepos.clear();
	curPerfMarkerEndTimepos.clear();
}


void GuidoNoteMarker::clearPerformanceMarker(ARMusicalVoice* voice)
{
	QList<ARMusicalObject*> newPerfMarkerObjs;

	// Remove previous format markers
	for (ARMusicalObject* obj : curPerfMarkerObjs)
	{
		if (!voice->RemoveElement(obj))
		{
			newPerfMarkerObjs << obj;
		}
	}

	curPerfMarkerObjs = newPerfMarkerObjs;
}


bool GuidoNoteMarker::setPerformanceMarker(int32_t num, int32_t denom)
{
	bool newMarkers = false;

	ARMusic* music = ar->armusic;

	GuidoPos pos = music->GetHeadPosition();
	while (pos)
	{
		ARMusicalVoice * arvc = music->GetNext(pos);

		newMarkers |= setPerformanceMarker(arvc, num, denom);
	}

	return newMarkers;
}


bool GuidoNoteMarker::markSingleNote (
		int8_t midiKey,
		int32_t num, int32_t denom,
		const QColor& color,
		QList<ARMusicalObject*>& formatList
) {
	ARMusic* music = ar->armusic;

	GuidoPos pos = music->GetHeadPosition();
	while (pos)
	{
		ARMusicalVoice * arvc = music->GetNext(pos);

		if (markSingleNote(arvc, midiKey, num, denom, color, formatList))
		{
			return true;
		}
	}

	return false;
}


bool GuidoNoteMarker::markSingleNote (
		ARMusicalVoice* voice,
		int8_t midiKey,
		int32_t num, int32_t denom,
		const QColor& color,
		QList<ARMusicalObject*>& formatList
) {
	TYPE_TIMEPOSITION targetTimepos(num, denom);

	TYPE_TIMEPOSITION startTimepos;
	TYPE_TIMEPOSITION endTimepos;

	GuidoPos startPos = nullptr;
	GuidoPos endPos = nullptr;

	auto filter = [targetTimepos, midiKey](ARNote* note, TYPE_TIMEPOSITION actualTimepos) {
		return actualTimepos == targetTimepos  &&  note->getMidiPitch() == midiKey;
	};

	if (!findNoteTagBounds(startTimepos, endTimepos, startPos, endPos, voice, filter, false))
	{
		return false;
	}

	formatNotes(startTimepos, endTimepos, startPos, endPos, voice, color, formatList);

	return startPos != nullptr;
}


bool GuidoNoteMarker::markCorrectPlay(int8_t midiKey, int32_t num, int32_t denom)
{
	return markSingleNote(midiKey, num, denom, corrPlayColor, corrPlayMarkerObjs);
}


bool GuidoNoteMarker::markMissedNote(int8_t midiKey, int32_t num, int32_t denom)
{
	return markSingleNote(midiKey, num, denom, missedNoteColor, missedNoteMarkerObjs);
}


void GuidoNoteMarker::removeMusicalObjects(const QList<ARMusicalObject*>& objs)
{
	ARMusic* music = ar->armusic;

	GuidoPos pos = music->GetHeadPosition();
	while (pos)
	{
		ARMusicalVoice * arvc = music->GetNext(pos);

		// Remove previous format markers
		for (ARMusicalObject* obj : objs)
		{
			arvc->RemoveElement(obj);
		}
	}
}


void GuidoNoteMarker::clearCorrectPlayMarkers()
{
	removeMusicalObjects(corrPlayMarkerObjs);
	corrPlayMarkerObjs.clear();
}


void GuidoNoteMarker::clearMissedNoteMarkers()
{
	removeMusicalObjects(missedNoteMarkerObjs);
	missedNoteMarkerObjs.clear();
}


void GuidoNoteMarker::clear()
{
	clearPerformanceMarker();
	clearCorrectPlayMarkers();
	clearMissedNoteMarkers();
}


QString GuidoNoteMarker::buildGuidoColorString(const QColor& color) const
{
	char buf[11];
	sprintf(buf, "0x%02X%02X%02X%02X", color.red(), color.green(), color.blue(), color.alpha());
	return QString(buf);
}
