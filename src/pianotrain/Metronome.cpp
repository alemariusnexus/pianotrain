#include "Metronome.h"
#include "System.h"
#include <QtCore/QFile>
#include <QtCore/QTimer>
#include <QtCore/QCoreApplication>
#include <QAudioDeviceInfo>
#include <QAudioOutput>
#include <QAudioDecoder>
#include <QAudioBuffer>
#include <nxcommon/util.h>



Metronome::Metronome(QObject* parent)
		: QObject(parent), regularVolume(1.0f), countoffVolume(1.0f), metronomeThread(nullptr)
{
	unsigned int measureStartSoundBufSize;
	unsigned int measureTickSoundBufSize;
	unsigned int measureSubTickSoundBufSize;

	calculateSoundBufferSizes(measureStartSoundBufSize, measureTickSoundBufSize, measureSubTickSoundBufSize);


	// TODO: I'm sure this can be done more intelligently

	System* sys = System::getInstance();

	for (unsigned int i = 0 ; i < measureStartSoundBufSize ; i++)
	{
		QSoundEffect* effect = new QSoundEffect;
		effect->setSource(QUrl::fromLocalFile(sys->getDataPath() + "/sounds/metronome_measure_start.wav"));
		measureStartSounds << effect;
	}

	for (unsigned int i = 0 ; i < measureTickSoundBufSize ; i++)
	{
		QSoundEffect* effect = new QSoundEffect;
		effect->setSource(QUrl::fromLocalFile(sys->getDataPath() + "/sounds/metronome_measure_tick.wav"));
		measureTickSounds << effect;
	}

	for (unsigned int i = 0 ; i < measureSubTickSoundBufSize ; i++)
	{
		QSoundEffect* effect = new QSoundEffect;
		effect->setSource(QUrl::fromLocalFile(sys->getDataPath() + "/sounds/metronome_measure_subtick.wav"));
		measureSubTickSounds << effect;
	}
}


void Metronome::startAt(uint64_t startTimestamp, int32_t countoff)
{
	uint64_t now = GetMultimediaTimerMilliseconds();

	// Stop any metronome thread that might still be running
	stop();

	metronomeThread = new MetronomeThread(this);
	metronomeThread->metronome = this;
	metronomeThread->metronomeStartTime = startTimestamp;
	metronomeThread->metronomeStopTime = 0;
	metronomeThread->countoff = countoff;

	for (QSoundEffect* eff : measureStartSounds)
	{
		//eff->moveToThread(metronomeThread);
		metronomeThread->measureStartSounds << eff;
	}

	for (QSoundEffect* eff : measureTickSounds)
	{
		//eff->moveToThread(metronomeThread);
		metronomeThread->measureTickSounds << eff;
	}

	for (QSoundEffect* eff : measureSubTickSounds)
	{
		//eff->moveToThread(metronomeThread);
		metronomeThread->measureSubTickSounds << eff;
	}

	metronomeThread->curMeasureStartSoundsIndex = 0;
	metronomeThread->curMeasureTickSoundsIndex = 0;
	metronomeThread->curMeasureSubTickSoundsIndex = 0;

	connect(metronomeThread, SIGNAL(finished()), this, SLOT(metronomeThreadFinished()));

    metronomeThread->start();
    metronomeThread->setPriority(QThread::TimeCriticalPriority);
}


void Metronome::startAtWithLength(uint64_t startTimestamp, uint32_t numTicks, int32_t countoff)
{
	uint64_t stopTimestamp = startTimestamp + (countoff + numTicks) * (60000 / ticksPerMinute);

	startAt(startTimestamp, countoff);
	stopAt(stopTimestamp);
}


void Metronome::stopAt(uint64_t stopTimestamp)
{
	if (!metronomeThread)
	{
		return;
	}

	metronomeThread->metronomeStopTime = stopTimestamp;
}


void Metronome::stop()
{
	stopAt(GetMultimediaTimerMilliseconds());
}


