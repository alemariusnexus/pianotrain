#include "guidoextensions.h"
#include "MidiPerformance.h"

#include <Guido/abstract/ARMusic.h>
#include <Guido/abstract/ARMusicalObject.h>
#include <Guido/abstract/ARNote.h>
#include <Guido/abstract/ARRest.h>
#include <Guido/abstract/ARKey.h>
#include <Guido/abstract/ARMusicalVoice.h>
#include <Guido/abstract/ARMusicalVoiceState.h>
#include <Guido/abstract/ARNoteFormat.h>
#include <Guido/graphic/GRMusic.h>
#include <Guido/graphic/GRPage.h>
#include <Guido/graphic/GRSystem.h>
#include <Guido/graphic/GRStaff.h>
#include <Guido/graphic/GREvent.h>
#include <Guido/graphic/GRKey.h>
#include <Guido/misc/kf_ivect.h>
#include <Guido/lib/GUIDOInternal.h>
#include <cassert>




/*class _GRInternalSystem : public GRSystem
{
public:
	_GRInternalSystem(const GRSystem* system) : system(system) {}

	std::vector<GRAccolade*>* getAccolades() const { return mAccolade; }

private:
	const GRSystem* system;
};*/





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


void GuidoCollectStaffs(CGRHandler gr, int pagenum, QList<const GRStaff*>& staffList, std::function<bool(const GRStaff*)> predicate)
{
	const GRMusic* music = gr->grmusic;
	const GRPage* page = music->getPage(pagenum);

	if (page)
	{
		const SystemPointerList* systems = page->getSystems();

		if (systems)
		{
			for (const GRSystem* system : *systems)
			{
				TYPE_TIMEPOSITION systemStart = system->getRelativeTimePosition();
				TYPE_TIMEPOSITION systemEnd = system->getRelativeEndTimePosition();

				const StaffVector* staffs = system->getStaves();

				if (staffs)
				{
					for (int i = staffs->GetMinimum() ; i <= staffs->GetMaximum() ; i++)
					{
						const GRStaff * staff = staffs->Get(i);

						if (predicate(staff))
						{
							staffList << staff;
						}
					}
				}
				else
				{
					const SSliceList* slices = const_cast<GRSystem*>(system)->getSlices();

					if (slices  &&  slices->size() != 0)
					{
						GuidoPos pos = slices->GetHeadPosition();
						while (pos)
						{
							const GRSystemSlice* slice = slices->GetNext(pos);

							const StaffVector* staffs = const_cast<GRSystemSlice*>(slice)->getStaves();

							if (staffs)
							{
								for (int i = staffs->GetMinimum() ; i <= staffs->GetMaximum() ; i++)
								{
									const GRStaff* staff = staffs->Get(i);

									if (predicate(staff))
									{
										staffList << staff;
									}
								}
							}
						}
					}

					// NOTE: GRAccolade doesn't seem to have staves (?)
					/*else
					{
						_GRInternalSystem isys(system);
						std::vector<GRAccolade*>* accolades = isys.getAccolades();

						if (accolades)
						{
							for (const GRAccolade* accolade : *accolades)
							{

							}
						}
					}*/
				}
			}
		}
	}
}


