#ifndef MIDIPERFORMANCE_H_
#define MIDIPERFORMANCE_H_

#include <QtCore/QObject>
#include <QtCore/QList>
#include <nxcommon/nxcommon_stdint.h>
#include <nxcommon/CString.h>

#include <vector>
#include <queue>
#include <thread>
#include <functional>


#define MIDI_PERFORMANCE_LEN_WHOLE 128
#define MIDI_PERFORMANCE_LEN_HALF 64
#define MIDI_PERFORMANCE_LEN_QUARTER 32
#define MIDI_PERFORMANCE_LEN_EIGHTH 16
#define MIDI_PERFORMANCE_LEN_SIXTEENTH 8




class MidiPerformance : public QObject
{
	Q_OBJECT

public:
	struct Note
	{
		int8_t midiKey;
		int32_t startTick;
		int32_t lenNum;
		int32_t lenDenom;
		bool hit;
		bool missed;
	};

public:
	MidiPerformance(QObject* parent = nullptr);

	void startChord();
	void endChord();

	void addNote(int8_t midiKey, int32_t lenNum, int32_t lenDenom);
	void addNote(const CString& name, int32_t lenNum, int32_t lenDenom);
	void addNote(int8_t midiKey, int32_t lenDenom);
	void addNote(const CString& name, int32_t lenDenom);

	void addRest(int32_t lenNum, int32_t lenDenom);
	void addRest(int32_t lenDenom);

	void setTempo(int32_t bpm, int32_t baseNum = 1, int32_t baseDenom = 4);

	void setTimingGracePeriodFunction(const std::function<int32_t(int32_t, int32_t)>& func);
	void setTimingGracePeriodRelativeFunction(const std::function<void(int32_t&, int32_t&, int32_t, int32_t)>& func);

	void start(int32_t countOffNum, int32_t countOffDenom);
	void start();

	void hitNote(int8_t midiKey, uint64_t timestamp);
	void hitNote(int8_t midiKey);

	void releaseNote(uint8_t midiKey, uint64_t timestamp);
	void releaseNote(uint8_t midiKey);

	void setNoteNameOctaveOffset(int8_t octaveOffset = -1);

signals:
	void currentNoteUpdated();
	void currentTickUpdated(int32_t num, int32_t denom);

private slots:
	void midiMessageReceived(uint8_t status, uint8_t data1, uint8_t data2, uint64_t timestamp);

private:
	void reportHitNote(const Note& note);
	void reportMissedNote(const Note& note);
	void reportExcessNote(int8_t midiKey, int32_t tick);
	void reportWrongNote(const Note& note, int8_t playedMidiKey);

	void addAbsoluteNote(int8_t midiKey, int32_t start, int32_t lenNum, int32_t lenDenom);

	void performanceThreadMain();

	void checkMissedNotes();

	int32_t getPerformanceTick(uint64_t timestamp) const;
	int32_t getPerformanceTick() const;

	int32_t noteLengthToTickCount(int32_t num, int32_t denom) const;

private:
	QList<Note> notes;
	int32_t nextStart;
	int32_t ticksPerMinute;
	std::function<int32_t(int32_t, int32_t)> gracePeriodFunc;
	std::thread perfThread;
	uint64_t perfStartTimestamp;
	int8_t noteNameOctaveOffset;

	bool chordActive;
	int32_t chordMinLength;
};

#endif /* MIDIPERFORMANCE_H_ */
