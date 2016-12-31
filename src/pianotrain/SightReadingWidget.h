#ifndef SIGHTREADINGWIDGET_H_
#define SIGHTREADINGWIDGET_H_

#include <pianotrain/config.h>
#include <QtCore/QTimer>
#include <QtCore/QThread>
#include <QSoundEffect>
#include <QWidget>
#include <Guido/GUIDOParse.h>
#include <Guido/GUIDOEngine.h>
#include <ui_SightReadingWidget.h>
#include "Metronome.h"
#include "guido/GuidoNoteMarker.h"

class MidiPerformance;




class SightReadingWidget : public QWidget
{
	Q_OBJECT

public:
	SightReadingWidget(QWidget* parent = nullptr);

	Q_INVOKABLE void setGMNCode(const QString& code);

	Q_INVOKABLE void setTempo(int32_t bpm);

	Q_INVOKABLE void setRhythmMode(bool rhythmMode);

	Q_INVOKABLE Metronome* getMetronome() { return metronome; }

	Q_INVOKABLE void startPerformance();

	Q_INVOKABLE void stopPerformance();

	Q_INVOKABLE DynamicWrapScoreWidget* getScoreWidget();

signals:
	void generateRequested();
	void performanceStarted();
	void performanceFinished(bool stopped);

private slots:
	void onGenerate();
	void onStartStop();

	void performanceFinishedSlot(bool stopped);

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

	void noteExcess (
			int8_t midiKey,
			int32_t hitNum, int32_t hitDenom );

private:
	Ui_SightReadingWidget ui;

	ARHandler ar;
	MidiPerformance* perf;
	int32_t tempo;
	bool rhythmMode;

	Metronome* metronome;
	uint32_t musicDurationNum;
	uint32_t musicDurationDenom;
};


#endif /* SIGHTREADINGWIDGET_H_ */