QPointF GuidoApproximateNoteGraphicalPosition(CGRHandler gr, int pagenum, float width, float height, int32_t timeNum, int32_t timeDenom,
		TYPE_PITCH notePitch, TYPE_REGISTER noteRegister, QList<QPointF>* ledgerLinePositions)
{

	// !!!!!!!!!! HERE BE DRAGONS !!!!!!!!!!


	TYPE_TIMEPOSITION timepos(timeNum, timeDenom);
	float ftimepos = timeNum / (float) timeDenom;



	// ********** Find all staffs that include the timepos **********

	const GRMusic* music = gr->grmusic;
	const GRPage* page = music->getPage(pagenum);

	TYPE_TIMEPOSITION pageStart = page->getRelativeTimePosition();
	TYPE_TIMEPOSITION pageEnd = page->getRelativeEndTimePosition();

	// Clamp timepos to page duration
	if (timepos < pageStart)
	{
		timepos = pageStart;
	}
	else if (timepos > pageEnd)
	{
		timepos = pageEnd;
	}

	timeNum = timepos.getNumerator();
	timeDenom = timepos.getDenominator();

	QList<const GRStaff*> possibleStaffs;

	GuidoCollectStaffs(gr, pagenum, possibleStaffs, [timepos](const GRStaff* staff) {
		TYPE_TIMEPOSITION staffStart = staff->getRelativeTimePosition();
		TYPE_TIMEPOSITION staffEnd = staff->getRelativeEndTimePosition();

		return timepos >= staffStart  &&  timepos <= staffEnd;
	});



	// ********** Select the staff where the note fits most naturally **********

	QList<const GRStaff*> closestStaffs;
	int closestNumLedgerLines = INT_MAX;

	// Select staffs with least number of ledger lines necessary
	for (const GRStaff* staff : possibleStaffs)
	{
		int numLedgerLines = abs(staff->getNumHelplines(notePitch, noteRegister));

		if (numLedgerLines < closestNumLedgerLines)
		{
			// Found better staff
			closestStaffs.clear();
			closestStaffs << staff;
			closestNumLedgerLines = numLedgerLines;
		}
		else if (numLedgerLines == closestNumLedgerLines)
		{
			// Found equally good staff
			closestStaffs << staff;
		}
	}

	if (closestStaffs.isEmpty())
	{
		return QPointF();
	}

	// TODO: Find a better way to select the best among equally close staffs
	const GRStaff* bestStaff = closestStaffs[0];



	// NOTE: This was previously done to get the current key signature (to calculate which accidentals
	// are needed. I removed it, because to determine the correct accidentals, we also need to take
	// accidentals of the previous notes in the same measure into account, which is a pain in the ass.
	// Instead, I decided to ignore active accidentals (key signature etc.), which means that e.g.
	// excess notes are always rendered with only sharps, as if the key signature was C major without
	// additional accidentals.
	// TODO: This is not optimal. Maybe change it?
	/*// ********** Find key signature **********

	const NEPointerList* staffElements = const_cast<GRStaff*>(bestStaff)->getElements();

	const GRKey* activeKey = nullptr;

	GuidoPos pos = staffElements->GetHeadPosition();
	while (pos)
	{
		const GRNotationElement* elem = staffElements->GetNext(pos);
		const GRKey* key = dynamic_cast<const GRKey*>(elem);

		if (key)
		{
			TYPE_TIMEPOSITION keyStart = key->getRelativeTimePosition();

			const ARKey* arKey = (const ARKey*) key->getAbstractRepresentation();
			TYPE_DURATION dur = arKey->getDuration();

			if (keyStart <= timepos)
			{
				// NOTE: The >= (as opposed to >) is crucial here. When a key signature changes, two GRKey elements
				// will appear in the GR: The first one neutralizes the previous key signature (all naturals), and
				// the second one introduces the new key signature. Obviously, we want the second one.
				// TODO: This relies on the correct ordering of these elements. They appear at the same timepos, so
				// can we really rely on that?
				if (!activeKey  ||  keyStart >= activeKey->getRelativeTimePosition())
				{
					activeKey = key;
				}
			}
		}
	}*/


	// ********** Find a list of possible anchors for the note, find y position **********

	struct NoteAnchor
	{
		TYPE_TIMEPOSITION startPos;
		float x;
		const char* type;
	};

	QList<NoteAnchor> possibleAnchors;

	float staffToOutScaleY = 1.0f;
	float staffToOutOffsetY = 0.0f;

	float posY = 0.0f;

	if (page  &&  bestStaff)
	{
		// NOTE: Much of this code mirrors GRSystem::GetMap() and the various GetMap() calls done from there
		//
		// Unfortunately, to call staff->GetMap() we need to fill the MapInfos parameter with valid info. We will use
		// staff->GetMap() instead of manually looping through the elements of staff because some conversion of time
		// positions and the graphics rect takes place in staff->GetMap(), which would otherwise need to be done
		// manually.

		// From GRSystem::GetMap()
		MapInfos infos;
		infos.fScale.x = width;
		infos.fScale.y = height;
		page->getScaling(infos.fScale.x, infos.fScale.y);

		// From GRPage::GetMap()
		infos.fPos.x += page->getMarginLeft();
		infos.fPos.y += page->getMarginTop();

		GRSystem* system = bestStaff->getGRSystem();

		// From GRSystem::GetMap()
		infos.fPos.x += system->getPosition().x;
		infos.fPos.y += system->getPosition().y;


		// Finally, we have a valid MapInfo...

		// Do it now, because we need the scale and offset values calculated above
		float offsetY = infos.fPos.y + bestStaff->getPosition().y;
		if (bestStaff->getGRSystemSlice())
		{
			offsetY += bestStaff->getGRSystemSlice()->getPosition().y;
		}

		staffToOutOffsetY = offsetY;
		staffToOutScaleY = infos.fScale.y;

		posY = (bestStaff->getNotePosition(notePitch, noteRegister) + offsetY) * infos.fScale.y;

		// Collect events in the chosen staff!
		FunctionalMapCollector<void> eventCollector(nullptr,
				[&possibleAnchors](void*, const FloatRect& rect, const TimeSegment& ts, const GuidoElementInfos& info)
		{
			float fstart = ts.first.num / (float) ts.first.denom;
			float fend = ts.second.num / (float) ts.second.denom;

			if (fstart != fend)
			{
				NoteAnchor anchor;
				anchor.startPos = TYPE_TIMEPOSITION(ts.first.num, ts.first.denom);
				anchor.x = 0.5f * (rect.left + rect.right);
				anchor.type = "event";
				possibleAnchors << anchor;
			}
		});
		bestStaff->GetMap(kGuidoEvent, eventCollector, infos);

		printf("\n");
		fflush(stdout);

		// Collect the staff itself
		FunctionalMapCollector<void> staffCollector(nullptr,
				[&possibleAnchors](void*, const FloatRect& rect, const TimeSegment& ts, const GuidoElementInfos& info)
		{
			NoteAnchor startAnchor;
			startAnchor.startPos = TYPE_TIMEPOSITION(ts.first.num, ts.first.denom);
			startAnchor.x = rect.left;
			startAnchor.type = "staff start";
			possibleAnchors << startAnchor;

			NoteAnchor endAnchor;
			endAnchor.startPos = TYPE_TIMEPOSITION(ts.second.num, ts.second.denom);
			endAnchor.x = rect.right;
			endAnchor.type = "staff end";
			possibleAnchors << endAnchor;
		});
		bestStaff->GetMap(kGuidoStaff, staffCollector, infos);


		// From GRSystem::GetMap()
		infos.fPos.x -= system->getPosition().x;
		infos.fPos.y -= system->getPosition().y;

		// From GRPage::GetMap()
		infos.fPos.x -= page->getMarginLeft();
		infos.fPos.y -= page->getMarginTop();
	}



	// ********** Select the best previous and next anchor **********

	NoteAnchor prevAnchor;
	NoteAnchor nextAnchor;

	prevAnchor.startPos = TYPE_TIMEPOSITION(INT_MIN, 1);
	nextAnchor.startPos = TYPE_TIMEPOSITION(INT_MAX, 1);

	for (NoteAnchor anchor : possibleAnchors)
	{
		if (timepos >= anchor.startPos  &&  anchor.startPos > prevAnchor.startPos)
		{
			prevAnchor = anchor;
		}

		if (timepos <= anchor.startPos  &&  anchor.startPos < nextAnchor.startPos)
		{
			nextAnchor = anchor;
		}
	}



	// ********** Interpolate x position between anchors **********

	float fprev = prevAnchor.startPos.getNumerator() / (float) prevAnchor.startPos.getDenominator();
	float fnext = nextAnchor.startPos.getNumerator() / (float) nextAnchor.startPos.getDenominator();

	float t = 0.0f;

	if (fabs(fnext-fprev) > 0.001f)
	{
		t = (ftimepos - fprev) / (fnext - fprev);
	}

	float posX = t * nextAnchor.x + (1.0f-t) * prevAnchor.x;

	QPointF notePos(posX, posY);



	// ********** Calculate ledger lines positions **********

	if (ledgerLinePositions)
	{
		int numLedgerLines = bestStaff->getNumHelplines(notePitch, noteRegister);

		if (numLedgerLines != 0)
		{
			float localPosX = bestStaff->getNotePosition(notePitch, noteRegister);
			float lspace = bestStaff->getStaffLSPACE();

			// See if the note is on or next to a ledger line
			bool onLedgerLine;

			int shiftedPitch = notePitch;
			int shiftedOctave = noteRegister;
			float ledgerDirection; // Direction in which y moves for ledger lines, STARTING AT THE NOTE

			if (numLedgerLines > 0)
			{
				ledgerDirection = 1.0f; // [sic]
				GuidoShiftPitchOnStaffSingle(shiftedPitch, shiftedOctave, 1);
				onLedgerLine = (bestStaff->getNumHelplines(shiftedPitch, shiftedOctave) == numLedgerLines);
			}
			else
			{
				ledgerDirection = -1.0f; // [sic]
				GuidoShiftPitchOnStaffSingle(shiftedPitch, shiftedOctave, -1);
				onLedgerLine = (bestStaff->getNumHelplines(shiftedPitch, shiftedOctave) == numLedgerLines);
			}

			numLedgerLines = abs(numLedgerLines);

			float ledgerOffset = 0.0f;

			if (!onLedgerLine)
			{
				ledgerOffset += 0.5f*ledgerDirection*lspace;
			}

			for (int i = 0 ; i < numLedgerLines ; i++, ledgerOffset += lspace*ledgerDirection)
			{
				QPointF ledgerPos(notePos.x(), notePos.y() + ledgerOffset*staffToOutScaleY);
				*ledgerLinePositions << ledgerPos;
			}
		}
	}



	return notePos;
}


