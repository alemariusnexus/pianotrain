#include "SightReadingWidget.h"
#include <QtCore/QTimer>
#include <QtCore/QThread>
#include <QSound>
#include <Guido/GUIDOParse.h>
#include <Guido/GUIDOEngine.h>
#include <GuidoQt/QGuidoGraphicsItem.h>
#include <Guido/lib/GUIDOInternal.h>
#include <Guido/abstract/ARMusic.h>
#include <Guido/abstract/ARNote.h>
#include <Guido/graphic/GRMusic.h>
#include <Guido/graphic/GRVoice.h>
#include <Guido/graphic/GRNote.h>
#include <Guido/graphic/GRStaff.h>
#include <Guido/graphic/GRSingleNote.h>
#include <nxcommon/util.h>
#include "guido/guidoextensions.h"
#include "MidiPerformance.h"

#include <iostream>
#include <thread>
#include <algorithm>




SightReadingWidget::SightReadingWidget(QWidget* parent)
		: QWidget(parent), perf(nullptr), metronome(nullptr) //, activePage(-1),
		  //activeLineOverlayColor(QColor(0, 0, 0, 0)), inactiveLineOverlayColor(QColor(0, 0, 0, 20))
{
	ui.setupUi(this);

	connect(ui.generateButton, SIGNAL(clicked()), this, SLOT(onGenerate()));
	connect(ui.prevPageButton, SIGNAL(clicked()), this, SLOT(onPreviousPage()));
	connect(ui.nextPageButton, SIGNAL(clicked()), this, SLOT(onNextPage()));

	//const char* guidoStr = "[ {c,e} d e f {c,e,g} a b c2 d e f g a b ]";
	//const char* guidoStr = "[ {c,d} e f ]";
	//const char* guidoStr = "[ {c/4,d/4} ]";
	//const char* guidoStr = "[ c/8 e g _ {c/4,e,g} d/8 f a {d/4,f,a} e/8 g b {e/4,g,b} f/8 a c2 {f/4,a,c} ]";
	//const char* guidoStr = "[ c/8 e g _ {c/2,e,g} | d/8 f a _ {d/2,f,a} | e/8 g b _ {e/2,g,b} | f/8 a c2 _ {f/2,a,c} ]";
	//const char* guidoStr = "[ \\clef<\"treble\"> c/8 e g _ {c/2,e,g} | c/8 f a _ {c/2,f,a} | c/8 e g _ {c/2,e,g} | b0/8 f1 g _ {b0/2,f1,g} | {c/1,e,g} | c/8 e g _ {c/2,e,g} | c/8 f a _ {c/2,f,a} | c/8 e g _ {c/2,e,g} | b0/8 f1 g _ {b0/2,f1,g} | {c/1,e,g} ]";
	/*const char* guidoStr = "{ [ \\clef<\"treble\"> c/8 e g _ {c/2,e,g} | c/8 f a _ {c/2,f,a} | c/8 e g _ {c/2,e,g} | b0/8 f1 g _ {b0/2,f1,g} | {c/1,e,g} | c/8 e g _ {c/2,e,g} | c/8 f a _ {c/2,f,a} | c/8 e g _ {c/2,e,g} | b0/8 f1 g _ {b0/2,f1,g} | {c/1,e,g} ], "
			"[ \\clef<\"bass\"> c0/8 e g _ {c/2,e,g} | c/8 f a _ {c/2,f,a} | c/8 e g _ {c/2,e,g} | b-1/8 f0 g _ {b-1/2,f0,g} | {c/1,e,g} | c/8 e g _ {c/2,e,g} | c/8 f a _ {c/2,f,a} | c/8 e g _ {c/2,e,g} | b-1/8 f0 g _ {b-1/2,f0,g} | {c/1,e,g} ] }";*/
	/*const char* guidoStr = "{ [ \\clef<\"treble\"> c/8 e g _ {c/2,e,g} | c/8 f a _ {c/2,f,a} | c/8 e g _ {c/2,e,g} | b0/8 f1 g _ {b0/2,f1,g} | {c/1,e,g} ], "
			"[ \\clef<\"bass\"> c0/8 e g _ {c/2,e,g} | c/8 f a _ {c/2,f,a} | c/8 e g _ {c/2,e,g} | b-1/8 f0 g _ {b-1/2,f0,g} | {c/1,e,g} ] }";*/
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
	//const char* guidoStr = "{ [ c# ] }";
	//const char* guidoStr = "{ [ \\clef<\"treble\"> c/8 {d,f} e f g \\newLine a b c2], [ \\clef<\"bass\"> c0/4 d e f ] }";
	//const char* guidoStr = "{ [ \\clef<\"treble\"> c/8 d], [ \\clef<\"bass\"> _/4 ] }";
	//const char* guidoStr = "{ [ \\pageFormat<\"A4\", 0, 0, 0, 0> c d e f | \\newPage g a b c2 | \\newPage d e f g | \\newPage a b c3 ] }";
	const char* guidoStr = "{ [ \\pageFormat<10cm, 30cm, 1mm, 1mm, 1mm, 1mm> "
			"c d e f | \\newPage "
			"g a b c2 | \\newPage "
			"d e f g | \\newPage "
			"a b c3 b2 | \\newPage "
			"a g f e | \\newPage "
			"d c b1 a | \\newPage "
			"g f e d | \\newPage "
			"c d e f | \\newPage "
			"g a b c2 | \\newPage "
			"d e f g | \\newPage "
			"a b c3 {c1,c2,c3} "
			"] }";

	GuidoParser* parser = GuidoOpenParser();
	ar = GuidoString2AR(parser, guidoStr);

	ui.scoreWidget->setARHandler(ar);


	TYPE_DURATION dur = ar->armusic->getDuration();
	musicDurationNum = dur.getNumerator();
	musicDurationDenom = dur.getDenominator();


    metronome = new Metronome;
}


