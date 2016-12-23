ADD_SUBDIRECTORY(guido)

CONFIGURE_FILE(config.cmake.h "${CMAKE_CURRENT_BINARY_DIR}/config.h")

SET(SRCS ${SRCS}
    pianotrain/main.cpp pianotrain/MainWindow.cpp pianotrain/MidiService.cpp pianotrain/SightReadingWidget.cpp pianotrain/MidiPerformance.cpp
    pianotrain/Metronome.cpp
    PARENT_SCOPE
    )
SET(MOC_SRCS ${MOC_SRCS}
    pianotrain/MainWindow.h pianotrain/MidiService.h pianotrain/SightReadingWidget.h pianotrain/MidiPerformance.h
    pianotrain/Metronome.h
    PARENT_SCOPE
    )
SET(UIC_SRCS ${UIC_SRCS}
    pianotrain/MainWindow.ui pianotrain/SightReadingWidget.ui
    PARENT_SCOPE
    )

SET(INCS ${INCS}
    pianotrain
    PARENT_SCOPE
    )
