#ifndef MAINWINDOW_H_
#define MAINWINDOW_H_

#include <QtGui/QWidget>
#include <ui_MainWindow.h>


class MainWindow : public QWidget
{
	Q_OBJECT

public:
	MainWindow(QWidget* parent = nullptr);

private:
	Ui_MainWindow ui;
};

#endif /* MAINWINDOW_H_ */
