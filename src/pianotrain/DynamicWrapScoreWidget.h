#ifndef PIANOTRAIN_DYNAMICWRAPSCOREWIDGET_H_
#define PIANOTRAIN_DYNAMICWRAPSCOREWIDGET_H_

#include "ScoreWidget.h"
#include <ui_DynamicWrapScoreWidget.h>
#include <QtCore/QList>
#include <QWidget>
#include <QStackedWidget>
#include <QBoxLayout>
#include "guido/ExtendedGuidoWidget.h"


class DynamicWrapScoreWidget : public ScoreWidget
{
	Q_OBJECT

	Q_ENUMS(_PerformanceMarkerMode)

public:
	enum _PerformanceMarkerMode
	{
		MarkNone,
		MarkAllSimultaneous,
		MarkLeftmost,
		MarkMeasure
	};

public:
	DynamicWrapScoreWidget(QWidget* parent = nullptr);

	virtual Q_INVOKABLE void setARHandler(ARHandler ar);

	virtual Q_INVOKABLE CGRHandler getGRHandler() const;

	virtual Q_INVOKABLE void setPerformanceMarkerMode(ScoreWidgetEnums::PerformanceMarkerMode mode);
	virtual Q_INVOKABLE void setPerformanceMarkerColor(const QColor& color);

	virtual Q_INVOKABLE void setPerformanceMarker(int32_t markerNum, int32_t markerDenom);
	virtual Q_INVOKABLE void clearPerformanceMarker();

	virtual Q_INVOKABLE void addExcessNote(int32_t timeNum, int32_t timeDenom, int pitch, int octave);
	virtual Q_INVOKABLE void addExcessNote(int32_t timeNum, int32_t timeDenom, int8_t midiKey);
	virtual Q_INVOKABLE void clearExcessNotes();

	Q_INVOKABLE void setLineCount(int lineCount);

	Q_INVOKABLE int getActivePage() const;
	Q_INVOKABLE void setActivePage(int pageNum);

	Q_INVOKABLE void setPreviousPageCount(int numPreviousPages);

	Q_INVOKABLE void setActiveLineOverlayColor(const QColor& color);
	Q_INVOKABLE void setInactiveLineOverlayColor(const QColor& color);

	virtual Q_INVOKABLE void updateARHandler();

	Q_INVOKABLE void setShowFullScore(bool showFullScore);

private:
	int getPageCount() const;

private:
	Ui_DynamicWrapScoreWidget ui;

	QList<ExtendedGuidoWidget*> lineWidgets;
	int activePage;
	int numPreviousPages;

	QColor activeLineOverlayColor;
	QColor inactiveLineOverlayColor;
};

#endif /* PIANOTRAIN_DYNAMICWRAPSCOREWIDGET_H_ */
