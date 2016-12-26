#ifndef PIANOTRAIN_DYNAMICWRAPSCOREWIDGET_H_
#define PIANOTRAIN_DYNAMICWRAPSCOREWIDGET_H_

#include "ScoreWidget.h"
#include <QtCore/QList>
#include <QWidget>
#include <QBoxLayout>
#include "guido/ExtendedGuidoWidget.h"


class DynamicWrapScoreWidget : public ScoreWidget
{
	Q_OBJECT

public:
	DynamicWrapScoreWidget(QWidget* parent = nullptr);

	virtual void setARHandler(ARHandler ar);

	virtual CGRHandler getGRHandler() const;

	virtual void setPerformanceMarkerMode(PerformanceMarkerMode mode);
	virtual void setPerformanceMarkerColor(const QColor& color);

	virtual void setPerformanceMarker(int32_t markerNum, int32_t markerDenom);
	virtual void clearPerformanceMarker();

	virtual void addExcessNote(int32_t timeNum, int32_t timeDenom, int pitch, int octave);
	virtual void addExcessNote(int32_t timeNum, int32_t timeDenom, int8_t midiKey);
	virtual void clearExcessNotes();

	void setLineCount(int lineCount);

	int getActivePage() const;
	void setActivePage(int pageNum);

	void setPreviousPageCount(int numPreviousPages);

	void setActiveLineOverlayColor(const QColor& color);
	void setInactiveLineOverlayColor(const QColor& color);

	virtual void updateARHandler();

private:
	int getPageCount() const;

private:
	QList<ExtendedGuidoWidget*> lineWidgets;
	QVBoxLayout* mainLayout;
	int activePage;
	int numPreviousPages;

	QColor activeLineOverlayColor;
	QColor inactiveLineOverlayColor;
};

#endif /* PIANOTRAIN_DYNAMICWRAPSCOREWIDGET_H_ */
