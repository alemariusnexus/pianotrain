#include "GuidoNoteMarker.h"

#include <Guido/abstract/ARMusic.h>
#include <Guido/abstract/ARMusicalVoice.h>
#include <Guido/abstract/ARMusicalVoiceState.h>
#include <Guido/abstract/ARNoteFormat.h>
#include <Guido/abstract/ARRestFormat.h>
#include <Guido/lib/GUIDOInternal.h>

#include <iostream>



#define GNM_ENABLE_DEBUG



// We need access to some protected member variables of ARMusicalVoiceState, so we have to
// use a subclass to expose them.
// It's dirty, but it works...
class ARCustomMusicalVoiceState : public ARMusicalVoiceState
{
public:
	GuidoPos& getVpos() { return vpos; }
	TYPE_TIMEPOSITION getCurtp() { return curtp; }
};





GuidoNoteMarker::GuidoNoteMarker(ARHandler ar, QObject* parent)
		: QObject(parent), ar(ar), perfMarkerNoteColor(QColor(0, 255, 0)), perfMarkerRestColor(QColor(0, 255, 0, 127))
{
}


void GuidoNoteMarker::clearPerformanceMarker()
{
	// TODO: Make this configurable, or whatever
	int voicenum = 1;

	ARMusic* music = ar->armusic;

	GuidoPos pos = music->GetHeadPosition();
	while (pos)
	{
		ARMusicalVoice * arvc = music->GetNext(pos);
		if (arvc->getVoiceNum() == voicenum)
		{
			// Remove previous format markers
			for (ARMusicalObject* obj : curPerfMarkerObjs)
			{
				arvc->RemoveElement(obj);
			}
			curPerfMarkerObjs.clear();

			break;
		}
	}
}


void GuidoNoteMarker::setPerformanceMarker(int32_t num, int32_t denom)
{
	// TODO: Make this configurable, or whatever
	int voicenum = 1;

	ARMusic* music = ar->armusic;

	GuidoPos pos = music->GetHeadPosition();
	while (pos)
	{
		ARMusicalVoice * arvc = music->GetNext(pos);
		if (arvc->getVoiceNum() == voicenum)
		{
			setPerformanceMarker(arvc, num, denom);
			break;
		}
	}
}


void GuidoNoteMarker::setPerformanceMarkerColors(const QColor& noteColor, const QColor& restColor)
{
	perfMarkerNoteColor = noteColor;
	perfMarkerRestColor = restColor;
}


void GuidoNoteMarker::setPerformanceMarker(ARMusicalVoice* voice, int32_t num, int32_t denom)
{
	// Implementation is based on Guidolib's ARMusicalVoice::MarkVoice() method

	ARMusicalVoice::_readmode oldReadMode = voice->getReadMode();

	// Skip over chord-related stuff. It's easier this way...
	voice->setReadMode(ARMusicalVoice::CHORDMODE);

	clearPerformanceMarker();

	TYPE_TIMEPOSITION tpos(num, denom);

	ARCustomMusicalVoiceState vst;

	voice->GetHeadPosition(vst);

	TYPE_TIMEPOSITION activeTimeposStart;
	TYPE_TIMEPOSITION activeTimeposEnd;

	GuidoPos activePosStart = nullptr;
	GuidoPos activePosEnd = nullptr;

#ifdef GNM_ENABLE_DEBUG
	printf("\n");
#endif

	// Read in the AR structure until we find the note positions where formatting should start/end
	while (vst.getVpos())
	{
		GuidoPos prevpos = vst.getVpos();
		const TYPE_TIMEPOSITION tp (vst.getCurtp());

		ARMusicalObject * o = voice->GetNext(vst.getVpos(), vst);
		ARMusicalObject * oEv = ARMusicalEvent::cast(o);

		if (oEv)
		{
			if (tp <= tpos)
			{
				// Object lies before our active position

				// There may be multiple objects at the same time position. In that case, we will start before
				// the first of those objects
				if (!activePosStart  ||  activeTimeposStart != tp)
				{
					activeTimeposStart = tp;
					activePosStart = prevpos;

#ifdef GNM_ENABLE_DEBUG
					printf("  -> new start!\n");
#endif
				}
			}
			else
			{
				// Object lies after our active position
				activeTimeposEnd = tp;
				activePosEnd = prevpos;

#ifdef GNM_ENABLE_DEBUG
				printf("  -> new end!\n");
#endif

				break;
			}
		}
	}

#ifdef GNM_ENABLE_DEBUG
	printf("\n");
	fflush(stdout);
#endif


	// Insert format tags into AR structure

	if (activePosStart)
	{
		// !!! IMPORTANT !!!
		// Do NOT use setRGBColor() below. You would expect it to do the same as setColor(), but it doesn't. setRGBColor()
		// behaves strangely with chords, where it colors the note heads but not the stems. Don't ask me why...

		QString noteColorStr = buildGuidoColorString(perfMarkerNoteColor);
		QString restColorStr = buildGuidoColorString(perfMarkerRestColor);

		ARNoteFormat * ntformat = new ARNoteFormat;
		ntformat->setRelativeTimePosition(activeTimeposStart);
		ntformat->setColor(noteColorStr.toUtf8().constData());
		voice->AddElementAt(activePosStart, ntformat);
		curPerfMarkerObjs << ntformat;

		ARRestFormat* rformat = new ARRestFormat;
		rformat->setRelativeTimePosition(activeTimeposStart);
		rformat->setColor(restColorStr.toUtf8().constData());
		voice->AddElementAt(activePosStart, rformat);
		curPerfMarkerObjs << rformat;
	}
	if (activePosEnd)
	{
		// NOTE: activePosEnd might be NULL when we're right at the end of the score. In that case, it doesn't seem necessary
		// to restore the old format. Also, trying to restore the old format in that case proved to be difficult because
		// Guidolib behaves weird with things like chords.

		ARNoteFormat * ntformat = new ARNoteFormat;
		ntformat->setRelativeTimePosition(activeTimeposEnd);
		voice->AddElementAt(activePosEnd, ntformat);
		curPerfMarkerObjs << ntformat;

		ARRestFormat* rformat = new ARRestFormat;
		rformat->setRelativeTimePosition(activeTimeposEnd);
		voice->AddElementAt(activePosEnd, rformat);
		curPerfMarkerObjs << rformat;
	}

	voice->setReadMode(oldReadMode);
}


QString GuidoNoteMarker::buildGuidoColorString(const QColor& color) const
{
	char buf[11];
	sprintf(buf, "0x%02X%02X%02X%02X", color.red(), color.green(), color.blue(), color.alpha());
	return QString(buf);
}