void Metronome::setTicksPerMinute(int32_t tpm)
{
	ticksPerMinute = tpm;
}


void Metronome::setTicksPerMeasure(int32_t tpm)
{
	ticksPerMeasure = tpm;
}


void Metronome::setNumSubdivisions(int32_t num)
{
	numSubdivisions = num;
}


void Metronome::setRegularVolume(float volume)
{
	regularVolume = volume;
}


void Metronome::setCountoffVolume(float volume)
{
	countoffVolume = volume;
}


uint64_t Metronome::getPerformanceBeginTime() const
{
	if (!metronomeThread)
	{
		return 0;
	}

	return metronomeThread->metronomeStartTime + metronomeThread->countoff * (60000 / ticksPerMinute);
}


void Metronome::metronomeThreadFinished()
{
	// NOTE: This routine is executed in the main thread, NOT in the metronome thread

	QThread* thr = (QThread*) sender();

	if (metronomeThread == thr)
	{
		metronomeThread = nullptr;
	}

	thr->deleteLater();
}


void MetronomeThread::eventLoopTick()
{
	uint64_t now = GetMultimediaTimerMilliseconds();

	if (metronomeStopTime != 0  &&  now >= metronomeStopTime)
	{
		this->exit(0);
		return;
	}

	if (now >= nextMetronomeTick)
	{
		metronomeSubTick();
		QCoreApplication::processEvents(QEventLoop::AllEvents);
		calculateNextTick();
	}

	QTimer::singleShot(0, this, SLOT(eventLoopTick()));
}


void MetronomeThread::metronomeSubTick()
{
	int32_t numSubdivisions = metronome->getNumSubdivisions();
	int32_t subticksPerMeasure = metronome->getTicksPerMeasure() * numSubdivisions;

	QSoundEffect* effectToPlay = nullptr;

	if (metronomeTickCounter % subticksPerMeasure == 0)
	{
		effectToPlay = metronome->measureStartSounds[curMeasureStartSoundsIndex];
		curMeasureStartSoundsIndex = (curMeasureStartSoundsIndex+1) % metronome->measureStartSounds.size();
	}
	else if (metronomeTickCounter % numSubdivisions == 0)
	{
		effectToPlay = metronome->measureTickSounds[curMeasureTickSoundsIndex];
		curMeasureTickSoundsIndex = (curMeasureTickSoundsIndex+1) % metronome->measureTickSounds.size();
	}
	else
	{
		effectToPlay = metronome->measureSubTickSounds[curMeasureSubTickSoundsIndex];
		curMeasureSubTickSoundsIndex = (curMeasureSubTickSoundsIndex+1) % metronome->measureSubTickSounds.size();
	}

	if (effectToPlay)
	{
		if (metronomeTickCounter < countoff*numSubdivisions)
		{
			effectToPlay->setVolume(metronome->getCountoffVolume());
		}
		else
		{
			effectToPlay->setVolume(metronome->getRegularVolume());
		}

		effectToPlay->play();
	}

	if (metronomeTickCounter < countoff*numSubdivisions)
	{
		// TODO: Maybe print something
	}

	metronomeTickCounter++;
}


void MetronomeThread::run()
{
	metronomeTickCounter = 0;

	calculateNextTick();

	QTimer::singleShot(0, this, SLOT(eventLoopTick()));

	exec();
}


void MetronomeThread::calculateNextTick()
{
	nextMetronomeTick = metronomeStartTime + (metronomeTickCounter * 60000) / (metronome->getTicksPerMinute() * metronome->getNumSubdivisions());
}


void Metronome::calculateSoundBufferSizes(unsigned int& measureStartSoundBufSize, unsigned int& measureTickSoundBufSize,
			unsigned int& measureSubTickSoundBufSize)
{
	// TODO: Do this more intelligently
	measureStartSoundBufSize = 2;
	measureTickSoundBufSize = 8;
	measureSubTickSoundBufSize = 16;
}
