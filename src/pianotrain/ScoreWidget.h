#ifndef PIANOTRAIN_SCOREWIDGET_H_
#define PIANOTRAIN_SCOREWIDGET_H_

#include <pianotrain/config.h>
#include <QtCore/QList>
#include <QWidget>
#include <QColor>
#include <Guido/GUIDOEngine.h>
#include "guido/ExtendedGuidoWidget.h"
#include "ScoreWidgetBase.h"


//class ScoreWidget : public QWidget
class ScoreWidget : public QWidget, public ScoreWidgetBase
{
	Q_OBJECT

public:
	ScoreWidget(QWidget* parent = nullptr);

	/*virtual void setARHandler(ARHandler ar);
	ARHandler getARHandler();
	CARHandler getARHandler() const;

	virtual GRHandler getGRHandler();
	virtual CGRHandler getGRHandler() const = 0;

	virtual void setPerformanceMarkerMode(PerformanceMarkerMode mode);
	virtual void setPerformanceMarkerColor(const QColor& color);

	virtual void setPerformanceMarker(int32_t markerNum, int32_t markerDenom);
	virtual void clearPerformanceMarker();

	virtual void addExcessNote(int32_t timeNum, int32_t timeDenom, int pitch, int octave);
	virtual void addExcessNote(int32_t timeNum, int32_t timeDenom, int8_t midiKey);
	virtual void clearExcessNotes();*/

protected:
	/*ARHandler ar;

	PerformanceMarkerMode perfMarkerMode;
	QColor perfMarkerColor;
	int32_t perfMarkerNum;
	int32_t perfMarkerDenom;

	QList<ExtendedGuidoWidget::ExcessNote> excessNotes;*/
};

#endif /* PIANOTRAIN_SCOREWIDGET_H_ */
