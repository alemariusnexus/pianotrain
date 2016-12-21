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

SET(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR})

IF(WIN32 AND NOT MINGW)
	FIND_PACKAGE(Qt4 4.8.0 COMPONENTS QtCore QtGui qtmain REQUIRED)
ELSE(WIN32 AND NOT MINGW)
	FIND_PACKAGE(Qt4 4.8.0 COMPONENTS QtCore QtGui REQUIRED)
ENDIF(WIN32 AND NOT MINGW)

IF(WIN32)
    SET(ADDITIONAL_SYSTEM_LIBRARIES winmm)
ENDIF(WIN32)

FIND_PATH(GUIDO_INCLUDE_DIRS GUIDOEngine.h PATH_SUFFIXES Guido)
FIND_LIBRARY(GUIDO_LIBRARIES NAMES GUIDOEngine libGUIDOEngine GUIDOEngineD libGUIDOEngineD GUIDOEngine.dll libGUIDOEngine.dll GUIDOEngineD.dll libGUIDOEngineD.dll)

FIND_PATH(GUIDOQT_INCLUDE_DIRS QGuidoWidget.h PATH_SUFFIXES GuidoQt QtGuido)
FIND_LIBRARY(GUIDOQT_LIBRARIES NAMES GuidoQt libGuidoQt)

FIND_PATH(PORTMIDI_INCLUDE_DIRS portmidi.h PATH_SUFFIXES portmidi)
FIND_LIBRARY(PORTMIDI_LIBRARIES NAMES portmidi libportmidi portmidi_s libportmidi_s)

FIND_PACKAGE(Nxcommon REQUIRED CONFIG)

INCLUDE(${QT_USE_FILE})
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

QT4_WRAP_CPP(MOC_COMPILED_SRCS ${MOC_SRCS})
QT4_WRAP_UI(UIC_COMPILED_SRCS ${UIC_SRCS})
#QT4_ADD_TRANSLATION(QM_FILES electronics_en.ts electronics_de.ts)

QT4_ADD_RESOURCES(QT_RESOURCE_FILES resources.qrc)

FOREACH(VAR ${LIBNXCOMMON_INCLUDE_DIRS})
    MESSAGE("${VAR}")
ENDFOREACH(VAR)

INCLUDE_DIRECTORIES(${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR} ${GUIDO_INCLUDE_DIRS} ${GUIDOQT_INCLUDE_DIRS} ${PORTMIDI_INCLUDE_DIRS} ${INCS})

ADD_COMPILE_OPTIONS(--std=c++11)
ADD_EXECUTABLE(pianotrain ${SRCS} ${MOC_COMPILED_SRCS} ${UIC_COMPILED_SRCS} ${QM_FILES} ${QT_RESOURCE_FILES})
TARGET_LINK_LIBRARIES(pianotrain ${GUIDOQT_LIBRARIES} ${GUIDO_LIBRARIES} ${QT_LIBRARIES} ${PORTMIDI_LIBRARIES} ${ADDITIONAL_SYSTEM_LIBRARIES} nxcommon)
