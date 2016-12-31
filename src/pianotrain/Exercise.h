#ifndef PIANOTRAIN_EXERCISE_H_
#define PIANOTRAIN_EXERCISE_H_

#include <pianotrain/config.h>
#include <QtCore/QObject>
#include <QtCore/QList>
#include <QWidget>
#include <QBoxLayout>
#include <QIcon>



class Exercise : public QObject
{
	Q_OBJECT

public:
	Exercise(QObject* parent = nullptr);

	virtual Q_INVOKABLE QWidget* getMainWidget();
	virtual Q_INVOKABLE QWidget* getControlWidget();
	virtual Q_INVOKABLE void activate();
	virtual Q_INVOKABLE void deactivate();

	Q_INVOKABLE void setID(const QString& id);
	Q_INVOKABLE void setName(const QString& name);
	Q_INVOKABLE void setCategory(const QString& category);
	Q_INVOKABLE void setIcon(const QIcon& icon);

	Q_INVOKABLE QString getID() const { return id; }
	Q_INVOKABLE QString getName() const { return name; }
	Q_INVOKABLE QString getCategory() const { return category; }
	Q_INVOKABLE QIcon getIcon() const { return icon; }

	Q_INVOKABLE void addControlWidget(QWidget* widget);

signals:
	void activated();
	void deactivated();

private:
	QString id;
	QString name;
	QString category;
	QIcon icon;

	QWidget* mainControlWidget;
	QHBoxLayout* mainControlLayout;
};

#endif /* PIANOTRAIN_EXERCISE_H_ */
