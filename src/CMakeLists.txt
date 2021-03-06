# Copyright 2010-2014 David "Alemarius Nexus" Lerch
# 
# This file is part of pianotrain.
#
# pianotrain is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
# 
# pianotrain is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# 
# You should have received a copy of the GNU General Public License
# along with pianotrain.  If not, see <http://www.gnu.org/licenses/>.



CMAKE_MINIMUM_REQUIRED(VERSION 2.8)
PROJECT(pianotrain C CXX)

CMAKE_POLICY(SET CMP0020 OLD)
CMAKE_POLICY(SET CMP0028 NEW)

SET(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR})

FIND_PATH(GUIDO_INCLUDE_DIRS GUIDOEngine.h PATH_SUFFIXES Guido)
FIND_PATH(GUIDO_MISC_INCLUDE_DIRS GUIDOTypes.h PATH_SUFFIXES Guido/misc)
FIND_PATH(GUIDO_LIB_INCLUDE_DIRS GUIDOInternal.h PATH_SUFFIXES Guido/lib)
FIND_PATH(GUIDO_ABSTRACT_INCLUDE_DIRS ARNote.h PATH_SUFFIXES Guido/abstract)
FIND_PATH(GUIDO_TOOLS_INCLUDE_DIRS PrintVisitor.h PATH_SUFFIXES Guido/tools)
FIND_PATH(GUIDO_PARSER_INCLUDE_DIRS GuidoParser.h PATH_SUFFIXES Guido/parser)
FIND_PATH(GUIDO_GRAPHIC_INCLUDE_DIRS GRNote.h PATH_SUFFIXES Guido/graphic)
FIND_LIBRARY(GUIDO_LIBRARIES NAMES GUIDOEngine libGUIDOEngine GUIDOEngineD libGUIDOEngineD GUIDOEngine.dll libGUIDOEngine.dll GUIDOEngineD.dll libGUIDOEngineD.dll)

FIND_PATH(GUIDOQT_INCLUDE_DIRS QGuidoWidget.h PATH_SUFFIXES GuidoQt QtGuido)
FIND_LIBRARY(GUIDOQT_LIBRARIES NAMES GuidoQt libGuidoQt)

FIND_PATH(PORTMIDI_INCLUDE_DIRS portmidi.h PATH_SUFFIXES portmidi)
FIND_LIBRARY(PORTMIDI_LIBRARIES NAMES portmidi libportmidi portmidi_s libportmidi_s)

FIND_PATH(PYTHONQT_INCLUDE_DIRS PythonQt.h PATH_SUFFIXES PythonQt)
FIND_LIBRARY(PYTHONQT_LIBRARIES NAMES PythonQt pythonqt PythonQt_d pythonqt_d)
FIND_LIBRARY(PYTHONQTALL_LIBRARIES NAMES PythonQt_QtAll pythonqt_qtall PythonQt_QtAll_d pythonqt_qtall_d)

#FIND_PATH(SFML_SYSTEM_INCLUDE_DIRS SFML/System.hpp)
#FIND_LIBRARY(SFML_SYSTEM_LIBRARIES NAMES sfml-system libsfml-system sfml-system-d libsfml-system-d)

#FIND_PATH(SFML_AUDIO_INCLUDE_DIRS SFML/Audio.hpp)
#FIND_LIBRARY(SFML_AUDIO_LIBRARIES NAMES sfml-audio libsfml-audio sfml-audio-d libsfml-audio-d)

FIND_PACKAGE(Nxcommon REQUIRED CONFIG)

FIND_PACKAGE(PythonLibs REQUIRED)

IF(NXCOMMON_QT_SUPPORT STREQUAL "qt4")
    #MESSAGE(FATAL_ERROR "nxcommon was build for Qt4, but pianotrain requires Qt5!")

    IF(WIN32 AND NOT MINGW)
        FIND_PACKAGE(Qt4 4.8.0 COMPONENTS QtCore QtGui qtmain REQUIRED)
    ELSE()
        FIND_PACKAGE(Qt4 4.8.0 COMPONENTS QtCore QtGui REQUIRED)
    ENDIF()
    
    INCLUDE(${QT_USE_FILE})
ELSEIF(NXCOMMON_QT_SUPPORT STREQUAL "qt5")
    FIND_PACKAGE(Qt5Core)
    FIND_PACKAGE(Qt5Widgets)
    FIND_PACKAGE(Qt5Multimedia)
    FIND_PACKAGE(Qt5PrintSupport) # Needed by GuidoQt
    FIND_PACKAGE(Qt5UiTools)
    #FIND_PACKAGE(Qt5LinguistTools)
ENDIF(NXCOMMON_QT_SUPPORT STREQUAL "qt4")

#FIND_PATH(LUA_INCLUDE_DIRS lua.h)
#FIND_LIBRARY(LUA_LIBRARIES NAMES luajit libluajit)

#FIND_PATH(LIBQTLUA_INCLUDE_DIRS QtLua/State)
#FIND_LIBRARY(LIBQTLUA_LIBRARIES NAMES qtlua libqtlua)



SET(LIBRARIES "")
SET(INCLUDES "${CMAKE_CURRENT_BINARY_DIR}" "${CMAKE_CURRENT_SOURCE_DIR}")

