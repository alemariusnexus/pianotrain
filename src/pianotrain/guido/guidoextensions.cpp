#include "guidoextensions.h"

#include <Guido/abstract/ARMusic.h>
#include <Guido/abstract/ARMusicalVoice.h>
#include <Guido/abstract/ARMusicalVoiceState.h>
#include <Guido/abstract/ARNoteFormat.h>
#include <Guido/lib/GUIDOInternal.h>

#include <iostream>



// We need access to some protected member variables of ARMusicalVoiceState, so we have to
// use a subclass to expose them.
// It's dirty, but it works...
class ARCustomMusicalVoiceState2 : public ARMusicalVoiceState
{
public:
	GuidoPos& getVpos() { return vpos; }
	TYPE_TIMEPOSITION getCurtp() { return curtp; }
};


void GuidoPrintAR(ARHandler ar, std::ostream& out)
{
	ARMusicalVoice* voice = nullptr;
	ARMusic* music = ar->armusic;

	GuidoPos pos = music->GetHeadPosition();
	while (pos)
	{
		voice = music->GetNext(pos);
		break;
	}

	voice->setReadMode(ARMusicalVoice::CHORDMODE);

	ARCustomMusicalVoiceState2 vst;

	voice->GetHeadPosition(vst);

	while (vst.getVpos())
	{
		GuidoPos prevpos = vst.getVpos();
		const TYPE_TIMEPOSITION tp (vst.getCurtp());
		ARMusicalObject * o = voice->GetNext(vst.getVpos(), vst);
		ARMusicalObject * oEv = ARMusicalEvent::cast(o);

		//printf("At %d, %d:   ", tp.getNumerator(), tp.getDenominator());
		out << "At " << tp.getNumerator() << ", " << tp.getDenominator() << ":   ";
		o->print(out);
	}
}


#if 0
class ARCustomMusicalVoiceState : public ARMusicalVoiceState
{
public:
	GuidoPos& getVpos() { return vpos; }
	TYPE_TIMEPOSITION getCurtp() { return curtp; }
};



ARNoteFormat* elem1 = nullptr;
ARNoteFormat* elem2 = nullptr;


