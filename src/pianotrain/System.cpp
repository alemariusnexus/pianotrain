#include "System.h"
#include "SightReadingWidget.h"
#include "SightReadingExercise.h"
#include <cstdio>
#include <QFile>

#include <QAbstractItemDelegate>

#include <PythonQt.h>
#include <PythonQt_QtAll.h>

#include "MainWindow.h"
#include "Metronome.h"
#include "ScoreWidgetBase.h"
#include "DynamicWrapScoreWidget.h"




System* System::getInstance()
{
	static System inst;
	return &inst;
}


System::System()
{
}


void System::setDataPath(const QString& path)
{
	dataPath = path;
}


QString System::getScriptPath() const
{
	return dataPath + "/scripts";
}


QString System::getDataPath() const
{
	return dataPath;
}


QWidget* System::loadUiFile(const QString& filePath, QWidget* parent)
{
	QUiLoader loader;

	QFile file(filePath);
	if (!file.open(QFile::ReadOnly))
	{
		return nullptr;
	}

	QWidget* widget = loader.load(&file, parent);

	file.close();

	return widget;
}


void System::addPythonSystemPath(const QString& path)
{
	pythonSysPaths << path;
}


void System::setupScripting()
{
	PythonQt::init(PythonQt::IgnoreSiteModule);
	PythonQt_QtAll::init();

	for (QString path : pythonSysPaths)
	{
		PythonQt::self()->addSysPath(path);
	}

	PythonQt::self()->addSysPath(getScriptPath());

	PythonQtObjectPtr mainModule = PythonQt::self()->getMainModule();

	PythonQt::self()->registerClass(&MainWindow::staticMetaObject, "QtGui", PythonQtCreateObject<MainWindowPythonWrapper>);
	PythonQt::self()->registerClass(&SightReadingExercise::staticMetaObject, "QtGui", PythonQtCreateObject<SightReadingExercisePythonWrapper>);
	PythonQt::self()->registerClass(&Metronome::staticMetaObject, "QtGui");
	PythonQt::self()->registerClass(&ScoreWidgetEnums::staticMetaObject, "QtGui");
	PythonQt::self()->registerClass(&DynamicWrapScoreWidget::staticMetaObject, "QtGui");

	mainModule.addObject("system", this);

	mainModule.evalFile(System::getInstance()->getScriptPath() + "/setup.py");
}
