#include "ExtendedGuidoWidget.h"
#include <Guido/GUIDOScoreMap.h>
#include <cstdio>
#include <cfloat>
#include <functional>
#include <algorithm>

using std::min;
using std::max;




template <class StateT>
class FunctionalMapCollector : public MapCollector
{
public:
	typedef std::function<void(StateT*, const FloatRect&, const TimeSegment&, const GuidoElementInfos&)> FuncType;

public:
	FunctionalMapCollector(StateT* state, FuncType func) : state(state), func(func) {}
	virtual void Graph2TimeMap(const FloatRect& box, const TimeSegment& dates, const GuidoElementInfos& infos) { func(state, box, dates, infos); }

private:
	StateT* state;
	FuncType func;
};





ExtendedGuidoWidget::ExtendedGuidoWidget(QWidget* parent)
		: QGuidoWidget(parent), performanceMarkerNum(-1), performanceMarkerDenom(1)
{
}


void ExtendedGuidoWidget::setPerformanceMarker(int32_t markerNum, int32_t markerDenom)
{
	performanceMarkerNum = markerNum;
	performanceMarkerDenom = markerDenom;

	QRect perfMarkerRect = calculatePerformanceMarkerRect(performanceMarkerNum, performanceMarkerDenom);

	if (perfMarkerRect != lastPerformanceMarkerRect)
	{
		update();
	}
}


void ExtendedGuidoWidget::clearPerformanceMarker()
{
	if (performanceMarkerNum != -1)
	{
		performanceMarkerNum = -1;

		update();
	}
}


QRect ExtendedGuidoWidget::calculatePerformanceMarkerRect(int32_t timeNum, int32_t timeDenom)
{
	CGRHandler gr = getGRHandler();


	// Find system rectangle (used to determine height of the marker rect)

	struct SystemMapState
	{
		FloatRect rect;
	} systemMapState;

	auto systemMapCallback = [timeNum, timeDenom](SystemMapState* state,
			const FloatRect& rect, const TimeSegment& ts, const GuidoElementInfos& info)
	{
		if (ts.include({timeNum, timeDenom}))
		{
			state->rect = rect;
		}
	};

	FunctionalMapCollector<SystemMapState> systemMapCollector(&systemMapState, systemMapCallback);
	GuidoGetMap(gr, 1, width(), height(), kGuidoSystem, systemMapCollector);


	// Find event rectangle

	struct EventMapState
	{
		FloatRect rect;
	} eventMapState;
	eventMapState.rect = FloatRect(FLT_MAX, FLT_MAX, 0.0f, 0.0f);

	auto eventMapCallback = [timeNum, timeDenom](EventMapState* state,
			const FloatRect& rect, const TimeSegment& ts, const GuidoElementInfos& info)
	{
		if (ts.include({timeNum, timeDenom}))
		{
			state->rect.left = min(state->rect.left, rect.left);
			state->rect.top = min(state->rect.top, rect.top);
			state->rect.right = max(state->rect.right, rect.right);
			state->rect.bottom = max(state->rect.bottom, rect.bottom);
		}
	};

	FunctionalMapCollector<EventMapState> eventMapCollector(&eventMapState, eventMapCallback);
	GuidoGetMap(gr, 1, width(), height(), kGuidoEvent, eventMapCollector);



	float rectLeft = eventMapState.rect.left;
	float rectRight = eventMapState.rect.right;
	float rectTop = systemMapState.rect.top;
	float rectBottom = systemMapState.rect.bottom;

	return QRect(rectLeft, rectTop, rectRight-rectLeft, rectBottom-rectTop);
}


void ExtendedGuidoWidget::paintEvent(QPaintEvent* event)
{
	QGuidoWidget::paintEvent(event);

	if (performanceMarkerNum >= 0)
	{
		QPainter painter(this);

		QRect perfMarkerRect = calculatePerformanceMarkerRect(performanceMarkerNum, performanceMarkerDenom);
		lastPerformanceMarkerRect = perfMarkerRect;

		painter.fillRect(perfMarkerRect, QColor(0, 0, 255, 50));
	}
}
