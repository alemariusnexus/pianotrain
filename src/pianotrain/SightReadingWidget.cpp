#include "SightReadingWidget.h"
#include <QtCore/QTimer>
#include <QtCore/QThread>
#include <QSound>
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
		: QWidget(parent), perf(nullptr), metronome(nullptr)
{
	ui.setupUi(this);

	connect(ui.generateButton, SIGNAL(clicked()), this, SLOT(onGenerate()));

	//const char* guidoStr = "[ {c,e} d e f {c,e,g} a b c2 d e f g a b ]";
	//const char* guidoStr = "[ {c,d} e f ]";
	//const char* guidoStr = "[ {c/4,d/4} ]";
	//const char* guidoStr = "[ c/8 e g _ {c/4,e,g} d/8 f a {d/4,f,a} e/8 g b {e/4,g,b} f/8 a c2 {f/4,a,c} ]";
	//const char* guidoStr = "[ c/8 e g _ {c/2,e,g} | d/8 f a _ {d/2,f,a} | e/8 g b _ {e/2,g,b} | f/8 a c2 _ {f/2,a,c} ]";
	//const char* guidoStr = "[ \\clef<\"treble\"> c/8 e g _ {c/2,e,g} | c/8 f a _ {c/2,f,a} | c/8 e g _ {c/2,e,g} | b0/8 f1 g _ {b0/2,f1,g} | {c/1,e,g} | c/8 e g _ {c/2,e,g} | c/8 f a _ {c/2,f,a} | c/8 e g _ {c/2,e,g} | b0/8 f1 g _ {b0/2,f1,g} | {c/1,e,g} ]";
	/*const char* guidoStr = "{ [ \\clef<\"treble\"> c/8 e g _ {c/2,e,g} | c/8 f a _ {c/2,f,a} | c/8 e g _ {c/2,e,g} | b0/8 f1 g _ {b0/2,f1,g} | {c/1,e,g} | c/8 e g _ {c/2,e,g} | c/8 f a _ {c/2,f,a} | c/8 e g _ {c/2,e,g} | b0/8 f1 g _ {b0/2,f1,g} | {c/1,e,g} ], "
			"[ \\clef<\"bass\"> c0/8 e g _ {c/2,e,g} | c/8 f a _ {c/2,f,a} | c/8 e g _ {c/2,e,g} | b-1/8 f0 g _ {b-1/2,f0,g} | {c/1,e,g} | c/8 e g _ {c/2,e,g} | c/8 f a _ {c/2,f,a} | c/8 e g _ {c/2,e,g} | b-1/8 f0 g _ {b-1/2,f0,g} | {c/1,e,g} ] }";*/
	//const char* guidoStr = "[ \\restFormat<color=\"red\">(_/4) ]";
	//const char* guidoStr = "[ c/8 _ d ]";
	//const char* guidoStr = "[ c/1 g ]";
	//const char* guidoStr = "[ c c/8 _/4 {c,e,g} ]";
	//const char* guidoStr = "[ {d,\\noteFormat<color=\"red\">(f)} ]";
	//const char* guidoStr = "[ c/4 {d,f} b ]";
	//const char* guidoStr = "[ {c/4,d} ]";
	//const char* guidoStr = "[ c \\noteFormat<color=\"red\">({c/4,d/4,e/4}) c ]";
	//const char* guidoStr = "[ c {c/4,d/4,e/4} c ]";
	//const char* guidoStr = "[ c c ]";
	//const char* guidoStr = "[ c/4 d/4 e/4 ]";
	const char* guidoStr = "{ [ \\clef<\"treble\"> c/8 d e f g a b c2], [ \\clef<\"bass\"> c0/4 d e f ] }";
	//const char* guidoStr = "{ [ \\clef<\"treble\"> c/8 d], [ \\clef<\"bass\"> _/4 ] }";

	GuidoParser* parser = GuidoOpenParser();
	ar = GuidoString2AR(parser, guidoStr);
	//ar = GuidoFile2AR(parser, "E:/source/guidolib-1.60-src/gmn-examples/lyrics/bach_lyrics.gmn");

	TYPE_DURATION dur = ar->armusic->getDuration();
	musicDurationNum = dur.getNumerator();
	musicDurationDenom = dur.getDenominator();

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


    metronome = new Metronome;

    //QTimer::singleShot(1000, this, SLOT(startPerformanceCountoff()));

    //QTimer::singleShot(5000, this, SLOT(interruptStuff()));
}


