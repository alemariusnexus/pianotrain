#include "qtluabinding.h"
#include "../MainWindow.h"
#include "../SightReadingWidget.h"
#include <QtLua/State>
#include <QtLua/qtluametatype.hh>
#include <QtLua/qtluametatype.hxx>



#define REGISTER_CUSTOM_QOBJECT(cls) \
	QTLUA_METATYPE_QOBJECT(_QtLuaConverter_ ## cls, cls) \
	{ static _QtLuaConverter_ ## cls _cnv; } \
	QtLua::qtlib_register_meta(&cls::staticMetaObject, &QtLua::create_qobject<cls>);


void RegisterQtLuaMetaObjects()
{
	REGISTER_CUSTOM_QOBJECT(MainWindow)
	REGISTER_CUSTOM_QOBJECT(SightReadingWidget)
}
