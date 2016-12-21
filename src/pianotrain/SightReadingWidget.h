#ifndef SIGHTREADINGWIDGET_H_
#define SIGHTREADINGWIDGET_H_

#include <QtGui/QWidget>
#include <Guido/GUIDOParse.h>
#include <Guido/GUIDOEngine.h>
#include <ui_SightReadingWidget.h>
#include "guido/GuidoNoteMarker.h"


class SightReadingWidget : public QWidget
{
	Q_OBJECT

public:
	SightReadingWidget(QWidget* parent = nullptr);

private slots:
	void onGenerate();
	void currentTickUpdated(int32_t num, int32_t denom);

private:
	Ui_SightReadingWidget ui;
	ARHandler ar;
	GuidoNoteMarker* noteMarker;
};

#endif /* SIGHTREADINGWIDGET_H_ */
