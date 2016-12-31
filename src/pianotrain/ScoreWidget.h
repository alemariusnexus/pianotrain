#ifndef PIANOTRAIN_SCOREWIDGET_H_
#define PIANOTRAIN_SCOREWIDGET_H_

#include <pianotrain/config.h>
#include <QtCore/QList>
#include <QWidget>
#include <QColor>
#include <Guido/GUIDOEngine.h>
#include "guido/ExtendedGuidoWidget.h"
#include "ScoreWidgetBase.h"


class ScoreWidget : public QWidget, public ScoreWidgetBase
{
	Q_OBJECT

public:
	ScoreWidget(QWidget* parent = nullptr);
};

#endif /* PIANOTRAIN_SCOREWIDGET_H_ */