void SightReadingWidget::interruptStuff()
{
	metronome->stop();
	perf->stop();
}


void SightReadingWidget::performanceFinished(bool stopped)
{
	ui.scoreWidget->clearPerformanceMarker();
}


void SightReadingWidget::startPerformance()
{
	perf->start();

	fflush(stdout);
}


void SightReadingWidget::startPerformanceCountoff()
{
	printf("\n");
	fflush(stdout);

	ui.scoreWidget->setActivePage(1);

	ui.scoreWidget->clearMarkers();

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

    connect(perf, SIGNAL(noteExcess(int8_t, int32_t, int32_t)), this, SLOT(noteExcess(int8_t, int32_t, int32_t)));

    perf->setTempo(120);

	metronome->setTicksPerMinute(120);
	metronome->setTicksPerMeasure(4);
	metronome->setNumSubdivisions(2);


	uint32_t numMetronomeTicks = (uint32_t) ceilf((musicDurationNum / (float) musicDurationDenom) * 4);
	metronome->startAtWithLength(GetMultimediaTimerMilliseconds() + 500, numMetronomeTicks, 4);

	perf->startAt(metronome->getPerformanceBeginTime());
}


void SightReadingWidget::currentTickUpdated(int32_t num, int32_t denom)
{
	GuidoDate date = {num, denom};

	TYPE_DURATION duration = ui.scoreWidget->getGRHandler()->grmusic->getDuration();

	if (TYPE_DURATION(num, denom) > duration)
	{
		date = {duration.getNumerator(), duration.getDenominator()};
		num = date.num;
		denom = date.denom;
	}

	int page = GuidoFindPageAt(ui.scoreWidget->getGRHandler(), date);

	ui.scoreWidget->setActivePage(page);
	ui.scoreWidget->setPerformanceMarker(num, denom);
}


void SightReadingWidget::noteHit (
		int8_t midiKey,
		int32_t hitNum, int32_t hitDenom,
		int32_t startNum, int32_t startDenom,
		int32_t lenNum, int32_t lenDenom,
		void* userData
) {
	ui.scoreWidget->markCorrectPlay(midiKey, startNum, startDenom);
}


void SightReadingWidget::noteMissed (
		int8_t midiKey,
		int32_t startNum, int32_t startDenom,
		int32_t lenNum, int32_t lenDenom,
		void* userData
) {
}


void SightReadingWidget::noteExcess (
		int8_t midiKey,
		int32_t hitNum, int32_t hitDenom
) {
	ui.scoreWidget->addExcessNote(hitNum, hitDenom, midiKey);
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

	startPerformanceCountoff();
}


void SightReadingWidget::onPreviousPage()
{
	ui.scoreWidget->setActivePage(ui.scoreWidget->getActivePage() - 1);
}


void SightReadingWidget::onNextPage()
{
	ui.scoreWidget->setActivePage(ui.scoreWidget->getActivePage() + 1);
}

