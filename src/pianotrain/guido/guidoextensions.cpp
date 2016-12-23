#include "guidoextensions.h"
#include "MidiPerformance.h"

#include <Guido/abstract/ARMusic.h>
#include <Guido/abstract/ARMusicalObject.h>
#include <Guido/abstract/ARNote.h>
#include <Guido/abstract/ARRest.h>
#include <Guido/abstract/ARMusicalVoice.h>
#include <Guido/abstract/ARMusicalVoiceState.h>
#include <Guido/abstract/ARNoteFormat.h>
#include <Guido/lib/GUIDOInternal.h>


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

	if (!voice)
	{
		return;
	}

	//voice->setReadMode(ARMusicalVoice::CHORDMODE);
	voice->setReadMode(ARMusicalVoice::EVENTMODE);

	ARCustomMusicalVoiceState vst;

	voice->GetHeadPosition(vst);

	while (vst.getVpos())
	{
		GuidoPos prevpos = vst.getVpos();
		const TYPE_TIMEPOSITION tp (vst.getCurtp());
		ARMusicalObject * o = voice->GetNext(vst.getVpos(), vst);
		ARMusicalObject * oEv = ARMusicalEvent::cast(o);

		out << "At " << tp.getNumerator() << ", " << tp.getDenominator() << ":   ";
		o->print(out);
	}
}


void GudioFillMidiPerformance(ARHandler ar, MidiPerformance* perf, const QList<int>& voiceNums)
{
	ARMusicalVoice* voice = nullptr;
	ARMusic* music = ar->armusic;

	GuidoPos musicPos = music->GetHeadPosition();
	while (musicPos)
	{
		ARMusicalVoice* voice = music->GetNext(musicPos);

		if (voiceNums.isEmpty()  ||  voiceNums.contains(voice->getVoiceNum()))
		{
			voice->setReadMode(ARMusicalVoice::EVENTMODE);

			ARCustomMusicalVoiceState vst;

			voice->GetHeadPosition(vst);

			TYPE_TIMEPOSITION timeposBeforeChord;
			bool chordCommaActive = false;

			TYPE_DURATION lastValidDuration;

			while (vst.getVpos())
			{
				GuidoPos prevpos = vst.getVpos();
				const TYPE_TIMEPOSITION tp (vst.getCurtp());
				ARMusicalObject * o = voice->GetNext(vst.getVpos(), vst);

				if (o)
				{
					if (o->isARChordComma())
					{
						chordCommaActive = true;
					}
					else if (o->isARNote())
					{
						ARNote* note = (ARNote*) ARNote::cast(o);
						int8_t midiKey = note->getMidiPitch();
						TYPE_DURATION dur = note->getDuration();

						TYPE_TIMEPOSITION actualTimepos = tp;

						if (chordCommaActive)
						{
							actualTimepos = timeposBeforeChord;
						}
						else
						{
							timeposBeforeChord = tp;
						}

						if (dur.getNumerator() <= 0)
						{
							// This happens e.g. for chords, where the individual notes have zero duration and the
							// duration is instead specified by an "empty" marker note preceding the chord
							dur = lastValidDuration;
						}
						else
						{
							lastValidDuration = dur;
						}

						// note->getMidiPitch() will return -1 e.g. for "marker" notes at the start of a chord
						if (midiKey >= 0)
						{
							perf->addAbsoluteNote(midiKey, actualTimepos.getNumerator(), actualTimepos.getDenominator(),
									dur.getNumerator(), dur.getDenominator(), note);
						}

						chordCommaActive = false;
					}
					else if (o->isARRest())
					{
						ARRest* rest = (ARRest*) ARRest::cast(o);
						TYPE_DURATION dur = rest->getDuration();

						TYPE_TIMEPOSITION actualTimepos = tp;

						if (chordCommaActive)
						{
							actualTimepos = timeposBeforeChord;
						}
						else
						{
							timeposBeforeChord = tp;
						}

						if (dur.getNumerator() <= 0)
						{
							// s.a.
							dur = lastValidDuration;
						}
						else
						{
							lastValidDuration = dur;
						}

						perf->addAbsoluteRest(actualTimepos.getNumerator(), actualTimepos.getDenominator(),
								dur.getNumerator(), dur.getDenominator(), rest);

						chordCommaActive = false;
					}
				}
			}
		}
	}
}

