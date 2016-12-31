#include "ExerciseListWidget.h"
#include <QBoxLayout>
#include <QLabel>
#include <QFrame>
#include <algorithm>



ExerciseListWidget::ExerciseListWidget(QWidget* parent)
		: QWidget(parent)
{
	ui.setupUi(this);

	connect(ui.listWidget, SIGNAL(itemClicked(QListWidgetItem*)), this, SLOT(exerciseItemChosen(QListWidgetItem*)));
	connect(ui.listWidget, SIGNAL(itemActivated(QListWidgetItem*)), this, SLOT(exerciseItemChosen(QListWidgetItem*)));

	connect(ui.expandCollapseButton, SIGNAL(clicked()), this, SLOT(expandCollapseClicked()));

	ui.expandCollapseButton->hide();

	ui.zoomInButton->setIcon(QIcon::fromTheme("zoom-in", QIcon(":/icons/zoom-in.png")));
	ui.zoomOutButton->setIcon(QIcon::fromTheme("zoom-out", QIcon(":/icons/zoom-out.png")));

	ui.zoomInButton->setText("");
	ui.zoomOutButton->setText("");

	ui.zoomInButton->setIconSize(QSize(22, 22));
	ui.zoomOutButton->setIconSize(QSize(22, 22));

	ui.listWidget->setIconSize(QSize(100, 50));

	QFont font = ui.listWidget->font();
	font.setPointSize(12);
	ui.listWidget->setFont(font);
}


//void ExerciseListWidget::addExercise(const QString& id, const QString& name, const QIcon& icon, const QString& category)
void ExerciseListWidget::addExercise(Exercise* exercise)
{
	QList<Exercise*>& exes = exercises[exercise->getCategory()];
	exes.insert(std::upper_bound(exes.begin(), exes.end(), exercise, [](const Exercise* a, const Exercise* b) {
		return a->getName() < b->getName();
	}), exercise);

	rebuildList();
}


void ExerciseListWidget::rebuildList()
{
	ui.listWidget->clear();

	for (QString category : exercises.keys())
	{
		if (!category.isNull())
		{
			QFrame* categoryWidget = new QFrame(ui.listWidget);

			categoryWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
			categoryWidget->setContentsMargins(0, 0, 0, 0);
			categoryWidget->setFrameShape(QFrame::Box);
			categoryWidget->setFrameShadow(QFrame::Raised);

			QColor spacerBgcolor = ui.listWidget->palette().base().color().darker(110);
			categoryWidget->setStyleSheet(QString("background-color: rgb(%1, %2, %3);")
					.arg(spacerBgcolor.red()).arg(spacerBgcolor.green()).arg(spacerBgcolor.blue()));

			QHBoxLayout* categoryLayout = new QHBoxLayout(categoryWidget);
			categoryLayout->setSizeConstraint(QLayout::SetMinimumSize);
			categoryLayout->setContentsMargins(10, 0, 0, 0);
			categoryWidget->setLayout(categoryLayout);

			QLabel* sectLabel = new QLabel(category, categoryWidget);
			QFont font = sectLabel->font();
			font.setPointSize(12);
			font.setBold(true);
			sectLabel->setFont(font);
			categoryLayout->addWidget(sectLabel);

			QSpacerItem* spacer = new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Preferred);
			categoryLayout->addSpacerItem(spacer);

			QListWidgetItem* spacerItem = new QListWidgetItem(ui.listWidget);
			spacerItem->setFlags(Qt::NoItemFlags);
			ui.listWidget->setItemWidget(spacerItem, categoryWidget);
		}

		for (Exercise* ex : exercises[category])
		{
			QListWidgetItem* item = new QListWidgetItem(ex->getIcon(), ex->getName(), ui.listWidget);
			item->setData(Qt::UserRole, ex->getID());
		}
	}
}


void ExerciseListWidget::exerciseItemChosen(QListWidgetItem* item)
{
	QVariant data = item->data(Qt::UserRole);

	if (data.isNull())
	{
		return;
	}

	QString id = data.toString();
	QString oldID = selectedExerciseID;

	selectedExerciseID = id;

	if (id != oldID)
	{
		for (auto& exes : exercises.values())
		{
			for (Exercise* ex : exes)
			{
				if (ex->getID() == id)
				{
					emit exerciseSelected(ex);
					break;
				}
			}
		}
	}
}


void ExerciseListWidget::expandCollapseClicked()
{
	ui.mainWidget->setVisible(!ui.mainWidget->isVisible());
}