SET(LIBRARIES ${LIBRARIES} ${GUIDOQT_LIBRARIES})
SET(INCLUDES ${INCLUDES} "${GUIDOQT_INCLUDE_DIRS}")

SET(LIBRARIES ${LIBRARIES} ${GUIDO_LIBRARIES})
SET(INCLUDES ${INCLUDES} "${GUIDO_INCLUDE_DIRS}" "${GUIDO_MISC_INCLUDE_DIRS}" "${GUIDO_LIB_INCLUDE_DIRS}" "${GUIDO_ABSTRACT_INCLUDE_DIRS}"
        "${GUIDO_TOOLS_INCLUDE_DIRS}" "${GUIDO_PARSER_INCLUDE_DIRS}" "${GUIDO_GRAPHIC_INCLUDE_DIRS}")

SET(LIBRARIES ${LIBRARIES} ${PORTMIDI_LIBRARIES})
SET(INCLUDES ${INCLUDES} "${PORTMIDI_INCLUDE_DIRS}")

#SET(LIBRARIES ${LIBRARIES} ${LUA_LIBRARIES})
#SET(INCLUDES ${INCLUDES} "${LUA_INCLUDE_DIRS}")

IF(NXCOMMON_QT_SUPPORT STREQUAL "qt4")
    SET(LIBRARIES ${LIBRARIES} ${QT_LIBRARIES})
ELSE()
    SET(LIBRARIES ${LIBRARIES} Qt5::Core Qt5::Widgets Qt5::Multimedia Qt5::PrintSupport Qt5::UiTools)
    
    IF(WIN32 AND NOT MINGW)
        SET(LIBRARIES ${LIBRARIES} Qt5::WinMain)
    ENDIF()
ENDIF()

#SET(LIBRARIES ${LIBRARIES} ${LIBQTLUA_LIBRARIES})
#SET(INCLUDES ${INCLUDES} "${LIBQTLUA_INCLUDE_DIRS}")

SET(LIBRARIES ${LIBRARIES} ${PYTHONQT_LIBRARIES} ${PYTHONQTALL_LIBRARIES})
SET(INCLUDES ${INCLUDES} "${PYTHONQT_INCLUDE_DIRS}")

SET(LIBRARIES ${LIBRARIES} ${PYTHON_LIBRARIES})
SET(INCLUDES ${INCLUDES} "${PYTHON_INCLUDE_DIRS}")

#SET(LIBRARIES ${LIBRARIES} ${SFML_SYSTEM_LIBRARIES} ${SFML_AUDIO_LIBRARIES})
#SET(INCLUDES ${INCLUDES} "${SFML_SYSTEM_INCLUDE_DIRS}" "${SFML_AUDIO_INCLUDE_DIRS}")

IF(WIN32)
    # Needed by PortMidi and for multimedia timers in libnxcommon
    SET(LIBRARIES ${LIBRARIES} winmm)
ENDIF(WIN32)

SET(LIBRARIES ${LIBRARIES} nxcommon)


#INCLUDE(${QT_USE_FILE})
#INCLUDE(GetGitRevisionDescription)

#GET_GIT_HEAD_REVISION(GIT_REFSPEC GIT_SHA1)

#CONFIGURE_FILE(edb_config.cmake.h "${CMAKE_CURRENT_BINARY_DIR}/edb_config.h")


SET(SRCS "")
SET(MOC_SRCS "")
SET(UIC_SRCS "")
SET(INCS "")

ADD_SUBDIRECTORY(pianotrain)


#SET_SOURCE_FILES_PROPERTIES(electronics_en.ts electronics_de.ts PROPERTIES
#    OUTPUT_LOCATION "${CMAKE_SOURCE_DIR}")

IF(NXCOMMON_QT_SUPPORT STREQUAL "qt4")
    QT4_WRAP_CPP(MOC_COMPILED_SRCS ${MOC_SRCS})
    QT4_WRAP_UI(UIC_COMPILED_SRCS ${UIC_SRCS})
    #QT4_ADD_TRANSLATION(QM_FILES gtatools-gui_en.ts gtatools-gui_de.ts)
    
    QT4_ADD_RESOURCES(QT_RESOURCE_FILES resources.qrc)
ELSE()
    QT5_WRAP_CPP(MOC_COMPILED_SRCS ${MOC_SRCS})
    QT5_WRAP_UI(UIC_COMPILED_SRCS ${UIC_SRCS})
    #QT5_ADD_TRANSLATION(QM_FILES gtatools-gui_en.ts gtatools-gui_de.ts)
    
    QT5_ADD_RESOURCES(QT_RESOURCE_FILES resources.qrc)
ENDIF()

SET(INCLUDES ${INCLUDES} ${INCS})

INCLUDE_DIRECTORIES(${INCLUDES})

ADD_COMPILE_OPTIONS(--std=c++11)
ADD_EXECUTABLE(pianotrain ${SRCS} ${MOC_COMPILED_SRCS} ${UIC_COMPILED_SRCS} ${QM_FILES} ${QT_RESOURCE_FILES})

TARGET_LINK_LIBRARIES(pianotrain ${LIBRARIES})