void GuidoMidiKeyToPitch(int8_t midiKey, int& pitch, int& octave)
{
	octave = midiKey/12 - 4;

	int8_t baseNote = midiKey % 12;

	switch (baseNote)
	{
	case 0:
		pitch = NOTE_C;
		break;
	case 1:
		pitch = NOTE_CIS;
		break;
	case 2:
		pitch = NOTE_D;
		break;
	case 3:
		pitch = NOTE_DIS;
		break;
	case 4:
		pitch = NOTE_E;
		break;
	case 5:
		pitch = NOTE_F;
		break;
	case 6:
		pitch = NOTE_FIS;
		break;
	case 7:
		pitch = NOTE_G;
		break;
	case 8:
		pitch = NOTE_GIS;
		break;
	case 9:
		pitch = NOTE_A;
		break;
	case 10:
		pitch = NOTE_AIS;
		break;
	case 11:
		pitch = NOTE_H;
		break;
	default:
		assert(false);
	}
}


int8_t GuidoPitchToMidiKey(int pitch, int octave)
{
	// From ARNote::getMidiPitch()

	int oct = 12 * (octave+4);
	if (oct < 0) return 0;

	int midiPitch = -1;
	switch (pitch) {
		case NOTE_C:
		case NOTE_D:
		case NOTE_E:	midiPitch = (pitch - NOTE_C) * 2;
			break;
		case NOTE_F:
		case NOTE_G:
		case NOTE_A:
		case NOTE_H:	midiPitch = (pitch - NOTE_C) * 2 - 1;
			break;

		case NOTE_CIS:
		case NOTE_DIS:  midiPitch = (pitch - NOTE_CIS) * 2 + 1;
			break;

		case NOTE_FIS:
		case NOTE_GIS:
		case NOTE_AIS:  midiPitch = (pitch - NOTE_CIS) * 2 + 3;
			break;
		default:
			return midiPitch;
	}
	return oct + midiPitch;
}


