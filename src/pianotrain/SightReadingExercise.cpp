#include "SightReadingExercise.h"
#include "SightReadingWidget.h"




SightReadingExercise::SightReadingExercise()
{
	sightReadingWidget = new SightReadingWidget;

	connect(sightReadingWidget, SIGNAL(generateRequested()), this, SIGNAL(generateRequested()));
	connect(sightReadingWidget, SIGNAL(performanceStarted()), this, SIGNAL(performanceStarted()));
	connect(sightReadingWidget, SIGNAL(performanceFinished(bool)), this, SIGNAL(performanceFinished(bool)));
}


QWidget* SightReadingExercise::getMainWidget()
{
	return sightReadingWidget;
}


void SightReadingExercise::activate()
{
	Exercise::activate();
}


void SightReadingExercise::deactivate()
{
	Exercise::deactivate();
}

