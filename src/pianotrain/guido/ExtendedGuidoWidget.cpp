#include "ExtendedGuidoWidget.h"
#include "guidoextensions.h"
#include <Guido/GUIDOScoreMap.h>
#include <Guido/VGSystem.h>
#include <Guido/VGDevice.h>
#include <Guido/VGFont.h>
#include <Guido/lib/GUIDOInternal.h>
#include <Guido/lib/MusicalSymbols.h>
#include <Guido/lib/FontManager.h>
#include <Guido/graphic/GRPage.h>
#include <Guido/graphic/GRMusic.h>
#include <Guido/parser/GuidoDefs.h>
#include <GuidoQt/GSystemQt.h>
#include <QPixmapCache>
#include <nxcommon/util.h>
#include <cstdio>
#include <cfloat>
#include <functional>
#include <algorithm>

using std::min;
using std::max;





ExtendedGuidoWidget::ExtendedGuidoWidget(QWidget* parent)
		: QGuidoWidget(parent), blanked(false), overlayColor(255, 255, 255, 0), singleLineMode(false)
{
}


void ExtendedGuidoWidget::setARHandler(ARHandler ar)
{
	ScoreWidgetBase::setARHandler(ar);
	QGuidoWidget::setARHandler(ar);
}


CGRHandler ExtendedGuidoWidget::getGRHandler() const
{
	return QGuidoWidget::getGRHandler();
}


void ExtendedGuidoWidget::setPerformanceMarkerMode(ScoreWidgetEnums::PerformanceMarkerMode mode)
{
	ScoreWidgetEnums::PerformanceMarkerMode oldMode = perfMarkerMode;

	ScoreWidgetBase::setPerformanceMarkerMode(mode);

	if (mode != oldMode)
	{
		update();
	}
}


void ExtendedGuidoWidget::setPerformanceMarker(int32_t markerNum, int32_t markerDenom)
{
	ScoreWidgetBase::setPerformanceMarker(markerNum, markerDenom);

	QRect perfMarkerRect = calculatePerformanceMarkerRect(perfMarkerNum, perfMarkerDenom);

	if (perfMarkerRect != lastPerformanceMarkerRect)
	{
		update();
	}
}


void ExtendedGuidoWidget::addExcessNote(int32_t timeNum, int32_t timeDenom, int pitch, int octave)
{
	ScoreWidgetBase::addExcessNote(timeNum, timeDenom, pitch, octave);

	update();
}


void ExtendedGuidoWidget::addExcessNote(int32_t timeNum, int32_t timeDenom, int8_t midiKey)
{
	ScoreWidgetBase::addExcessNote(timeNum, timeDenom, midiKey);
	update();
}


void ExtendedGuidoWidget::clearExcessNotes()
{
	ScoreWidgetBase::clearExcessNotes();
	update();
}


void ExtendedGuidoWidget::clearPerformanceMarker()
{
	int32_t oldNum = perfMarkerNum;

	ScoreWidgetBase::clearPerformanceMarker();

	lastPerformanceMarkerRect = QRect();

	if (oldNum != -1)
	{
		update();
	}
}


void ExtendedGuidoWidget::setBlanked(bool blanked)
{
	if (this->blanked != blanked)
	{
		this->blanked = blanked;
		update();
	}
}


void ExtendedGuidoWidget::setOverlayColor(const QColor& overlayColor)
{
	if (this->overlayColor != overlayColor)
	{
		this->overlayColor = overlayColor;
		update();
	}
}


void ExtendedGuidoWidget::setSingleLineMode(bool singleLineMode)
{
	if (singleLineMode != this->singleLineMode)
	{
		this->singleLineMode = singleLineMode;
		update();
	}
}


void ExtendedGuidoWidget::updateARHandler()
{
	ScoreWidgetBase::updateARHandler();

	ar->refCount += 2;
	QGuidoWidget::setARHandler(ar);
	ar->refCount -= 1;

	update();
}


