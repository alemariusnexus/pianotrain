#include "Exercise.h"




Exercise::Exercise(QObject* parent)
		: QObject(parent)
{
	mainControlWidget = new QWidget;

	mainControlLayout = new QHBoxLayout(mainControlWidget);
	mainControlWidget->setLayout(mainControlLayout);
	mainControlLayout->addStretch(1);
}


QWidget* Exercise::getMainWidget()
{
	return nullptr;
}


QWidget* Exercise::getControlWidget()
{
	return mainControlWidget;
}


void Exercise::activate()
{
	emit activated();
}


void Exercise::deactivate()
{
	emit deactivated();
}


void Exercise::setID(const QString& id)
{
	this->id = id;
}


void Exercise::setName(const QString& name)
{
	this->name = name;
}


void Exercise::setCategory(const QString& category)
{
	this->category = category;
}


void Exercise::setIcon(const QIcon& icon)
{
	this->icon = icon;
}


void Exercise::addControlWidget(QWidget* widget)
{
	widget->setParent(mainControlWidget);
	mainControlLayout->insertWidget(mainControlLayout->count()-1, widget);
}
