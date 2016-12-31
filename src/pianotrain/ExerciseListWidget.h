#ifndef PIANOTRAIN_EXERCISELISTWIDGET_H_
#define PIANOTRAIN_EXERCISELISTWIDGET_H_

#include <pianotrain/config.h>
#include "Exercise.h"
#include <QWidget>
#include <QString>
#include <QIcon>
#include <QList>
#include <QMap>
#include <ui_ExerciseListWidget.h>

class ExerciseListWidget : public QWidget
{
	Q_OBJECT

private:
	/*struct Exercise
	{
		QString id;
		QString name;
		QIcon icon;
		QString category;
	};*/

public:
	ExerciseListWidget(QWidget* parent = nullptr);

	//void addExercise(const QString& id, const QString& name, const QIcon& icon = QIcon(), const QString& category = QString());
	void addExercise(Exercise* exercise);

signals:
	void exerciseSelected(Exercise* exercise);

private slots:
	void exerciseItemChosen(QListWidgetItem* item);
	void expandCollapseClicked();

private:
	void rebuildList();

private:
	Ui_ExerciseListWidget ui;

	QMap<QString, QList<Exercise*> > exercises;
	QString selectedExerciseID;
};

#endif /* PIANOTRAIN_EXERCISELISTWIDGET_H_ */
