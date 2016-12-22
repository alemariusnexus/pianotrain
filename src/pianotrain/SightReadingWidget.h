#ifndef SIGHTREADINGWIDGET_H_
#define SIGHTREADINGWIDGET_H_

#include <QtCore/QTimer>
#include <QtGui/QWidget>
#include <Guido/GUIDOParse.h>
#include <Guido/GUIDOEngine.h>
#include <ui_SightReadingWidget.h>
#include "guido/GuidoNoteMarker.h"

class MidiPerformance;


class SightReadingWidget : public QWidget
{
	Q_OBJECT

public:
	SightReadingWidget(QWidget* parent = nullptr);

private:
	void updateGuidoDisplay();

	void delayMilliseconds(uint64_t ms);

private slots:
	void startPerformance();

	void onGenerate();
	void currentTickUpdated(int32_t num, int32_t denom);

	void metronomeTick();

	void noteHit (
			int8_t midiKey,
			int32_t hitNum, int32_t hitDenom,
			int32_t startNum, int32_t startDenom,
			int32_t lenNum, int32_t lenDenom,
			void* userData );

	void noteMissed (
			int8_t midiKey,
			int32_t startNum, int32_t startDenom,
			int32_t lenNum, int32_t lenDenom,
			void* userData );

private:
	Ui_SightReadingWidget ui;
	QTimer* metronomeTimer;
	ARHandler ar;
	MidiPerformance* perf;
	GuidoNoteMarker* noteMarker;
	uint64_t perfStartTime;

	uint32_t metronomeTickCounter;
};

#endif /* SIGHTREADINGWIDGET_H_ */
