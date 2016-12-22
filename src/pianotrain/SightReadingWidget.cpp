#include "SightReadingWidget.h"
#include <QtCore/QTimer>
#include <QtGui/QSound>
#include <Guido/GUIDOParse.h>
#include <Guido/GUIDOEngine.h>
#include <GuidoQt/QGuidoGraphicsItem.h>
#include <Guido/lib/GUIDOInternal.h>
#include <Guido/abstract/ARMusic.h>
#include <nxcommon/util.h>
#include "guido/guidoextensions.h"
#include "MidiPerformance.h"

#include <iostream>
#include <thread>



SightReadingWidget::SightReadingWidget(QWidget* parent)
		: QWidget(parent), metronomeTickCounter(0)
{
	ui.setupUi(this);

	connect(ui.generateButton, SIGNAL(clicked()), this, SLOT(onGenerate()));

	//const char* guidoStr = "[ {c,e} d e f {c,e,g} a b c2 d e f g a b ]";
	//const char* guidoStr = "[ {c,d} e f ]";
	//const char* guidoStr = "[ {c/4,d/4} ]";
	//const char* guidoStr = "[ c/8 e g _ {c/4,e,g} d/8 f a {d/4,f,a} e/8 g b {e/4,g,b} f/8 a c2 {f/4,a,c} ]";
	//const char* guidoStr = "[ c/8 e g _ {c/2,e,g} | d/8 f a _ {d/2,f,a} | e/8 g b _ {e/2,g,b} | f/8 a c2 _ {f/2,a,c} ]";
	const char* guidoStr = "[ \\clef<\"g2\"> c/8 e g _ {c/2,e,g} | c/8 f a _ {c/2,f,a} | c/8 e g _ {c/2,e,g} | b0/8 f1 g _ {b0/2,f1,g} | {c/1,e,g} ]";
	//const char* guidoStr = "[ \\restFormat<color=\"red\">(_/4) ]";
	//const char* guidoStr = "[ c/8 _ d ]";
	//const char* guidoStr = "[ c c/8 _/4 {c,e,g} ]";
	//const char* guidoStr = "[ {d,\\noteFormat<color=\"red\">(f)} ]";
	//const char* guidoStr = "[ c/4 {d,f} b ]";
	//const char* guidoStr = "[ {c/4,d} ]";
	//const char* guidoStr = "[ c \\noteFormat<color=\"red\">({c/4,d/4,e/4}) c ]";
	//const char* guidoStr = "[ c {c/4,d/4,e/4} c ]";
	//const char* guidoStr = "[ c c ]";
	//const char* guidoStr = "[ c/4 d/4 e/4 ]";

	GuidoParser* parser = GuidoOpenParser();
	ar = GuidoString2AR(parser, guidoStr);

	noteMarker = new GuidoNoteMarker(ar);

	/*printf("\n=== NEW AST ===\n");
	GuidoPrintAR(ar, std::cout);
	printf("\n");
	fflush(stdout);

	printf("\n");
	printf("=== MARKING 1 ===\n");
	noteMarker->markMissedNote(60, 0, 4);

	printf("\n=== NEW AST ===\n");
	GuidoPrintAR(ar, std::cout);
	printf("\n");
	fflush(stdout);

	printf("\n");
	printf("=== MARKING 2 ===\n");
	noteMarker->markMissedNote(62, 1, 4);

	printf("\n");
	fflush(stdout);
	noteMarker->markMissedNote(65, 1, 4);
	noteMarker->markMissedNote(71, 2, 4);

	printf("\n=== NEW AST ===\n");
	GuidoPrintAR(ar, std::cout);
	printf("\n");
	fflush(stdout);*/

	ui.guidoWidget->setARHandler(ar);


	perf = new MidiPerformance;

	GudioFillMidiPerformance(ar, perf);

    connect(perf, SIGNAL(currentTickUpdated(int32_t, int32_t)), this, SLOT(currentTickUpdated(int32_t, int32_t)));

    connect(perf, SIGNAL(noteHit(int8_t, int32_t, int32_t, int32_t, int32_t, int32_t, int32_t, void*)),
    		this, SLOT(noteHit(int8_t, int32_t, int32_t, int32_t, int32_t, int32_t, int32_t, void*)));

    connect(perf, SIGNAL(noteMissed(int8_t, int32_t, int32_t, int32_t, int32_t, void*)),
    		this, SLOT(noteMissed(int8_t, int32_t, int32_t, int32_t, int32_t, void*)));

    perf->setTempo(60);

    /*SleepMilliseconds(3000.0f);

    printf("1...\n"); fflush(stdout);
    SleepMilliseconds(1000.0f);

    printf("2...\n"); fflush(stdout);
    SleepMilliseconds(1000.0f);

    printf("3...\n"); fflush(stdout);
    SleepMilliseconds(1000.0f);

    printf("4...\n"); fflush(stdout);
    SleepMilliseconds(1000.0f);

    printf("\n"); fflush(stdout);*/

    //perf->start();

    //std::thread thr(&SightReadingWidget::startPerformance, this);
    //thr.detach();

    QTimer::singleShot(10000, this, SLOT(startPerformance()));
    //startPerformance();



    /*perf->hitNote(67, 2, 8);
    perf->hitNote(64, 4, 8);
    perf->hitNote(67, 4, 8);*/

    //perf->hitNote(61, 1, 12);
}


