#include <pianotrain/config.h>

#include <QApplication>
#include <QWidget>
#include <QFontDatabase>

#include <GuidoQt/QGuidoWidget.h>
#include <GuidoQt/QGuidoPainter.h>

#include <portmidi.h>

#include <nxcommon/util.h>
#include <nxcommon/CString.h>

#include "MainWindow.h"
#include "MidiService.h"
#include "MidiPerformance.h"




int main(int argc, char** argv)
{
	/*printf("Go\n");
	fflush(stdout);
	return 0;*/

    QApplication app(argc, argv);
    
    qRegisterMetaType<uint8_t>("uint8_t");
    qRegisterMetaType<uint16_t>("uint16_t");
    qRegisterMetaType<uint32_t>("uint32_t");
    qRegisterMetaType<uint64_t>("uint64_t");

    qRegisterMetaType<int8_t>("int8_t");
    qRegisterMetaType<int16_t>("int16_t");
    qRegisterMetaType<int32_t>("int32_t");
    qRegisterMetaType<int64_t>("int64_t");

    QFontDatabase::addApplicationFont(":/fonts/guido2.ttf");
    
    QGuidoPainter::startGuidoEngine();

    MidiService* midi = MidiService::getInstance();

    midi->startup();

    midi->openInput("Digital Piano");


    int deviceId = -1;

    printf("=== AVAILABLE MIDI DEVICES ===\n");
    for (int i = 0 ; i < Pm_CountDevices() ; i++)
    {
    	const PmDeviceInfo* info = Pm_GetDeviceInfo(i);

    	if (info->input)
    	{
    		printf("Device %d (input): %s\n", i, info->name);
    	}
    	else
    	{
    		printf("Device %d (non-input): %s\n", i, info->name);
    	}
    }

    fflush(stdout);

    //perf.hitNote(60, GetMultimediaTimerMilliseconds() + 10);
    //perf.hitNote(60, GetMultimediaTimerMilliseconds() + 700);

    //PmDeviceID defaultInDev = Pm_GetDefaultInputDeviceID();

    /*int deviceId = -1;

    for (int i = 0 ; i < Pm_CountDevices() ; i++)
    {
    	const PmDeviceInfo* info = Pm_GetDeviceInfo(i);

    	if (info->input)
    	{
    		if (strcmp(info->name, "Digital Piano") == 0)
    		{
    			deviceId = i;
    		}
    		printf("Device %d: %s\n", i, info->name);
    	}
    }

    if (deviceId == -1)
    {
    	fprintf(stderr, "ERROR: Digital piano not found in MIDI devices!\n");
    	exit(1);
    }

    StartMultimediaTimer();

    PmStream* midi;

    Pm_OpenInput(&midi, deviceId, NULL, 100, &_PmGetTime, NULL);

    Pm_SetFilter(midi, PM_FILT_ACTIVE | PM_FILT_CLOCK | PM_FILT_SYSEX);

    PmEvent midiEvt;

    while (Pm_Poll(midi))
    {
    	Pm_Read(midi, &midiEvt, 1);
    }

    fflush(stdout);

    printf("Listening to MIDI messages...\n");
    fflush(stdout);

    while (true)
    {
    	if (Pm_Poll(midi)  &&  Pm_Read(midi, &midiEvt, 1) > 0)
    	{
    		int32_t status = Pm_MessageStatus(midiEvt.message);
    		int32_t data1 = Pm_MessageData1(midiEvt.message);
    		int32_t data2 = Pm_MessageData2(midiEvt.message);

    		int32_t channel = status & 0xF;
    		int32_t cmd = status & 0xF0;

    		if (cmd == 0x90)
    		{
    			// Note On event. Also Note Off if velocity == 0

    			int32_t key = data1 & 0x7F;
    			int32_t velocity = data2 & 0x7F;

    			if (velocity > 0)
    			{
    				// Actual Note On

    				printf("%s pressed (velocity: %d)\n", ConvertMidiKeyToString(key).get(), velocity);
    			}
    			else
    			{
    				// Alternative implementation for Note Off

    				printf("%s released\n", ConvertMidiKeyToString(key).get());
    			}
    		}
    		else if (cmd == 0x80)
    		{
    			// Note Off event. Not always implemented (Note On with velocity=0 is often used instead)

    			int32_t key = data1 & 0x7F;
    			int32_t velocity = data2 & 0x7F;

    			printf("%s released\n", ConvertMidiKeyToString(key).get());
    		}
    		else
    		{
    			printf("MIDI message: %02X - %02X %02X\n", status, data1, data2);
    		}

    		fflush(stdout);
    	}
    }*/

	/*QGuidoWidget w;
    w.resize(200, 200);
	w.setGMNCode("[c d e f g a b c2]", QString());
	w.resize(800, 800);
	w.show();*/

    MainWindow* mainWin = new MainWindow;
    mainWin->show();

    int status = app.exec();
    
    QGuidoPainter::stopGuidoEngine();
    
    return status;
}
