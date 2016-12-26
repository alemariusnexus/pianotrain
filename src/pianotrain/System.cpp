#include "System.h"
#include "qtlua/qtluabinding.h"
#include "SightReadingWidget.h"
#include <nxcommon/script/luasys.h>
#include <cstdio>

#include <QtLua/qtluaqmetaobjectwrapper.hh>
#include <QAbstractItemDelegate>



struct blastr
{
	const QMetaObject *_mo;
};


const blastr blastrs[] = {
		{ &QWidget::staticMetaObject }
};



System* System::getInstance()
{
	static System inst;
	return &inst;
}


System::System()
{
}


void System::setDataPath(const QString& path)
{
	dataPath = path;
}


QString System::getScriptPath() const
{
	return dataPath + "/scripts";
}


void System::setupLua()
{
	RegisterQtLuaMetaObjects();

	qtLua = new QtLua::State;
	lua = qtLua->get_lua_state();

	connect(qtLua, SIGNAL(output(const QString&)), this, SLOT(luaOutput(const QString&)));

	luaL_openlibs(lua);
	luaS_opencorelibs(lua);
	qtLua->openlib(QtLua::QtLib);
	qtLua->openlib(QtLua::QtLuaLib);

	if (luaL_dostring(lua, "if qt.meta.QWidget then return true else return false end") != LUA_OK)
	{
		fprintf(stderr, "ERROR: Error running QtLua sanity check script: %s\n", lua_tostring(lua, -1));
		fflush(stderr);
		lua_close(lua);
		lua = NULL;
		exit(1);
	}

	bool qtLuaSanityCheck = lua_toboolean(lua, -1);
	lua_pop(lua, 1);

	if (!qtLuaSanityCheck)
	{
		// NOTE: If this fails, it might be due to the global initialization order error in QtLua (qt_meta and meta_object_table
		// variables not initialized in correct order). Refer to https://savannah.nongnu.org/bugs/index.php?49931
		fprintf(stderr, "ERROR: QtLua sanity check failed! qt.meta.QWidget is not set in Lua. Did QtLua load correctly?\n");
		fflush(stderr);
		lua_close(lua);
		lua = NULL;

		fprintf(stderr, "Haeh\n");
		fflush(stderr);
		exit(1);
	}

	if (luaL_loadfile(lua, getScriptPath().append("/setup.lua").toUtf8().constData()) != LUA_OK) {
		fprintf(stderr, "ERROR: Error loading setup.lua: %s\n", lua_tostring(lua, -1));
		fflush(stderr);
		lua_close(lua);
		lua = NULL;
		exit(1);
	}

	if (lua_pcall(lua, 0, LUA_MULTRET, 0) != LUA_OK) {
		fprintf(stderr, "ERROR: Error running setup.lua: %s\n", lua_tostring(lua, -1));
		fflush(stderr);
		lua_close(lua);
		lua = NULL;
		exit(1);
	}

	fflush(stdout);
}


void System::luaOutput(const QString& str)
{
	printf("%s", str.toUtf8().constData());
	fflush(stdout);
}