void SightReadingWidget::delayMilliseconds(uint64_t ms)
{
	SleepMilliseconds((float) ms);
	/*uint64_t delayEnd = GetMultimediaTimerMilliseconds() + ms;

	uint64_t now;

	while ((now = GetMultimediaTimerMilliseconds()) < delayEnd)
	{
		QCoreApplication::processEvents(QEventLoop::AllEvents, std::min(100u, (unsigned int) (delayEnd-now)));
	}*/
}


void SightReadingWidget::metronomeTick()
{
	/*uint64_t ts = GetMultimediaTimerMilliseconds();
	printf("Sound played at %llu\n", (long long unsigned) ts);
	fflush(stdout);*/

	if (metronomeTickCounter%2 == 0)
	{
		QSound::play("C:/Program Files (x86)/Open Metronome/Samples/Bass Drum 1.wav");
	}
	else
	{
		QSound::play("C:/Program Files (x86)/Open Metronome/Samples/Tambourine.wav");
	}

	if (metronomeTickCounter < 8)
	{
		if (metronomeTickCounter%2 == 0)
		{
			printf("%u... ", (metronomeTickCounter/2)+1);
		}
		else
		{
			printf("and...\n");
		}

		fflush(stdout);
	}
	else if (metronomeTickCounter == 8)
	{
		printf("\n");
		fflush(stdout);

		perfStartTime = GetMultimediaTimerMilliseconds();
		perf->start();
	}

	metronomeTickCounter++;
}


void SightReadingWidget::startPerformance()
{
	//SleepMilliseconds(10000.0f);

	metronomeTimer = new QTimer(this);
	connect(metronomeTimer, SIGNAL(timeout()), this, SLOT(metronomeTick()));
	metronomeTimer->start(500);

	/*for (int i = 0 ; i < 2 ; i++)
	{
		//QSound::play("C:/Program Files (x86)/Open Metronome/Samples/ZZ_Default.wav");
		printf("1..."); fflush(stdout);
		//SleepMilliseconds(500.0f);
		delayMilliseconds(500);

		//QSound::play("C:/Program Files (x86)/Open Metronome/Samples/ZZ_Default.wav");

		printf(" and...\n"); fflush(stdout);
		//SleepMilliseconds(500.0f);
		delayMilliseconds(500);

		//QSound::play("C:/Program Files (x86)/Open Metronome/Samples/ZZ_Default.wav");
		printf("2..."); fflush(stdout);
		//SleepMilliseconds(500.0f);
		delayMilliseconds(500);

		printf(" and...\n"); fflush(stdout);
		//SleepMilliseconds(500.0f);
		delayMilliseconds(500);

		//QSound::play("C:/Program Files (x86)/Open Metronome/Samples/ZZ_Default.wav");
		printf("3..."); fflush(stdout);
		//SleepMilliseconds(500.0f);
		delayMilliseconds(500);

		printf(" and...\n"); fflush(stdout);
		//SleepMilliseconds(500.0f);
		delayMilliseconds(500);

		//QSound::play("C:/Program Files (x86)/Open Metronome/Samples/ZZ_Default.wav");
		printf("4..."); fflush(stdout);
		//SleepMilliseconds(500.0f);
		delayMilliseconds(500);

		printf(" and...\n"); fflush(stdout);
		//SleepMilliseconds(500.0f);
		delayMilliseconds(500);
	}

    printf("\n"); fflush(stdout);*/
}


void SightReadingWidget::updateGuidoDisplay()
{
	ar->refCount += 2;
	ui.guidoWidget->setARHandler(ar);
	ar->refCount -= 1;
}


void SightReadingWidget::currentTickUpdated(int32_t num, int32_t denom)
{
	if (num / (float) denom > 0.5f)
	{
		//noteMarker->clearCorrectPlayMarkers();
	}

	if (noteMarker->setPerformanceMarker(num, denom))
	{
		/*uint64_t ts = GetMultimediaTimerMilliseconds();
		printf("Perf marker set at %llu\n", (long long unsigned) ts);
		fflush(stdout);*/

		updateGuidoDisplay();
	}

	//updateGuidoDisplay();
	//ar->refCount += 2;
	//ui.guidoWidget->setARHandler(ar);
	//ar->refCount -= 1;
}


void SightReadingWidget::noteHit (
		int8_t midiKey,
		int32_t hitNum, int32_t hitDenom,
		int32_t startNum, int32_t startDenom,
		int32_t lenNum, int32_t lenDenom,
		void* userData
) {
	printf("At %d, %d: Hit note %d\n", hitNum, hitDenom, midiKey);
	fflush(stdout);

	noteMarker->markCorrectPlay(midiKey, startNum, startDenom);
	updateGuidoDisplay();
}


void SightReadingWidget::noteMissed (
		int8_t midiKey,
		int32_t startNum, int32_t startDenom,
		int32_t lenNum, int32_t lenDenom,
		void* userData
) {
	printf("At %d, %d (%u): Missed note %d\n", startNum, startDenom, (unsigned int) (GetMultimediaTimerMilliseconds() - perfStartTime), midiKey);
	fflush(stdout);

	//noteMarker->markMissedNote(midiKey, startNum, startDenom);
	//updateGuidoDisplay();
}


void SightReadingWidget::onGenerate()
{
}
