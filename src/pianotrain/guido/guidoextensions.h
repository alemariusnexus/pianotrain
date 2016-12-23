#ifndef GUIDOEXTENSIONS_H_
#define GUIDOEXTENSIONS_H_

#include <Guido/GUIDOParse.h>
#include <Guido/GUIDOEngine.h>
#include <Guido/abstract/ARMusicalVoiceState.h>
#include <QtCore/QList>
#include <ostream>

class MidiPerformance;


// We need access to some protected member variables of ARMusicalVoiceState, so we have to
// use a subclass to expose them.
// It's dirty, but it works...
class ARCustomMusicalVoiceState : public ARMusicalVoiceState
{
public:
	GuidoPos& getVpos() { return vpos; }
	TYPE_TIMEPOSITION getCurtp() { return curtp; }
};



void GuidoPrintAR(ARHandler ar, std::ostream& out);

void GudioFillMidiPerformance(ARHandler ar, MidiPerformance* perf, const QList<int>& voiceNums = QList<int>());


#endif /* GUIDOEXTENSIONS_H_ */
