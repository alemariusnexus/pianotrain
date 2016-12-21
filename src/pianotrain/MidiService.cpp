#include "MidiService.h"

#include <nxcommon/util.h>

#include <cstdio>



PmTimestamp _PmGetTime(void*)
{
	return (PmTimestamp) GetMultimediaTimerMilliseconds();
}


CString ConvertMidiKeyToString(int32_t key, int32_t octaveOffset)
{
	const char* relPitchNames[] = {
			"C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B"
	};

	int32_t octave = key / 12; // MIDI octave (0 - 10)
	int32_t relKey = key % 12;

	char str[6];
	sprintf(str, "%s%u", relPitchNames[relKey], octave + octaveOffset);

	return CString(str);
}


int32_t ConvertStringToMidiKey(const CString& str, int32_t octaveOffset)
{
	CString trimmed(str);
	trimmed.trim();

	int8_t baseKey;
	int8_t modifier = 0;
	int8_t octave = 5 + octaveOffset;

	if (trimmed.length() > 0)
	{
		char baseKeyChar = trimmed[0];
		trimmed.remove(0);

		switch (baseKeyChar)
		{
		case 'c':
		case 'C':
			baseKey = 0;
			break;
		case 'd':
		case 'D':
			baseKey = 2;
			break;
		case 'e':
		case 'E':
			baseKey = 4;
			break;
		case 'f':
		case 'F':
			baseKey = 5;
			break;
		case 'g':
		case 'G':
			baseKey = 7;
			break;
		case 'a':
		case 'A':
			baseKey = 9;
			break;
		case 'b':
		case 'B':
		case 'h':
		case 'H':
			baseKey = 11;
			break;
		default:
			baseKey = -1;
		}

		while (trimmed.length() > 0)
		{
			char modifierChar = trimmed[0];

			if (strchr("#", modifierChar) != 0)
			{
				modifier += 1;
				trimmed.remove(0);
			}
			else if (strchr("bB&", modifierChar) != 0)
			{
				modifier -= 1;
				trimmed.remove(0);
			}
			else
			{
				break;
			}
		}

		if (trimmed.length() > 0)
		{
			int octaveInt;
			if (sscanf(trimmed.get(), "%d", &octaveInt) != 0)
			{
				octave = (int8_t) octaveInt;
			}
		}
	}

	octave -= octaveOffset;

	return baseKey + octave*12 + modifier;
}




MidiService* MidiService::getInstance()
{
	static MidiService inst;
	return &inst;
}


void MidiService::startup()
{
}


MidiService::MidiService()
		: midi(nullptr)
{
}


uint64_t MidiService::getCurrentTimestamp() const
{
	return GetMultimediaTimerMilliseconds();
}


QList<MidiService::MidiDevice> MidiService::listDevices() const
{
	QList<MidiDevice> devs;

	for (int i = 0 ; i < Pm_CountDevices() ; i++)
    {
    	const PmDeviceInfo* info = Pm_GetDeviceInfo(i);
    	MidiDevice dev;

    	dev.isInput = (bool) info->input;
    	dev.isOutput = (bool) info->output;
    	dev.name = CString(info->name);

    	devs << dev;
    }

	return devs;
}


void MidiService::openInput(int deviceID)
{
	Pm_OpenInput(&midi, deviceID, NULL, 100, &_PmGetTime, NULL);
    Pm_SetFilter(midi, PM_FILT_ACTIVE | PM_FILT_CLOCK | PM_FILT_SYSEX);

    PmEvent evt;

    // Clear event queue
    while (Pm_Poll(midi))
    {
    	Pm_Read(midi, &evt, 1);
    }

    stopPollThread = false;
    pollThread = std::thread(&MidiService::pollThreadMain, this);
}


bool MidiService::openInput(const CString& name)
{
	for (MidiDevice dev : listDevices())
	{
		if (dev.name == name)
		{
			openInput(dev.id);
			return true;
		}
	}

	return false;
}


void MidiService::pollThreadMain()
{
	PmEvent midiEvt;

	while (!stopPollThread)
	{
		while (Pm_Poll(midi))
		{
			if (Pm_Read(midi, &midiEvt, 1) > 0)
			{
				int32_t status = Pm_MessageStatus(midiEvt.message);
				int32_t data1 = Pm_MessageData1(midiEvt.message);
				int32_t data2 = Pm_MessageData2(midiEvt.message);

				reportMessage(status, data1, data2, (uint64_t) midiEvt.timestamp);
			}
		}

		SleepMilliseconds(1);
	}
}


void MidiService::reportMessage(uint32_t status, uint32_t data1, uint32_t data2, uint64_t timestamp)
{
	emit messageReceived((uint8_t) status, (uint8_t) data1, (uint8_t) data2, timestamp);
}
