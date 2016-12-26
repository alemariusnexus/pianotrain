#ifndef SYSTEM_H_
#define SYSTEM_H_

#include <QtCore/QObject>
#include <QtLua/State>

extern "C" {

#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>

}



class System : public QObject
{
	Q_OBJECT

public:
	static System* getInstance();

public:
	void setDataPath(const QString& path);
	void setupLua();

private:
	System();

	QString getScriptPath() const;

private slots:
	void luaOutput(const QString& str);

private:
	lua_State* lua;
	QtLua::State* qtLua;
	QString dataPath;
};

#endif /* SYSTEM_H_ */
