#ifndef MIDIPERFORMANCE_H_
#define MIDIPERFORMANCE_H_

#include <QtCore/QObject>
#include <QtCore/QList>
#include <QtCore/QThread>
#include <nxcommon/nxcommon_stdint.h>
#include <nxcommon/CString.h>

#include <vector>
#include <queue>
#include <thread>
#include <functional>


//#define MIDI_PERFORMANCE_LEN_WHOLE 128
#define MIDI_PERFORMANCE_LEN_WHOLE 2048
#define MIDI_PERFORMANCE_LEN_HALF 64
#define MIDI_PERFORMANCE_LEN_QUARTER 32
#define MIDI_PERFORMANCE_LEN_EIGHTH 16
#define MIDI_PERFORMANCE_LEN_SIXTEENTH 8



class MidiPerformance;


class MidiPerformanceThread : public QThread
{
	Q_OBJECT

public:
	MidiPerformanceThread(MidiPerformance* perf, QObject* parent = nullptr);

protected:
	virtual void run();

public:
	MidiPerformance* perf;
	bool stopped;
};



class MidiPerformance : public QObject
{
	Q_OBJECT

	friend class MidiPerformanceThread;

private:
	struct Note
	{
		int8_t midiKey;
		int32_t startTick;
		int32_t lenNum;
		int32_t lenDenom;
		bool hit;
		bool missed;
		void* userData;
	};

public:
	MidiPerformance(QObject* parent = nullptr);

	void startChord();
	void endChord();

	void addAbsoluteNote(int8_t midiKey, int32_t startNum, int32_t startDenom, int32_t lenNum, int32_t lenDenom, void* userData = nullptr);
	void addAbsoluteRest(int32_t startNum, int32_t startDenom, int32_t lenNum, int32_t lenDenom, void* userData = nullptr);

	void advance(int32_t lenNum, int32_t lenDenum);

	void addNote(int8_t midiKey, int32_t lenNum, int32_t lenDenom, void* userData = nullptr);
	void addNote(const CString& name, int32_t lenNum, int32_t lenDenom, void* userData = nullptr);
	void addNote(int8_t midiKey, int32_t lenDenom, void* userData = nullptr);
	void addNote(const CString& name, int32_t lenDenom, void* userData = nullptr);

	void addRest(int32_t lenNum, int32_t lenDenom, void* userData = nullptr);
	void addRest(int32_t lenDenom, void* userData = nullptr);

	void setTempo(int32_t bpm, int32_t baseNum = 1, int32_t baseDenom = 4);

	void setTimingGracePeriodFunction(const std::function<int32_t(int32_t, int32_t)>& func);
	void setTimingGracePeriodRelativeFunction(const std::function<void(int32_t&, int32_t&, int32_t, int32_t)>& func);

	void start();

	void stop();

	bool isPerformanceRunning() const;

	void hitNote(int8_t midiKey, int32_t timeNum, int32_t timeDenom);
	void hitNote(int8_t midiKey, uint64_t timestamp);
	void hitNote(int8_t midiKey);

	void releaseNote(uint8_t midiKey, int32_t timeNum, int32_t timeDenom);
	void releaseNote(uint8_t midiKey, uint64_t timestamp);
	void releaseNote(uint8_t midiKey);

	void setNoteNameOctaveOffset(int8_t octaveOffset = -1);

signals:
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

	void performanceFinished(bool stopped);

	/*// TODO: Expose more info
	void noteWrong (
			int8_t midiKey,
			int32_t hitNum, int32_t hitDenom );*/


private slots:
	void midiMessageReceived(uint8_t status, uint8_t data1, uint8_t data2, uint64_t timestamp);
	void perfThreadFinished();

protected:
	void reportHitNote(int32_t hitNum, int32_t hitDenom, const Note& note);
	void reportMissedNote(const Note& note);
	void reportExcessNote(int32_t hitNum, int32_t hitDenom, int8_t midiKey);
	void reportWrongNote(int32_t hitNum, int32_t hitDenom, const QList<Note*>& possibleNotes, int8_t playedMidiKey);

	int32_t getPerformanceTick(uint64_t timestamp) const;
	int32_t getPerformanceTick() const;

	int32_t noteLengthToTickCount(int32_t num, int32_t denom) const;

private:
	void notifyCurrentTickUpdated(int32_t num, int32_t denom);

private:
	QList<Note> notes;
	int32_t nextStart;
	int32_t ticksPerMinute;
	std::function<int32_t(int32_t, int32_t)> gracePeriodFunc;
	//std::thread* perfThread;
	MidiPerformanceThread* perfThread;
	uint64_t perfStartTimestamp;
	int8_t noteNameOctaveOffset;

	bool chordActive;
	int32_t chordMinLength;
};

#endif /* MIDIPERFORMANCE_H_ */
