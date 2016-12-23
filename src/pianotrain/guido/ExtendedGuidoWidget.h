#ifndef EXTENDEDGUIDOWIDGET_H_
#define EXTENDEDGUIDOWIDGET_H_

#include <GuidoQt/QGuidoWidget.h>


class ExtendedGuidoWidget : public QGuidoWidget
{
	Q_OBJECT

public:
	// TODO: A "leftmost with tolerance" mode would be nice
	enum PerformanceMarkerMode
	{
		MarkAllSimultaneous,
		MarkLeftmost
	};

public:
	ExtendedGuidoWidget(QWidget* parent = nullptr);

	void setPerformanceMarkerMode(PerformanceMarkerMode mode);
	void setPerformanceMarkerColor(const QColor& color);

	void setPerformanceMarker(int32_t markerNum, int32_t markerDenom);
	void clearPerformanceMarker();

protected:
	void paintEvent(QPaintEvent* event);

	virtual void paintPerformanceMarker(const QRect& markerRect);

private:
	QRect calculatePerformanceMarkerRect(int32_t timeNum, int32_t timeDenom);

private:
	PerformanceMarkerMode performanceMarkerMode;
	QColor performanceMarkerColor;
	int32_t performanceMarkerNum;
	int32_t performanceMarkerDenom;

	QRect lastPerformanceMarkerRect;
};

#endif /* EXTENDEDGUIDOWIDGET_H_ */
