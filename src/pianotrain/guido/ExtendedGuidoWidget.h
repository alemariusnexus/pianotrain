#ifndef EXTENDEDGUIDOWIDGET_H_
#define EXTENDEDGUIDOWIDGET_H_

#include <GuidoQt/QGuidoWidget.h>
#include "ScoreWidgetBase.h"


class GSystemQt;




class ExtendedGuidoWidget : public QGuidoWidget, public ScoreWidgetBase
{
	Q_OBJECT

public:
	ExtendedGuidoWidget(QWidget* parent = nullptr);

	virtual Q_INVOKABLE void setARHandler(ARHandler ar);

	virtual Q_INVOKABLE CGRHandler getGRHandler() const;

	virtual Q_INVOKABLE void setPerformanceMarkerMode(ScoreWidgetEnums::PerformanceMarkerMode mode);

	virtual Q_INVOKABLE void setPerformanceMarker(int32_t markerNum, int32_t markerDenom);
	virtual Q_INVOKABLE void clearPerformanceMarker();

	virtual Q_INVOKABLE void addExcessNote(int32_t timeNum, int32_t timeDenom, int pitch, int octave);
	virtual Q_INVOKABLE void addExcessNote(int32_t timeNum, int32_t timeDenom, int8_t midiKey);
	virtual Q_INVOKABLE void clearExcessNotes();

	virtual Q_INVOKABLE void updateARHandler();

	virtual Q_INVOKABLE void setBlanked(bool blanked);
	virtual Q_INVOKABLE void setOverlayColor(const QColor& overlayColor);
	virtual Q_INVOKABLE void setSingleLineMode(bool singleLineMode);

public:
	QSize sizeHint() const;

protected:
	void paintEvent(QPaintEvent* event);
	void resizeEvent(QResizeEvent* evt);

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

	bool singleLineMode;
	bool blanked;
	QColor overlayColor;

	/*QPainter* guidoPainter;
	GSystemQt* guidoSys;
	VGDevice* guidoDev;*/
};

#endif /* EXTENDEDGUIDOWIDGET_H_ */