void SightReadingWidget::interruptStuff()
{
	metronome->stop();
	perf->stop();
}


void SightReadingWidget::performanceFinished(bool stopped)
{
	//noteMarker->clearPerformanceMarker();
	ui.guidoWidget->clearPerformanceMarker();

	updateGuidoDisplay();

	printf("Performance %s!\n", stopped ? "stopped" : "finished");
	fflush(stdout);

	//QTimer::singleShot(1000, this, SLOT(startPerformanceCountoff()));
}


void SightReadingWidget::startPerformance()
{
	printf("\n");

	perfStartTime = GetMultimediaTimerMilliseconds();
	perf->start();

	fflush(stdout);
}


void SightReadingWidget::startPerformanceCountoff()
{
	printf("\n");
	fflush(stdout);

	noteMarker->clear();
	ui.guidoWidget->clearPerformanceMarker();

	updateGuidoDisplay();

	if (perf)
	{
		delete perf;
	}

	perf = new MidiPerformance;

	GudioFillMidiPerformance(ar, perf);

	connect(perf, SIGNAL(performanceFinished(bool)), this, SLOT(performanceFinished(bool)));

    connect(perf, SIGNAL(currentTickUpdated(int32_t, int32_t)), this, SLOT(currentTickUpdated(int32_t, int32_t)));

    connect(perf, SIGNAL(noteHit(int8_t, int32_t, int32_t, int32_t, int32_t, int32_t, int32_t, void*)),
    		this, SLOT(noteHit(int8_t, int32_t, int32_t, int32_t, int32_t, int32_t, int32_t, void*)));

    connect(perf, SIGNAL(noteMissed(int8_t, int32_t, int32_t, int32_t, int32_t, void*)),
    		this, SLOT(noteMissed(int8_t, int32_t, int32_t, int32_t, int32_t, void*)));

    perf->setTempo(120);

	metronome->setTicksPerMinute(120);
	metronome->setTicksPerMeasure(4);
	metronome->setNumSubdivisions(2);



	//metronome->startAtWithLength(GetMultimediaTimerMilliseconds() + 500, 8, 4);

	uint32_t numMetronomeTicks = (uint32_t) ceilf((musicDurationNum / (float) musicDurationDenom) * 4);
	metronome->startAtWithLength(GetMultimediaTimerMilliseconds() + 500, numMetronomeTicks, 4);

	QTimer::singleShot(2000 + 500, this, SLOT(startPerformance()));
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

	ui.guidoWidget->setPerformanceMarker(num, denom);

#if 0
	if (noteMarker->setPerformanceMarker(num, denom))
	{
		/*uint64_t ts = GetMultimediaTimerMilliseconds();
		printf("Perf marker set at %llu\n", (long long unsigned) ts);
		fflush(stdout);*/

		updateGuidoDisplay();
	}
#endif
}


void SightReadingWidget::noteHit (
		int8_t midiKey,
		int32_t hitNum, int32_t hitDenom,
		int32_t startNum, int32_t startDenom,
		int32_t lenNum, int32_t lenDenom,
		void* userData
) {
	//printf("At %d, %d: Hit note %d\n", hitNum, hitDenom, midiKey);
	//fflush(stdout);

	noteMarker->markCorrectPlay(midiKey, startNum, startDenom);
	updateGuidoDisplay();
}


void SightReadingWidget::noteMissed (
		int8_t midiKey,
		int32_t startNum, int32_t startDenom,
		int32_t lenNum, int32_t lenDenom,
		void* userData
) {
	//printf("At %d, %d (%u): Missed note %d\n", startNum, startDenom, (unsigned int) (GetMultimediaTimerMilliseconds() - perfStartTime), midiKey);
	//fflush(stdout);

	//noteMarker->markMissedNote(midiKey, startNum, startDenom);
	//updateGuidoDisplay();
}


void SightReadingWidget::onGenerate()
{
	if (metronome)
	{
		metronome->stop();
	}
	if (perf)
	{
		perf->stop();
	}

	noteMarker->clear();
	updateGuidoDisplay();

	QTimer::singleShot(1000, this, SLOT(startPerformanceCountoff()));
}
