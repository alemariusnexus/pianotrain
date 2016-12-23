#ifndef EXTENDEDGUIDOWIDGET_H_
#define EXTENDEDGUIDOWIDGET_H_

#include <GuidoQt/QGuidoWidget.h>


class ExtendedGuidoWidget : public QGuidoWidget
{
	Q_OBJECT

public:
	ExtendedGuidoWidget(QWidget* parent = nullptr);

	void setPerformanceMarker(int32_t markerNum, int32_t markerDenom);
	void clearPerformanceMarker();

private:
	QRect calculatePerformanceMarkerRect(int32_t timeNum, int32_t timeDenom);

protected:
	void paintEvent(QPaintEvent* event);

private:
	int32_t performanceMarkerNum;
	int32_t performanceMarkerDenom;

	QRect lastPerformanceMarkerRect;
};

#endif /* EXTENDEDGUIDOWIDGET_H_ */
