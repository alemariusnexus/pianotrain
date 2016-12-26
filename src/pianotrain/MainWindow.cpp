#include "MainWindow.h"
#include <QtCore/QFile>
#include <QtCore/QByteArray>
#include <cstdio>
#include <ctime>
#include <cstdlib>



MainWindow::MainWindow(QWidget* parent)
		: QWidget(parent)
{
	ui.setupUi(this);

	srand(time(NULL));
}


SightReadingWidget* MainWindow::getSightReadingWidget()
{
	return ui.sightReadingWidget;
}
