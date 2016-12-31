#ifndef PIANOTRAIN_SETTINGSDIALOG_H_
#define PIANOTRAIN_SETTINGSDIALOG_H_

#include <QDialog>
#include <QTimer>
#include <QtCore/QByteArray>
#include <ui_SettingsDialog.h>



class SettingsDialog : public QDialog
{
	Q_OBJECT

public:
	SettingsDialog(QWidget* parent = nullptr);

private slots:
	void metronomeVolumeSliderValueChanged(int value);

	void saveWindowSettings();

private:
	Ui_SettingsDialog ui;

	QTimer* stateSaveTimer;
	QByteArray lastSavedGeometry;
};

#endif /* PIANOTRAIN_SETTINGSDIALOG_H_ */
