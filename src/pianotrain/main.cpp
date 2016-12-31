#include <pianotrain/config.h>

#include <QApplication>
#include <QWidget>
#include <QFontDatabase>
#include <QLibrary>

#include <GuidoQt/QGuidoWidget.h>
#include <GuidoQt/QGuidoPainter.h>

#include <portmidi.h>

#include <nxcommon/util.h>
#include <nxcommon/CString.h>

#include "System.h"
#include "MainWindow.h"
#include "MidiService.h"
#include "MidiPerformance.h"
#include "SightReadingWidget.h"

#include <PythonQt.h>
#include <PythonQt_QtAll.h>

#include <QFile>
#include <QFileInfo>
#include <QLayout>
#include <QGroupBox>
#include <QPushButton>
#include <QLineEdit>
#include <QComboBox>
#include <QCommandLineParser>
#include <QCommandLineOption>
#include <QSettings>




int main(int argc, char** argv)
{
	// Disable stdout/stderr buffering on Windows that prevents text from appearing unless fflush() is called.
	// Whose fucking idea was that, anyway?
	setbuf(stdout, nullptr);
	setbuf(stderr, nullptr);

	QApplication app(argc, argv);

	QApplication::setOrganizationName("alemariusnexus");
	QApplication::setApplicationName("pianotrain");

    qRegisterMetaType<uint8_t>("uint8_t");
    qRegisterMetaType<uint16_t>("uint16_t");
    qRegisterMetaType<uint32_t>("uint32_t");
    qRegisterMetaType<uint64_t>("uint64_t");

    qRegisterMetaType<int8_t>("int8_t");
    qRegisterMetaType<int16_t>("int16_t");
    qRegisterMetaType<int32_t>("int32_t");
    qRegisterMetaType<int64_t>("int64_t");

    qRegisterMetaType<QWidget*>("QWidget*");

    QCommandLineParser parser;
    parser.addHelpOption();
    parser.addVersionOption();

    QCommandLineOption dataPathOption(QStringList() << "d" << "data",
    		"Path to the data directory", "directory");
    parser.addOption(dataPathOption);

    QCommandLineOption pythonPathOption(QStringList() << "p" << "pythonpath",
    		"Add a directory to the Python system path", "directory");
    parser.addOption(pythonPathOption);

    parser.process(app);


    System* system = System::getInstance();

    if (parser.isSet(dataPathOption))
    {
    	system->setDataPath(parser.value(dataPathOption));
    }
    else
    {
    	if (QFile::exists("./data"))
    	{
    		system->setDataPath("./data");
    	}
    	else if (QFile::exists(QApplication::applicationDirPath() + "/data"))
    	{
    		system->setDataPath(QApplication::applicationDirPath() + "/data");
    	}
    	else
    	{
    		system->setDataPath("./data");
    	}
    }

    if (!QFile::exists(system->getScriptPath() + "/setup.py"))
    {
    	fprintf(stderr, "ERROR: Invalid data directory: %s\n", system->getDataPath().toUtf8().constData());
    	return 1;
    }

    QSettings::setDefaultFormat(QSettings::IniFormat);

    for (QString path : parser.values(pythonPathOption))
    {
    	system->addPythonSystemPath(path);
    }

    QFileInfo dataDirInfo(system->getDataPath());
    printf("Using data directory: %s\n", dataDirInfo.absoluteFilePath().toUtf8().constData());

    QFontDatabase::addApplicationFont(system->getDataPath() + "/fonts/guido2.ttf");
    
    QGuidoPainter::startGuidoEngine();

    MidiService* midi = MidiService::getInstance();

    midi->startup();

    midi->openInput("Digital Piano");


    int deviceId = -1;

    printf("=== AVAILABLE MIDI DEVICES ===\n");
    for (int i = 0 ; i < Pm_CountDevices() ; i++)
    {
    	const PmDeviceInfo* info = Pm_GetDeviceInfo(i);

    	if (info->input)
    	{
    		printf("Device %d (input): %s\n", i, info->name);
    	}
    	else
    	{
    		printf("Device %d (non-input): %s\n", i, info->name);
    	}
    }

    system->setupScripting();

    int status = app.exec();
    
    QGuidoPainter::stopGuidoEngine();
    
    return status;
}