void GuidoShiftPitch(int& pitch, int& octave, int offset)
{
	// TODO: Do this properly, it doesn't work when midiKey is out of range
	int8_t midiKey = GuidoPitchToMidiKey(pitch, octave) + offset;
	GuidoMidiKeyToPitch(midiKey, pitch, octave);
}


void GuidoShiftPitchOnStaffSingle(int& pitch, int& octave, int direction)
{
	if (direction == 0)
	{
		return;
	}

	switch (pitch)
	{
	case NOTE_CIS:
		pitch = NOTE_C;
		break;
	case NOTE_DIS:
		pitch = NOTE_D;
		break;
	case NOTE_FIS:
		pitch = NOTE_F;
		break;
	case NOTE_GIS:
		pitch = NOTE_G;
		break;
	case NOTE_AIS:
		pitch = NOTE_A;
		break;
	default:
		break;
	}

	switch (pitch)
	{
		case NOTE_C:
			if (direction > 0)
			{
				pitch = NOTE_D;
			}
			else
			{
				pitch = NOTE_H;
				octave--;
			}
			break;

		case NOTE_D:
		case NOTE_E:
		case NOTE_F:
		case NOTE_G:
		case NOTE_A:
			pitch += direction;
			break;

		case NOTE_H:
			if (direction > 0)
			{
				pitch = NOTE_C;
				octave++;
			}
			else
			{
				pitch = NOTE_A;
			}
			break;
	}
}


/*void GuidoShiftPitchOnStaff(int& pitch, int& octave, int offset)
{
	int basePitch;

	switch (pitch)
	{
		case NOTE_C:
		case NOTE_D:
		case NOTE_E:
		case NOTE_F:
		case NOTE_G:
		case NOTE_A:
		case NOTE_H:
			basePitch = pitch;
			break;
		case NOTE_CIS:
			basePitch = NOTE_C;
			break;
		case NOTE_DIS:
			basePitch = NOTE_D;
			break;
		case NOTE_FIS:
			basePitch = NOTE_F;
			break;
		case NOTE_GIS:
			basePitch = NOTE_G;
			break;
		case NOTE_AIS:
			basePitch = NOTE_A;
			break;
	}

	int shiftedBasePitch = ((basePitch - NOTE_C) + offset + (NOTE_H-NOTE_C+1)*1000000) % (NOTE_H-NOTE_C+1);
}*/

