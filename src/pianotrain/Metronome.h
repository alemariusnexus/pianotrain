#ifndef METRONOME_H_
#define METRONOME_H_

#include <QtCore/QObject>
#include <QtCore/QThread>
#include <QtCore/QBuffer>
#include <QSoundEffect>
#include <QAudioDecoder>
#include <QAudioOutput>


#define NUM_AUDIO_OUTPUT_STREAMS 8


class Metronome;


class MetronomeThread : public QThread
{
Q_OBJECT

public:
	MetronomeThread(QObject* parent = nullptr) : QThread(parent) {}

protected:
	virtual void run();

private slots:
	void eventLoopTick();
	void metronomeSubTick();

private:
	void calculateNextTick();

public:
	Metronome* metronome;
	uint64_t metronomeStartTime;
	uint64_t metronomeStopTime;
	uint32_t metronomeTickCounter;
	uint64_t nextMetronomeTick;
	int32_t countoff;

	// Multiple copies of the same sound effect, to be able to play the sound again while it hasn't finished (happens at
	// high tempos). A single QSoundEffect doesn't play nicely this way.
	QList<QSoundEffect*> measureStartSounds;
	QList<QSoundEffect*> measureTickSounds;
	QList<QSoundEffect*> measureSubTickSounds;

	uint32_t curMeasureStartSoundsIndex;
	uint32_t curMeasureTickSoundsIndex;
	uint32_t curMeasureSubTickSoundsIndex;
};


class Metronome : public QObject
{
	Q_OBJECT

	friend class MetronomeThread;

public:
	Metronome(QObject* parent = nullptr);

	Q_INVOKABLE void startAt(uint64_t startTimestamp, int32_t countoff = 0);
	Q_INVOKABLE void startAtWithLength(uint64_t startTimestamp, uint32_t numTicks, int32_t countoff = 0);

	Q_INVOKABLE void stopAt(uint64_t stopTimestamp);
	Q_INVOKABLE void stop();

	Q_INVOKABLE uint64_t getPerformanceBeginTime() const;

	Q_INVOKABLE void setTicksPerMinute(int32_t tpm);
	Q_INVOKABLE void setTicksPerMeasure(int32_t tpm);
	Q_INVOKABLE void setNumSubdivisions(int32_t num);

	Q_INVOKABLE int32_t getTicksPerMinute() const { return ticksPerMinute; }
	Q_INVOKABLE int32_t getTicksPerMeasure() const { return ticksPerMeasure; }
	Q_INVOKABLE int32_t getNumSubdivisions() const { return numSubdivisions; }

	Q_INVOKABLE void setRegularVolume(float volume);
	Q_INVOKABLE void setCountoffVolume(float volume);

	Q_INVOKABLE float getRegularVolume() const { return regularVolume; }
	Q_INVOKABLE float getCountoffVolume() const { return countoffVolume; }

	Q_INVOKABLE float getVolumeMultiplier() const { return volumeMultiplier; }

private:
	void calculateSoundBufferSizes(unsigned int& measureStartSoundBufSize, unsigned int& measureTickSoundBufSize,
			unsigned int& measureSubTickSoundBufSize);

private slots:
	void metronomeThreadFinished();

private:
	int32_t ticksPerMinute;
	int32_t ticksPerMeasure;
	int32_t numSubdivisions;
	float regularVolume;
	float countoffVolume;
	float volumeMultiplier;

	MetronomeThread* metronomeThread;

	QList<QSoundEffect*> measureStartSounds;
	QList<QSoundEffect*> measureTickSounds;
	QList<QSoundEffect*> measureSubTickSounds;
};

#endif /* METRONOME_H_ */
