#ifndef PIANOTRAIN_SCOREWIDGETBASE_H_
#define PIANOTRAIN_SCOREWIDGETBASE_H_

#include <pianotrain/config.h>
#include <QtCore/QList>
#include <QColor>
#include <Guido/GUIDOEngine.h>
#include "guido/GuidoNoteMarker.h"



class ScoreWidgetBase
{
public:
	// TODO: A "leftmost with tolerance" mode would be nice
	enum PerformanceMarkerMode
	{
		MarkAllSimultaneous,
		MarkLeftmost
	};

protected:
	struct ExcessNote
	{
		int32_t timeNum;
		int32_t timeDenom;
		int pitch;
		int octave;
	};

public:
	ScoreWidgetBase();

	virtual void setARHandler(ARHandler ar);
	ARHandler getARHandler();
	CARHandler getARHandler() const;

	//virtual GRHandler getGRHandler();
	virtual CGRHandler getGRHandler() const = 0;

	virtual void setPerformanceMarkerMode(PerformanceMarkerMode mode);
	virtual void setPerformanceMarkerColor(const QColor& color);

	virtual void setPerformanceMarker(int32_t markerNum, int32_t markerDenom);
	virtual void clearPerformanceMarker();

	virtual void addExcessNote(int32_t timeNum, int32_t timeDenom, int pitch, int octave);
	virtual void addExcessNote(int32_t timeNum, int32_t timeDenom, int8_t midiKey);
	virtual void clearExcessNotes();

	virtual bool markCorrectPlay(int8_t midiKey, int32_t num, int32_t denom);
	virtual void clearCorrectPlayMarkers();

	virtual bool markMissedNote(int8_t midiKey, int32_t num, int32_t denom);
	virtual void clearMissedNoteMarkers();

	virtual void clearMarkers();

	virtual void updateARHandler();

protected:
	ARHandler ar;

	GuidoNoteMarker* noteMarker;

	PerformanceMarkerMode perfMarkerMode;
	QColor perfMarkerColor;
	int32_t perfMarkerNum;
	int32_t perfMarkerDenom;

	QList<ExcessNote> excessNotes;
};

#endif /* PIANOTRAIN_SCOREWIDGETBASE_H_ */
