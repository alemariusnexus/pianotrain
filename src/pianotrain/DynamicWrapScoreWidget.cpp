#include "DynamicWrapScoreWidget.h"
#include <QSizePolicy>
#include <Guido/lib/GUIDOInternal.h>
#include <Guido/abstract/ARMusic.h>
#include <algorithm>



DynamicWrapScoreWidget::DynamicWrapScoreWidget(QWidget* parent)
		: ScoreWidget(parent), activePage(1), numPreviousPages(0),
		  activeLineOverlayColor(QColor(0, 0, 0, 0)), inactiveLineOverlayColor(QColor(0, 0, 0, 20))
{
	mainLayout = new QVBoxLayout(this);
	setLayout(mainLayout);

	setLineCount(2);
	setActivePage(1);
}


int DynamicWrapScoreWidget::getPageCount() const
{
	if (lineWidgets.isEmpty())
	{
		return 0;
	}

	return lineWidgets[0]->pageCount();
}


void DynamicWrapScoreWidget::setLineCount(int lineCount)
{
	if (lineWidgets.size() != lineCount)
	{
		for (ExtendedGuidoWidget* widget : lineWidgets)
		{
			mainLayout->removeWidget(widget);
			delete widget;
		}

		lineWidgets.clear();

		for (int i = 0 ; i < lineCount ; i++)
		{
			ExtendedGuidoWidget* guidoWidget = new ExtendedGuidoWidget(this);

			QSizePolicy policy(QSizePolicy::Expanding, QSizePolicy::Preferred);
			policy.setHeightForWidth(true);
			guidoWidget->setSizePolicy(policy);

			mainLayout->addWidget(guidoWidget);

			lineWidgets << guidoWidget;
		}

		setARHandler(ar);
	}
}


void DynamicWrapScoreWidget::setARHandler(ARHandler ar)
{
	ScoreWidget::setARHandler(ar);

	for (ExtendedGuidoWidget* widget : lineWidgets)
	{
		if (ar)
		{
			ar->refCount += 2;
		}

		widget->setARHandler(ar);

		if (ar)
		{
			ar->refCount -= 1;
		}
	}

	setActivePage(getActivePage());
}


CGRHandler DynamicWrapScoreWidget::getGRHandler() const
{
	if (lineWidgets.isEmpty())
	{
		return nullptr;
	}

	return lineWidgets[0]->getGRHandler();
}


void DynamicWrapScoreWidget::setPerformanceMarkerMode(PerformanceMarkerMode mode)
{
	ScoreWidget::setPerformanceMarkerMode(mode);

	for (ExtendedGuidoWidget* widget : lineWidgets)
	{
		widget->setPerformanceMarkerMode(mode);
	}
}


void DynamicWrapScoreWidget::setPerformanceMarkerColor(const QColor& color)
{
	ScoreWidget::setPerformanceMarkerColor(color);

	for (ExtendedGuidoWidget* widget : lineWidgets)
	{
		widget->setPerformanceMarkerColor(color);
	}
}


void DynamicWrapScoreWidget::setPerformanceMarker(int32_t markerNum, int32_t markerDenom)
{
	ScoreWidget::setPerformanceMarker(markerNum, markerDenom);

	for (ExtendedGuidoWidget* widget : lineWidgets)
	{
		widget->setPerformanceMarker(markerNum, markerDenom);
	}
}


void DynamicWrapScoreWidget::clearPerformanceMarker()
{
	ScoreWidget::clearPerformanceMarker();

	for (ExtendedGuidoWidget* widget : lineWidgets)
	{
		widget->clearPerformanceMarker();
	}
}


void DynamicWrapScoreWidget::addExcessNote(int32_t timeNum, int32_t timeDenom, int pitch, int octave)
{
	ScoreWidget::addExcessNote(timeNum, timeDenom, pitch, octave);

	for (ExtendedGuidoWidget* widget : lineWidgets)
	{
		widget->addExcessNote(timeNum, timeDenom, pitch, octave);
	}
}


void DynamicWrapScoreWidget::addExcessNote(int32_t timeNum, int32_t timeDenom, int8_t midiKey)
{
	ScoreWidget::addExcessNote(timeNum, timeDenom, midiKey);

	for (ExtendedGuidoWidget* widget : lineWidgets)
	{
		widget->addExcessNote(timeNum, timeDenom, midiKey);
	}
}


void DynamicWrapScoreWidget::clearExcessNotes()
{
	ScoreWidget::clearExcessNotes();

	for (ExtendedGuidoWidget* widget : lineWidgets)
	{
		widget->clearExcessNotes();
	}
}


int DynamicWrapScoreWidget::getActivePage() const
{
	return activePage;
}


void DynamicWrapScoreWidget::setActivePage(int pageNum)
{
	pageNum = std::min(std::max(1, pageNum), getPageCount());

	int firstPage = std::min(std::max(1, pageNum - numPreviousPages), getPageCount());
	int firstWidgetIndex = (firstPage-1) % lineWidgets.size();

	for (int i = 0 ; i < lineWidgets.size() ; i++)
	{
		int linePage = firstPage + i;
		int widgetIndex = (firstWidgetIndex + i) % lineWidgets.size();

		ExtendedGuidoWidget* widget = lineWidgets[widgetIndex];

		if (linePage <= getPageCount())
		{
			widget->setPage(linePage);
			widget->setBlanked(false);
		}
		else
		{
			widget->setBlanked(true);
		}

		if (linePage == pageNum)
		{
			widget->setOverlayColor(activeLineOverlayColor);
		}
		else
		{
			widget->setOverlayColor(inactiveLineOverlayColor);
		}
	}

	activePage = pageNum;
}


void DynamicWrapScoreWidget::setPreviousPageCount(int numPreviousPages)
{
	this->numPreviousPages = numPreviousPages;

	setActivePage(getActivePage());
}


void DynamicWrapScoreWidget::setActiveLineOverlayColor(const QColor& color)
{
	activeLineOverlayColor = color;
}


void DynamicWrapScoreWidget::setInactiveLineOverlayColor(const QColor& color)
{
	inactiveLineOverlayColor = color;
}


void DynamicWrapScoreWidget::updateARHandler()
{
	ScoreWidgetBase::updateARHandler();

	for (ExtendedGuidoWidget* widget : lineWidgets)
	{
		widget->updateARHandler();
	}
}