QRect ExtendedGuidoWidget::calculatePerformanceMarkerRect(int32_t timeNum, int32_t timeDenom)
{
	// TODO: Support for more than one page

	if (perfMarkerMode == ScoreWidgetEnums::MarkNone)
	{
		return QRect();
	}

	CGRHandler gr = QGuidoWidget::getGRHandler();


	// Find system rectangle (used to determine height of the marker rect)

	struct SystemMapState
	{
		FloatRect rect;
	} systemMapState;
	systemMapState.rect = FloatRect(-1.0f, -1.0f, -1.0f, -1.0f);

	auto systemMapCallback = [timeNum, timeDenom](SystemMapState* state,
			const FloatRect& rect, const TimeSegment& ts, const GuidoElementInfos& info)
	{
		if (ts.include({timeNum, timeDenom}))
		{
			state->rect = rect;
		}
	};

	FunctionalMapCollector<SystemMapState> systemMapCollector(&systemMapState, systemMapCallback);
	GuidoGetMap(gr, firstVisiblePage(), width(), height(), kGuidoSystem, systemMapCollector);


	if (perfMarkerMode == ScoreWidgetEnums::MarkMeasure)
	{
		// Find system slice (seems to be the same as measure?)

		struct SystemSliceMapState
		{
			FloatRect rect;
			bool found;
		} systemSliceMapState;
		systemSliceMapState.found = false;

		auto systemSliceMapCallback = [timeNum, timeDenom](SystemSliceMapState* state,
				const FloatRect& rect, const TimeSegment& ts, const GuidoElementInfos& info)
		{
			float fstart = ts.first.num / (float) ts.first.denom;
			float fend = ts.second.num / (float) ts.second.denom;

			if (fstart != fend  &&  ts.include({timeNum, timeDenom}))
			{
				state->rect = rect;
				state->found = true;
			}
		};

		FunctionalMapCollector<SystemSliceMapState> systemSliceMapCollector(&systemSliceMapState, systemSliceMapCallback);
		GuidoGetMap(gr, firstVisiblePage(), width(), height(), kGuidoSystemSlice, systemSliceMapCollector);

		if (!systemSliceMapState.found)
		{
			// Timepos not found on current page
			return QRect();
		}

		float rectLeft = systemSliceMapState.rect.left;
		float rectRight = systemSliceMapState.rect.right;
		float rectTop = systemMapState.rect.top;
		float rectBottom = systemMapState.rect.bottom;

		return QRect(rectLeft, rectTop, rectRight-rectLeft, rectBottom-rectTop);
	}
	else
	{
		// Find event rectangle

		struct EventMapState
		{
			GuidoDate bestStart;
			FloatRect rect;
		} eventMapState;

		eventMapState.bestStart = {-1, 1};
		eventMapState.rect = FloatRect(FLT_MAX, FLT_MAX, 0.0f, 0.0f);

		auto eventMapCallback = [timeNum, timeDenom, this](EventMapState* state,
				const FloatRect& rect, const TimeSegment& ts, const GuidoElementInfos& info)
		{
			if (ts.include({timeNum, timeDenom}))
			{
				float bestStart = state->bestStart.num / (float) state->bestStart.denom;
				float start = ts.first.num / (float) ts.first.denom;

				if (state->bestStart.num < 0  ||  start > bestStart)
				{
					// Found an event that is better (occurred before marker time but later than current best)
					// -> form new event rect
					state->rect = rect;
					state->bestStart = ts.first;
				}
				else if (start == bestStart)
				{
					// Another event that starts at the same time

					if (perfMarkerMode == ScoreWidgetEnums::MarkAllSimultaneous)
					{
						// Expand the event rect
						state->rect.left = min(state->rect.left, rect.left);
						state->rect.top = min(state->rect.top, rect.top);
						state->rect.right = max(state->rect.right, rect.right);
						state->rect.bottom = max(state->rect.bottom, rect.bottom);
					}
					else
					{
						if (rect.left < state->rect.left)
						{
							// Found event with rect that is more to the left -> use it
							state->rect = rect;
						}
					}
				}
			}
		};

		FunctionalMapCollector<EventMapState> eventMapCollector(&eventMapState, eventMapCallback);
		GuidoGetMap(gr, firstVisiblePage(), width(), height(), kGuidoEvent, eventMapCollector);

		if (systemMapState.rect.left < 0.0f  ||  eventMapState.bestStart.num < 0)
		{
			// Timepos not found on currently displayed page
			return QRect();
		}



		float rectLeft = eventMapState.rect.left;
		float rectRight = eventMapState.rect.right;
		float rectTop = systemMapState.rect.top;
		float rectBottom = systemMapState.rect.bottom;

		return QRect(rectLeft, rectTop, rectRight-rectLeft, rectBottom-rectTop);
	}
}


void ExtendedGuidoWidget::paintPerformanceMarker(const QRect& markerRect)
{
	if (!markerRect.isNull())
	{
		QPainter painter(this);
		painter.fillRect(markerRect, perfMarkerColor);
	}
}


void ExtendedGuidoWidget::paintOverlay()
{
	QPainter painter(this);
	painter.setBrush(QBrush(overlayColor));
	painter.setPen(QPen(overlayColor));
	painter.drawRect(QRect(0, 0, width(), height()));
}


