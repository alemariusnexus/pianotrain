#ifndef MAINWINDOW_H_
#define MAINWINDOW_H_

#include <QWidget>
#include <ui_MainWindow.h>
#include "SightReadingWidget.h"


class MainWindow : public QWidget
{
	Q_OBJECT

public:
	MainWindow(QWidget* parent = nullptr);

public:
	Q_INVOKABLE SightReadingWidget* getSightReadingWidget();

private:
	Ui_MainWindow ui;
};

Q_DECLARE_METATYPE(MainWindow*);

#endif /* MAINWINDOW_H_ */
