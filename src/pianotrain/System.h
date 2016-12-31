#ifndef SYSTEM_H_
#define SYSTEM_H_

#include <QtCore/QObject>
#include <QUiLoader>



class System : public QObject
{
	Q_OBJECT

public:
	static System* getInstance();

public:
	void setDataPath(const QString& path);

	void setupScripting();

	QString getDataPath() const;
	QString getScriptPath() const;

	void addPythonSystemPath(const QString& path);

	Q_INVOKABLE QWidget* loadUiFile(const QString& filePath, QWidget* parent = nullptr);

private:
	System();

private:
	QList<QString> pythonSysPaths;
	QString dataPath;
};

#endif /* SYSTEM_H_ */