void ExtendedGuidoWidget::paintEvent(QPaintEvent* event)
{
	if (blanked)
	{
		QPainter painter(this);
		painter.setBrush(QBrush(QColor(255, 255, 255)));
		painter.setPen(QPen(QColor(255, 255, 255)));
		painter.drawRect(QRect(0, 0, width(), height()));

		paintOverlay();

		return;
	}


	QGuidoWidget::paintEvent(event);

	CGRHandler gr = QGuidoWidget::getGRHandler();

	if (gr)
	{
		QPainter painter(this);


		// ********** Render excess notes **********

		// The following is the result of hours of digging through the sources of Guidolib. It is not pretty, but
		// it seems to work very well...

		// Setup the QDeviceQt

		// We create them on the fly. It's not a costly operation, and QGuidoPainter does it the same way.
		GSystemQt* guidoSys = new GSystemQt(&painter);
		VGDevice* guidoDev = guidoSys->CreateDisplayDevice();

		VGColor color(255, 0, 0);
		guidoDev->SelectPenColor(color);
		guidoDev->SelectFillColor(color);
		guidoDev->SetFontColor(color);

		guidoDev->BeginDraw();

		GRPage* page = gr->grmusic->getPage(1);

		float scaleX = width();
		float scaleY = height();
		page->getScaling(scaleX, scaleY);

		guidoDev->SetScale(scaleX, scaleY);

		const VGFont* guidoFont = FontManager::gFontScriab;
		guidoDev->SetMusicFont(guidoFont);

		float noteHeadExtX, noteHeadExtY;

		// TODO: Optimize this. Might be slow and the result can be cached
		FontManager::gFontScriab->GetExtent(kFullHeadSymbol, &noteHeadExtX, &noteHeadExtY, guidoDev);


		for (const ExcessNote& note : excessNotes)
		{
			QList<QPointF> ledgerLinePositions;

			int pagenum;
			QPointF pos = GuidoApproximateNoteGraphicalPosition(gr, width(), height(), note.timeNum, note.timeDenom, note.pitch, note.octave,
					pagenum, &ledgerLinePositions);

			if (pagenum >= firstVisiblePage()  &&  pagenum <= lastVisiblePage())
			{
				// Draw the note head
				guidoDev->DrawMusicSymbol(pos.x() / scaleX - 0.5f*noteHeadExtX, pos.y() / scaleY, kFullHeadSymbol);


				// Draw the ledger lines (if any)

				float ledgerOffsetX = -60*0.85f; // From GRSingleNote::OnDraw(), where it's called "ledXPos"
				for (QPointF llpos : ledgerLinePositions)
				{
					guidoDev->DrawMusicSymbol(llpos.x() / scaleX + ledgerOffsetX, llpos.y() / scaleY, kLedgerLineSymbol);
				}



				// Draw an accidental if necessary
				// TODO: Respect key signature!
				// 		NOTE:  Not only key signature, also accidentals of previous notes in the same measure...
				//			   Code to find the key signature is commented out in GuidoApproximateNoteGraphicalPosition()

				int accidentalSymbol = kNoneSymbol;

				if (note.pitch >= NOTE_CIS  &&  note.pitch <= NOTE_AIS)
				{
					accidentalSymbol = kSharpSymbol;
				}

				if (accidentalSymbol != kNoneSymbol)
				{
					float accExtX, accExtY;

					// TODO: Optimize this. Might be slow and the result can be cached
					FontManager::gFontScriab->GetExtent(accidentalSymbol, &accExtX, &accExtY, guidoDev);

					// This is (basically) the actual formula used by GRAccidental::setAccidentalLayout(). Don't ask why.
					float accidentalOffsetX = - (0.5*noteHeadExtX + 1.2f*0.5f*accExtX + 0.2f * 50.0f);

					guidoDev->DrawMusicSymbol(pos.x() / scaleX + accidentalOffsetX - 0.5f*noteHeadExtX, pos.y() / scaleY, accidentalSymbol);
				}
			}
		}

		guidoDev->EndDraw();

		delete guidoDev;
		delete guidoSys;



		//if (performanceMarkerNum >= 0)
		if (perfMarkerNum >= 0)
		{
			//QRect perfMarkerRect = calculatePerformanceMarkerRect(performanceMarkerNum, performanceMarkerDenom);
			QRect perfMarkerRect = calculatePerformanceMarkerRect(perfMarkerNum, perfMarkerDenom);
			lastPerformanceMarkerRect = perfMarkerRect;
			paintPerformanceMarker(perfMarkerRect);
		}
	}


	paintOverlay();
}


void ExtendedGuidoWidget::resizeEvent(QResizeEvent* evt)
{
	// NOTE: Do NOT call QGuidoWidget::resizeEvent(). It will call resize() again, which leads to infinite
	// recursion when the widget is inside a QScrollArea
	QWidget::resizeEvent(evt);

	QPixmapCache::remove(key());
}


QSize ExtendedGuidoWidget::sizeHint() const
{
	if (singleLineMode)
	{
		// NOTE: pageCount() returns 1 when no GR/AR is set, which would make mPageManager->pageSize(i) crash...
		if (pageCount() < 1  ||  !getGRHandler())
		{
			return QGuidoWidget::sizeHint();
		}

		float largestRelativeHeight = -1.0f;
		QSizeF largestSize;

		for (int i = 1 ; i <= pageCount() ; i++)
		{
			QSizeF pgsize = mPageManager->pageSize(i);
			float relativeHeight = pgsize.height() / pgsize.width();

			if (largestRelativeHeight < 0.0f  ||  relativeHeight > largestRelativeHeight)
			{
				largestRelativeHeight = relativeHeight;
				largestSize = pgsize;
			}
		}

		return largestSize.toSize();
	}
	else
	{
		return QGuidoWidget::sizeHint();
	}
}
