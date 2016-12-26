#ifndef GUIDOEXTENSIONS_H_
#define GUIDOEXTENSIONS_H_

#include <Guido/GUIDOParse.h>
#include <Guido/GUIDOEngine.h>
#include <Guido/GUIDOScoreMap.h>
#include <Guido/abstract/ARMusicalVoiceState.h>
#include <QtCore/QList>
#include <QtCore/qpoint.h>
#include <ostream>
#include <functional>

class MidiPerformance;
class GRStaff;


// We need access to some protected member variables of ARMusicalVoiceState, so we have to
// use a subclass to expose them.
// It's dirty, but it works...
class ARCustomMusicalVoiceState : public ARMusicalVoiceState
{
public:
	GuidoPos& getVpos() { return vpos; }
	TYPE_TIMEPOSITION getCurtp() { return curtp; }
};


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




void GuidoPrintAR(ARHandler ar, std::ostream& out);

void GudioFillMidiPerformance(ARHandler ar, MidiPerformance* perf, const QList<int>& voiceNums = QList<int>());

QPointF GuidoApproximateNoteGraphicalPosition(CGRHandler gr, float width, float height, int32_t timeNum, int32_t timeDenom,
		TYPE_PITCH notePitch, TYPE_REGISTER noteRegister, int& pagenum, QList<QPointF>* ledgerLinePositions = nullptr);

void GuidoCollectStaffs(CGRHandler gr, int pagenum, QList<const GRStaff*>& staffList, std::function<bool(const GRStaff*)> predicate);

void GuidoMidiKeyToPitch(int8_t midiKey, int& pitch, int& octave);

int8_t GuidoPitchToMidiKey(int pitch, int octave);

void GuidoShiftPitch(int& pitch, int& octave, int offset);

void GuidoShiftPitchOnStaffSingle(int& pitch, int& octave, int direction);


#endif /* GUIDOEXTENSIONS_H_ */
