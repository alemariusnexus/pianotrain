#include "SettingsDialog.h"
#include <QSettings>



SettingsDialog::SettingsDialog(QWidget* parent)
		: QDialog(parent)
{
	ui.setupUi(this);

	connect(ui.buttonBox, SIGNAL(accepted()), this, SLOT(accept()));

	connect(ui.metronomeVolumeSlider, SIGNAL(valueChanged(int)), this, SLOT(metronomeVolumeSliderValueChanged(int)));

	QSettings settings;

	float metronomeVolMul = settings.value("audio/metronome/volume_multiplier", 1.0f).toFloat();
	ui.metronomeVolumeSlider->setValue(metronomeVolMul * ui.metronomeVolumeSlider->maximum());

	restoreGeometry(settings.value("gui/SettingsDialog/geometry").toByteArray());

	stateSaveTimer = new QTimer(this);
	connect(stateSaveTimer, SIGNAL(timeout()), this, SLOT(saveWindowSettings()));
	stateSaveTimer->start(500);
}


void SettingsDialog::metronomeVolumeSliderValueChanged(int value)
{
	QSettings settings;

	float volume = ui.metronomeVolumeSlider->value() / (float) ui.metronomeVolumeSlider->maximum();

	settings.setValue("audio/metronome/volume_multiplier", volume);
}


void SettingsDialog::saveWindowSettings()
{
	QSettings settings;

	QByteArray geometry = saveGeometry();

	if (geometry != lastSavedGeometry)
	{
		lastSavedGeometry = geometry;
		settings.setValue("gui/SettingsDialog/geometry", geometry);
	}
}
