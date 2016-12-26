#include "ScoreWidget.h"
#include "guido/guidoextensions.h"



ScoreWidget::ScoreWidget(QWidget* parent)
		: QWidget(parent)
{
}


/*void ScoreWidget::setARHandler(ARHandler ar)
{
	this->ar = ar;
}


ARHandler ScoreWidget::getARHandler()
{
	return ar;
}


CARHandler ScoreWidget::getARHandler() const
{
	return ar;
}


GRHandler ScoreWidget::getGRHandler()
{
	return const_cast<GRHandler>(const_cast<const ScoreWidget*>(this)->getGRHandler());
}


void ScoreWidget::setPerformanceMarkerMode(PerformanceMarkerMode mode)
{
	perfMarkerMode = mode;
}


void ScoreWidget::setPerformanceMarkerColor(const QColor& color)
{
	perfMarkerColor = color;
}


void ScoreWidget::setPerformanceMarker(int32_t markerNum, int32_t markerDenom)
{
	perfMarkerNum = markerNum;
	perfMarkerDenom = markerDenom;
}


void ScoreWidget::clearPerformanceMarker()
{
	perfMarkerNum = -1;
	perfMarkerDenom = 1;
}


void ScoreWidget::addExcessNote(int32_t timeNum, int32_t timeDenom, int pitch, int octave)
{
	ExtendedGuidoWidget::ExcessNote note;
	note.timeNum = timeNum;
	note.timeDenom = timeDenom;
	note.pitch = pitch;
	note.octave = octave;
	excessNotes << note;
}


void ScoreWidget::addExcessNote(int32_t timeNum, int32_t timeDenom, int8_t midiKey)
{
	int pitch, octave;
	GuidoMidiKeyToPitch(midiKey, pitch, octave);
	addExcessNote(timeNum, timeDenom, pitch, octave);
}


void ScoreWidget::clearExcessNotes()
{
	excessNotes.clear();
	update();
}*/
