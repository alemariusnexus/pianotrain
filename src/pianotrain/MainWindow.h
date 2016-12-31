#ifndef MAINWINDOW_H_
#define MAINWINDOW_H_

#include <QWidget>
#include <QtCore/QList>
#include <QtCore/QTimer>
#include <ui_MainWindow.h>
#include "Exercise.h"

#include <cstdio>


class MainWindow : public QWidget
{
	Q_OBJECT

public:
	Q_INVOKABLE MainWindow(QWidget* parent = nullptr);

	Q_INVOKABLE void addExercise(Exercise* exercise);

	Q_INVOKABLE void switchExercise(Exercise* exercise);

public:
	Q_INVOKABLE QWidget* findWidgetByName(const QString& name) const;

private slots:
	void exerciseChanged(Exercise* ex);

	void mainSplitterChanged();

	void saveWindowSettings();

private:
	Ui_MainWindow ui;
	QList<Exercise*> exercises;
	Exercise* currentExercise;

	QTimer* stateSaveTimer;
	QByteArray lastSavedGeometry;
	QByteArray lastSavedState;
};


class MainWindowPythonWrapper : public QObject
{
	Q_OBJECT

public:
	Q_INVOKABLE MainWindow* new_MainWindow(QWidget* parent = nullptr) { return new MainWindow(parent); }
	Q_INVOKABLE void delete_MainWindow(MainWindow* win) { delete win; }
};

Q_DECLARE_METATYPE(MainWindow*);

#endif /* MAINWINDOW_H_ */
