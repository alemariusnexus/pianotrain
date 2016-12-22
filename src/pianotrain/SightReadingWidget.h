#ifndef SIGHTREADINGWIDGET_H_
#define SIGHTREADINGWIDGET_H_

#include <QtCore/QTimer>
#include <QtCore/QThread>
#include <QSoundEffect>
#include <QWidget>
#include <Guido/GUIDOParse.h>
#include <Guido/GUIDOEngine.h>
#include <ui_SightReadingWidget.h>
#include "guido/GuidoNoteMarker.h"

class MidiPerformance;



class MetronomeThread : public QThread
{
	Q_OBJECT

public:
	MetronomeThread(QObject* parent = nullptr) : QThread(parent) {}

protected:
	virtual void run();

private slots:
	void eventLoopTick();
	void metronomeTick();

private:
	uint32_t metronomeTickCounter;
	QSoundEffect* metronomeFullTickSound;
	QSoundEffect* metronomeHalfTickSound;
	uint64_t nextMetronomeTick;
};


class SightReadingWidget : public QWidget
{
	Q_OBJECT

public:
	SightReadingWidget(QWidget* parent = nullptr);

private:
	void updateGuidoDisplay();

	void delayMilliseconds(uint64_t ms);

private slots:
	void startPerformanceCountoff();
	void startPerformance();

	void onGenerate();
	void currentTickUpdated(int32_t num, int32_t denom);

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
	QSoundEffect* metronomeFullTickSound;
	QSoundEffect* metronomeHalfTickSound;
};

#endif /* SIGHTREADINGWIDGET_H_ */
