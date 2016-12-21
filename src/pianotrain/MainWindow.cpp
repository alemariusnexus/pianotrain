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

	//ui.guidoWidget->setGMNCode("[ c/4 d e f \\bar g a b {c,e,g} ]", QString());

	//connect(ui.generateButton, SIGNAL(clicked()), this, SLOT(onGenerate()));

	/*QString gnmFile("E:/source/guidolib-1.60-src/gmn-examples/note/noteFormat.gmn");
	ui.guidoWidget->setGMNFile(gnmFile);*/
}
