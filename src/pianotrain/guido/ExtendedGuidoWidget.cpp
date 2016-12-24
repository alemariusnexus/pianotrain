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
#include <nxcommon/util.h>
#include <cstdio>
#include <cfloat>
#include <functional>
#include <algorithm>

using std::min;
using std::max;





ExtendedGuidoWidget::ExtendedGuidoWidget(QWidget* parent)
		: QGuidoWidget(parent), performanceMarkerMode(MarkAllSimultaneous), performanceMarkerColor(QColor(0, 0, 255, 50)),
		  performanceMarkerNum(-1), performanceMarkerDenom(1)
{
}


void ExtendedGuidoWidget::setPerformanceMarkerMode(PerformanceMarkerMode mode)
{
	if (performanceMarkerMode != mode)
	{
		performanceMarkerMode = mode;
		update();
	}
}


void ExtendedGuidoWidget::setPerformanceMarkerColor(const QColor& color)
{
	performanceMarkerColor = color;
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


void ExtendedGuidoWidget::addExcessNote(int32_t timeNum, int32_t timeDenom, int pitch, int octave)
{
	ExcessNote note;
	note.timeNum = timeNum;
	note.timeDenom = timeDenom;
	note.pitch = pitch;
	note.octave = octave;
	excessNotes << note;

	update();
}


void ExtendedGuidoWidget::addExcessNote(int32_t timeNum, int32_t timeDenom, int8_t midiKey)
{
	int pitch, octave;
	GuidoMidiKeyToPitch(midiKey, pitch, octave);
	addExcessNote(timeNum, timeDenom, pitch, octave);
}


void ExtendedGuidoWidget::clearExcessNotes()
{
	excessNotes.clear();
	update();
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

				if (performanceMarkerMode == MarkAllSimultaneous)
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
	GuidoGetMap(gr, 1, width(), height(), kGuidoEvent, eventMapCollector);



	float rectLeft = eventMapState.rect.left;
	float rectRight = eventMapState.rect.right;
	float rectTop = systemMapState.rect.top;
	float rectBottom = systemMapState.rect.bottom;

	return QRect(rectLeft, rectTop, rectRight-rectLeft, rectBottom-rectTop);
}


void ExtendedGuidoWidget::paintPerformanceMarker(const QRect& markerRect)
{
	QPainter painter(this);
	painter.fillRect(markerRect, performanceMarkerColor);
}


void ExtendedGuidoWidget::paintEvent(QPaintEvent* event)
{
	QGuidoWidget::paintEvent(event);

	CGRHandler gr = getGRHandler();

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

		QPointF pos = GuidoApproximateNoteGraphicalPosition(gr, 1, width(), height(), note.timeNum, note.timeDenom, note.pitch, note.octave,
				&ledgerLinePositions);


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

	guidoDev->EndDraw();

	delete guidoDev;
	delete guidoSys;


	if (performanceMarkerNum >= 0)
	{
		QRect perfMarkerRect = calculatePerformanceMarkerRect(performanceMarkerNum, performanceMarkerDenom);
		lastPerformanceMarkerRect = perfMarkerRect;

		paintPerformanceMarker(perfMarkerRect);
	}
}
