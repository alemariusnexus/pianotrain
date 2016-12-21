#include "SightReadingWidget.h"
#include <Guido/GUIDOParse.h>
#include <Guido/GUIDOEngine.h>
#include <GuidoQt/QGuidoGraphicsItem.h>
#include <Guido/lib/GUIDOInternal.h>
#include <Guido/abstract/ARMusic.h>
#include "guido/guidoextensions.h"
#include "MidiPerformance.h"

#include <iostream>



SightReadingWidget::SightReadingWidget(QWidget* parent)
		: QWidget(parent)
{
	ui.setupUi(this);

	connect(ui.generateButton, SIGNAL(clicked()), this, SLOT(onGenerate()));

	//ui.guidoWidget->setGMNCode("[ c d \\noteFormat<color=\"red\">(e) f ]", "C:/Users/alemariusnexus/gmn");

	//const char* guidoStr = "[ {c,e} d e f {c,e,g} a b c2 d e f g a b ]";
	//const char* guidoStr = "[ {c,d} e f ]";
	const char* guidoStr = "[ c/8 e g _ {c/4,e,g} d/8 f a {d/4,f,a} e/8 g b {e/4,g,b} f/8 a c2 {f/4,a,c} ]";
	//const char* guidoStr = "[ \\restFormat<color=\"red\">(_/4) ]";
	//const char* guidoStr = "[ c/8 _ d ]";
	//const char* guidoStr = "[ c \\noteFormat<color=\"red\">({c/4,d/4,e/4}) c ]";
	//const char* guidoStr = "[ c {c/4,d/4,e/4} c ]";
	//const char* guidoStr = "[ c c ]";
	//const char* guidoStr = "[ c/4 d/4 e/4 ]";
	//ui.guidoWidget->setGMNCode(guidoStr, "C:/Users/alemariusnexus/gmn");

	GuidoParser* parser = GuidoOpenParser();
	ar = GuidoString2AR(parser, guidoStr);

	GuidoDate start;
	start.num = 1;
	start.denom = 4;

	GuidoDate dur;
	dur.num = 2;
	dur.denom = 4;

	//MarkActiveNotes(ar, 1, 3, 8);

	//printf("%d\n", GuidoMarkVoice(ar, 1, start, dur, 255, 0, 0));
	fflush(stdout);

	/*QGraphicsScene* scene = new QGraphicsScene(ui.guidoWidget);

	QGuidoGraphicsItem* guidoItem = new QGuidoGraphicsItem;
	guidoItem->setGMNCode(guidoStr, "C:/Users/alemariusnexus/gmn");


	scene->addItem(guidoItem);

	ui.guidoWidget->setScene(scene);
	ui.guidoWidget->fitInView(guidoItem);*/

	noteMarker = new GuidoNoteMarker(ar);

	//printf("RCa: %d\n", ar->refCount);
	//MarkActiveNotes(ar, 1, 1, 4);
	//noteMarker->setPerformanceMarker(1, 8);

	/*printf("\n=== NEW AST ===\n");
	GuidoPrintAR(ar, std::cout);
	printf("\n");
	fflush(stdout);*/

	ui.guidoWidget->setARHandler(ar);
	//printf("RCb: %d\n", ar->refCount);
	//ar->refCount = 500;


	MidiPerformance* perf = new MidiPerformance;
    perf->addNote("c", 4);
    perf->addNote("d", 4);
    perf->addNote("e", 4);
    perf->addNote("f", 4);
    perf->addNote("g", 4);
    perf->addNote("a", 4);
    perf->addNote("b", 4);
    perf->addNote("c5", 4);
    perf->addNote("d5", 4);
    perf->addNote("e5", 4);
    perf->addNote("f5", 4);
    perf->addNote("g5", 4);
    perf->addNote("a5", 4);
    perf->addNote("b5", 4);

    connect(perf, SIGNAL(currentTickUpdated(int32_t, int32_t)), this, SLOT(currentTickUpdated(int32_t, int32_t)));
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

    perf->start();
}


void SightReadingWidget::currentTickUpdated(int32_t num, int32_t denom)
{
	printf("Tick: %d / %u\n", num, denom);
	fflush(stdout);

	//MarkActiveNotes(ar, 1, num, denom);
	noteMarker->setPerformanceMarker(num, denom);

	/*printf("RefCount a: %d\n", ar->refCount);
	fflush(stdout);*/

	/*if (!ui.guidoWidget->getARHandler())
	{
		ui.guidoWidget->setARHandler(ar);
	}*/

	//ui.guidoWidget->clearCache();
	//ui.guidoWidget->setARHandler(nullptr);

	ar->refCount += 2;
	//printf("  RefCount a: %d\n", ar->refCount);
	ui.guidoWidget->setARHandler(ar);
	//printf("  RefCount b: %d\n", ar->refCount);
	ar->refCount -= 1;

	/*const char* guidoStr = "[ c d e f ]";
	//ui.guidoWidget->setGMNCode(guidoStr, "C:/Users/alemariusnexus/gmn");

	GuidoParser* parser = GuidoOpenParser();
	ar = GuidoString2AR(parser, guidoStr);
	MarkActiveNotes(ar, 1, num, denom);

	ui.guidoWidget->setARHandler(ar);*/

	//printf("b\n");
	/*printf("RefCount b: %d\n", ar->refCount);
	fflush(stdout);*/
}


void SightReadingWidget::onGenerate()
{
	/*const char* notes = "cdefgab";
	QString code("[");

	for (int i = 0 ; i < 4 ; i++)
	{
		char note = notes[rand() % strlen(notes)];
		code.append(" ");
		code.append(note);
	}

	code.append("]");

	ui.guidoWidget->setGMNCode(code, QString());*/

	/*if (rand() % 2)
	{
		ui.guidoWidget->setGMNCode("[ c d \\symbol<file=\"timemark.png\",size=1.5,dx=-1.25>(e) f ]", "C:/Users/alemariusnexus/gmn");
	}
	else
	{
		ui.guidoWidget->setGMNCode("[ c d e f ]", "C:/Users/alemariusnexus/gmn");
	}*/
}