void MarkActiveNotes(ARMusicalVoice* voice, int32_t num, int32_t denom)
{
	printf("\n");

	voice->setReadMode(ARMusicalVoice::CHORDMODE);
	//voice->setReadMode(ARMusicalVoice::EVENTMODE);

	if (elem1)
	{
		voice->RemoveElement(elem1);
	}
	if (elem2)
	{
		voice->RemoveElement(elem2);
	}

	TYPE_TIMEPOSITION tpos(num, denom);
	//TYPE_DURATION duration(lengthnum,lengthdenom);

	//TYPE_TIMEPOSITION endtpos (tpos + duration);

	// now we traverse the voice and try to find the indicated timepositions
	//ARMusicalVoiceState vst;
	ARCustomMusicalVoiceState vst;

	voice->GetHeadPosition(vst);

	//GuidoPos startpos = NULL;
	//GuidoPos endpos = NULL;

	TYPE_TIMEPOSITION activeTimeposStart;
	TYPE_TIMEPOSITION activeTimeposEnd;
	TYPE_TIMEPOSITION activeTimeposCur;

	GuidoPos activePosStart = nullptr;
	GuidoPos activePosEnd = nullptr;
	GuidoPos activePosCur = nullptr;

	while (vst.getVpos())
	{
		GuidoPos prevpos = vst.getVpos();
		// GuidoPos ptagpos = vst.ptagpos;
		const TYPE_TIMEPOSITION tp (vst.getCurtp());
		ARMusicalObject * o = voice->GetNext(vst.getVpos(), vst);
		ARMusicalObject * oEv = ARMusicalEvent::cast(o);

		//o->getRelativeTimePosition()
		TYPE_TIMEPOSITION blat = o->getRelativeTimePosition();
		//printf("At %d, %d (%d, %d):   ", tp.getNumerator(), tp.getDenominator(), blat.getNumerator(), blat.getDenominator());
		o->print(std::cout);

		if (oEv)
		{
			//printf(" -> yes\n");
			//printf("There's an element at %d, %d\n", tp.getNumerator(), tp.getDenominator());
		}

		/*if (oEv && tp <= tpos)
		{
			printf("  -> choose it!\n");
			activePos = prevpos;
			activeTimepos = tp;
		}*/

		if (oEv)
		{
			activeTimeposCur = tp;
			activePosCur = prevpos;
			//printf("  -> cur!\n");

			if (tp <= tpos)
			{
				if (!activePosStart  ||  activeTimeposStart != tp)
				{
					//printf("  -> new start!\n");
					activeTimeposStart = tp;
					activePosStart = prevpos;
				}

				/*activeTimeposEnd = tp;
				activePosEnd = prevpos;
				printf("  -> new end!\n");*/
			}
			else
			{
				activeTimeposEnd = tp;
				activePosEnd = prevpos;
				//printf("  -> new end!\n");
				break;
			}
		}

		/*if (oEv && tp <= tpos)
		{
			if (!activePosStart  ||  activeTimeposStart != tp)
			{
				printf("  -> new start!\n");
				activeTimeposStart = tp;
				activePosStart = prevpos;
			}

			activeTimeposEnd = tp;
			activePosEnd = prevpos;
			printf("  -> new end!\n");
		}*/

		/*if (!startpos && oEv && tp == tpos)
		{
			// found the beginning...(?)
			startpos = prevpos;
		}

		if (startpos && oEv && vst.curtp == endtpos)
		{
			endpos = prevpos;
			break;
		}*/
	}
	// now we have the startpos and the endpos
	// to introduce a noteFormat-tag...

	/*if (!activePosEnd)
	{
		activePosEnd = activePosCur;
		activeTimeposEnd = activeTimeposCur;
		printf("  -> new end!\n");
	}*/

	if (activePosStart)
	{
		//printf("Heydu: %d, %d~~~~~~~~~~\n", activeTimepos.getNumerator(), activeTimepos.getDenominator());
		//fflush(stdout);

		ARNoteFormat * ntformat = new ARNoteFormat;
		ntformat->setRelativeTimePosition(activeTimeposStart);
		//ntformat->setRGBColor(255, 0, 0);
		ntformat->setColor("0xFF0000CC");
		voice->AddElementAt(activePosStart, ntformat);
		elem1 = ntformat;
	}
	if (activePosEnd)
	{
		ARNoteFormat * ntformat = new ARNoteFormat;
		ntformat->setRelativeTimePosition(activeTimeposEnd);
		voice->AddElementAt(activePosEnd, ntformat);
		elem2 = ntformat;
	}
	/*else
	{
		ARNoteFormat * ntformat = new ARNoteFormat;
		ntformat->setRelativeTimePosition(activeTimeposCur);
		voice->AddElementAfter(activePosCur, ntformat);
		elem2 = ntformat;
	}*/

	/*if (startpos && endpos)
	{
		ARNoteFormat * ntformat = new ARNoteFormat;
		ntformat->setRelativeTimePosition(tpos);
		ntformat->setRGBColor(red, green, blue);
		AddElementAt(startpos,ntformat);

		ntformat = new ARNoteFormat;
		ntformat->setRelativeTimePosition(endtpos);
		AddElementAfter(endpos,ntformat);
	}*/

	printf("\n");
	fflush(stdout);

	/*voice->setReadMode(ARMusicalVoice::EVENTMODE);

	printf("=== NEW AST ===\n");
	fflush(stdout);

	voice->GetHeadPosition(vst);

	while (vst.getVpos())
	{
		GuidoPos prevpos = vst.getVpos();
		// GuidoPos ptagpos = vst.ptagpos;
		const TYPE_TIMEPOSITION tp (vst.getCurtp());
		ARMusicalObject * o = voice->GetNext(vst.getVpos(), vst);
		ARMusicalObject * oEv = ARMusicalEvent::cast(o);

		//o->getRelativeTimePosition()
		TYPE_TIMEPOSITION blat = o->getRelativeTimePosition();
		//printf("At %d, %d (%d, %d):   ", tp.getNumerator(), tp.getDenominator(), blat.getNumerator(), blat.getDenominator());
		o->print(std::cout);
	}

	printf("\n");
	fflush(stdout);*/
}


void MarkActiveNotes(ARHandler ar, int voicenum, int32_t num, int32_t denom)
{
	ARMusic* music = ar->armusic;

	GuidoPos pos = music->GetHeadPosition();
	while (pos)
	{
		ARMusicalVoice * arvc = music->GetNext(pos);
		if (arvc->getVoiceNum() == voicenum)
		{
			MarkActiveNotes(arvc, num, denom);
			break;
		}
	}
}
#endif

