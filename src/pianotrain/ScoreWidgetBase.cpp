#include "ScoreWidgetBase.h"
#include "guido/guidoextensions.h"
#include "guido/ExtendedGuidoWidget.h"



ScoreWidgetBase::ScoreWidgetBase()
		: ar(nullptr), noteMarker(nullptr), perfMarkerMode(ScoreWidgetEnums::MarkAllSimultaneous), perfMarkerColor(QColor(0, 0, 255, 40)),
		  perfMarkerNum(-1), perfMarkerDenom(1)
{
}


void ScoreWidgetBase::setARHandler(ARHandler ar)
{
	if (ar != this->ar)
	{
		this->ar = ar;

		if (noteMarker)
		{
			delete noteMarker;
			noteMarker = nullptr;
		}

		if (ar)
		{
			noteMarker = new GuidoNoteMarker(ar);
		}
	}
}


ARHandler ScoreWidgetBase::getARHandler()
{
	return ar;
}


CARHandler ScoreWidgetBase::getARHandler() const
{
	return ar;
}


CGRHandler ScoreWidgetBase::getGRHandler() const
{
	return nullptr;
}


/*GRHandler ScoreWidgetBase::getGRHandler()
{
	return const_cast<GRHandler>(const_cast<const ScoreWidgetBase*>(this)->getGRHandler());
}*/


void ScoreWidgetBase::setPerformanceMarkerMode(ScoreWidgetEnums::PerformanceMarkerMode mode)
{
	perfMarkerMode = mode;
}


void ScoreWidgetBase::setPerformanceMarkerColor(const QColor& color)
{
	perfMarkerColor = color;
}


void ScoreWidgetBase::setPerformanceMarker(int32_t markerNum, int32_t markerDenom)
{
	perfMarkerNum = markerNum;
	perfMarkerDenom = markerDenom;
}


void ScoreWidgetBase::clearPerformanceMarker()
{
	perfMarkerNum = -1;
	perfMarkerDenom = 1;
}


void ScoreWidgetBase::addExcessNote(int32_t timeNum, int32_t timeDenom, int pitch, int octave)
{
	ExtendedGuidoWidget::ExcessNote note;
	note.timeNum = timeNum;
	note.timeDenom = timeDenom;
	note.pitch = pitch;
	note.octave = octave;
	excessNotes << note;
}


void ScoreWidgetBase::addExcessNote(int32_t timeNum, int32_t timeDenom, int8_t midiKey)
{
	int pitch, octave;
	GuidoMidiKeyToPitch(midiKey, pitch, octave);
	addExcessNote(timeNum, timeDenom, pitch, octave);
}


void ScoreWidgetBase::clearExcessNotes()
{
	excessNotes.clear();
}


bool ScoreWidgetBase::markCorrectPlay(int8_t midiKey, int32_t num, int32_t denom)
{
	if (noteMarker)
	{
		noteMarker->markCorrectPlay(midiKey, num, denom);
		updateARHandler();
	}
}


void ScoreWidgetBase::clearCorrectPlayMarkers()
{
	if (noteMarker)
	{
		noteMarker->clearCorrectPlayMarkers();
		updateARHandler();
	}
}


bool ScoreWidgetBase::markMissedNote(int8_t midiKey, int32_t num, int32_t denom)
{
	if (noteMarker)
	{
		noteMarker->markMissedNote(midiKey, num, denom);
		updateARHandler();
	}
}


void ScoreWidgetBase::clearMissedNoteMarkers()
{
	if (noteMarker)
	{
		noteMarker->clearMissedNoteMarkers();
		updateARHandler();
	}
}


void ScoreWidgetBase::clearMarkers()
{
	clearPerformanceMarker();
	clearCorrectPlayMarkers();
	clearMissedNoteMarkers();
	clearExcessNotes();
}


void ScoreWidgetBase::updateARHandler()
{
}
