#ifndef PIANOTRAIN_SIGHTREADINGEXERCISE_H_
#define PIANOTRAIN_SIGHTREADINGEXERCISE_H_

#include "Exercise.h"
#include "SightReadingWidget.h"



class SightReadingExercise : public Exercise
{
	Q_OBJECT

public:
	SightReadingExercise();

	virtual QWidget* getMainWidget();
	virtual void activate();
	virtual void deactivate();

signals:
	void generateRequested();
	void performanceStarted();
	void performanceFinished(bool stopped);

private:
	SightReadingWidget* sightReadingWidget;
};


class SightReadingExercisePythonWrapper : public QObject
{
	Q_OBJECT

public:
	Q_INVOKABLE SightReadingExercise* new_SightReadingExercise() { return new SightReadingExercise; }
	Q_INVOKABLE void delete_SightReadingExercise(SightReadingExercise* ex) { delete ex; }
};

#endif /* PIANOTRAIN_SIGHTREADINGEXERCISE_H_ */
