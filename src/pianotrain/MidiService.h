#ifndef MIDISERVICE_H_
#define MIDISERVICE_H_

#include <pianotrain/config.h>

#include <QtCore/QObject>
#include <QtCore/QList>

#include <nxcommon/CString.h>

#include <portmidi.h>

#include <thread>



CString ConvertMidiKeyToString(int32_t key, int32_t octaveOffset = -1);
int32_t ConvertStringToMidiKey(const CString& str, int32_t octaveOffset = -1);


class MidiService : public QObject
{
	Q_OBJECT

public:
	struct MidiDevice
	{
		int id;
		CString name;
		bool isInput;
		bool isOutput;
	};

public:
	static MidiService* getInstance();

public:
	void startup();

	QList<MidiDevice> listDevices() const;

	void openInput(int deviceID);

	bool openInput(const CString& name);

	uint64_t getCurrentTimestamp() const;

signals:
	void messageReceived(uint8_t status, uint8_t data1, uint8_t data2, uint64_t timestamp);

private:
	MidiService();

	void pollThreadMain();
	void reportMessage(uint32_t status, uint32_t data1, uint32_t data2, uint64_t timestamp);

private:
	PmStream* midi;
	std::thread pollThread;
	bool stopPollThread;
};

#endif /* MIDISERVICE_H_ */
