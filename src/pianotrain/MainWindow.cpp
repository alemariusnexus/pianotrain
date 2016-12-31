#include "MainWindow.h"
#include "System.h"
#include <QtCore/QFile>
#include <QtCore/QByteArray>
#include <QtCore/QDir>
#include <QtCore/QSettings>
#include <QtCore/QTimer>
#include <cstdio>
#include <ctime>
#include <cstdlib>

#include "SettingsDialog.h"
#include "SightReadingExercise.h"
#include <PythonQt.h>



MainWindow::MainWindow(QWidget* parent)
		: QMainWindow(parent), currentExercise(nullptr)
{
	ui.setupUi(this);

	connect(ui.actionQuit, SIGNAL(triggered()), qApp, SLOT(quit()));

	connect(ui.actionSettings, SIGNAL(triggered()), this, SLOT(settingsTriggered()));

	connect(ui.actionAbout, SIGNAL(triggered()), this, SLOT(aboutTriggered()));
	connect(ui.actionAboutQt, SIGNAL(triggered()), qApp, SLOT(aboutQt()));

	connect(ui.exerciseListWidget, SIGNAL(exerciseSelected(Exercise*)), this, SLOT(exerciseChanged(Exercise*)));

	connect(ui.mainSplitter, SIGNAL(splitterMoved(int, int)), this, SLOT(mainSplitterChanged()));

	srand(time(NULL));

	System* sys = System::getInstance();

	PythonQtObjectPtr mainModule = PythonQt::self()->getMainModule();

	mainModule.addObject("mainWindow", this);
	mainModule.addVariable("dataPath", sys->getDataPath());

	QDir exercisesDir(sys->getScriptPath() + "/exercises");

	for (QString scriptName : exercisesDir.entryList(QStringList() << "*.py", QDir::Files | QDir::Readable, QDir::Name))
	{
		QString scriptPath = exercisesDir.filePath(scriptName);

		printf("Running exercise script %s...\n", scriptName.toUtf8().constData());

		mainModule.evalFile(scriptPath);
	}


	QSettings settings;

	restoreGeometry(settings.value("gui/MainWindow/geometry").toByteArray());
	restoreState(settings.value("gui/MainWindow/state").toByteArray());

	ui.mainSplitter->restoreState(settings.value("gui/MainWindow/mainSplitter_state").toByteArray());

	stateSaveTimer = new QTimer(this);
	connect(stateSaveTimer, SIGNAL(timeout()), this, SLOT(saveWindowSettings()));
	stateSaveTimer->start(500);
}


QWidget* MainWindow::findWidgetByName(const QString& name) const
{
	return findChild<QWidget*>(name);
}


void MainWindow::addExercise(Exercise* exercise)
{
	QWidget* mainWidget = exercise->getMainWidget();

	if (mainWidget)
	{
		mainWidget->setParent(ui.mainStackWidget);
		ui.mainStackWidget->addWidget(mainWidget);
	}

	QWidget* controlWidget = exercise->getControlWidget();

	if (controlWidget)
	{
		controlWidget->setParent(ui.exerciseControlStackStackedWidget);
		ui.exerciseControlStackStackedWidget->addWidget(controlWidget);
	}

	ui.exerciseListWidget->addExercise(exercise);

	exercises << exercise;
}


void MainWindow::switchExercise(Exercise* exercise)
{
	if (currentExercise)
	{
		currentExercise->deactivate();
	}

	QWidget* mainWidget = exercise->getMainWidget();
	QWidget* controlWidget = exercise->getControlWidget();

	if (mainWidget)
	{
		ui.mainStackWidget->setCurrentWidget(mainWidget);
	}
	else
	{
		ui.mainStackWidget->setCurrentIndex(0);
	}

	if (controlWidget)
	{
		ui.exerciseControlStackStackedWidget->setCurrentWidget(controlWidget);
	}
	else
	{
		ui.exerciseControlStackStackedWidget->setCurrentIndex(0);
	}

	exercise->activate();

	currentExercise = exercise;
}


void MainWindow::exerciseChanged(Exercise* ex)
{
	switchExercise(ex);

	//printf("Exercise: %s\n", id.toUtf8().constData());
	//fflush(stdout);
}


void MainWindow::mainSplitterChanged()
{
	QSettings settings;

	settings.setValue("gui/MainWindow/mainSplitter_state", ui.mainSplitter->saveState());
}


void MainWindow::saveWindowSettings()
{
	QSettings settings;

	QByteArray geometry = saveGeometry();
	QByteArray state = saveState();

	if (geometry != lastSavedGeometry)
	{
		lastSavedGeometry = geometry;
		settings.setValue("gui/MainWindow/geometry", geometry);
	}
	if (state != lastSavedState)
	{
		lastSavedState = state;
		settings.setValue("gui/MainWindow/state", state);
	}
}


void MainWindow::aboutTriggered()
{
}


void MainWindow::settingsTriggered()
{
	SettingsDialog dlg(this);
	dlg.exec();
}
