#ifndef EXTENDEDGUIDOWIDGET_H_
#define EXTENDEDGUIDOWIDGET_H_

#include <GuidoQt/QGuidoWidget.h>
#include "ScoreWidgetBase.h"


class GSystemQt;




//class ExtendedGuidoWidget : public QGuidoWidget
class ExtendedGuidoWidget : public QGuidoWidget, public ScoreWidgetBase
{
	Q_OBJECT

public:
	ExtendedGuidoWidget(QWidget* parent = nullptr);

	virtual void setARHandler(ARHandler ar);

	virtual CGRHandler getGRHandler() const;

	virtual void setPerformanceMarkerMode(PerformanceMarkerMode mode);

	virtual void setPerformanceMarker(int32_t markerNum, int32_t markerDenom);
	virtual void clearPerformanceMarker();

	virtual void addExcessNote(int32_t timeNum, int32_t timeDenom, int pitch, int octave);
	virtual void addExcessNote(int32_t timeNum, int32_t timeDenom, int8_t midiKey);
	virtual void clearExcessNotes();

	virtual void updateARHandler();

	virtual void setBlanked(bool blanked);
	virtual void setOverlayColor(const QColor& overlayColor);

public:
	QSize sizeHint() const;

protected:
	void paintEvent(QPaintEvent* event);

	virtual void paintPerformanceMarker(const QRect& markerRect);

private:
	QRect calculatePerformanceMarkerRect(int32_t timeNum, int32_t timeDenom);
	void paintOverlay();

private:
	/*PerformanceMarkerMode performanceMarkerMode;
	QColor performanceMarkerColor;
	int32_t performanceMarkerNum;
	int32_t performanceMarkerDenom;
	QList<ExcessNote> excessNotes;*/

	QRect lastPerformanceMarkerRect;

	bool blanked;
	QColor overlayColor;

	/*QPainter* guidoPainter;
	GSystemQt* guidoSys;
	VGDevice* guidoDev;*/
};

#endif /* EXTENDEDGUIDOWIDGET_H_ */
