ADD_SUBDIRECTORY(guido)

CONFIGURE_FILE(config.cmake.h "${CMAKE_CURRENT_BINARY_DIR}/config.h")

SET(SRCS ${SRCS}
    pianotrain/main.cpp pianotrain/MainWindow.cpp pianotrain/MidiService.cpp pianotrain/SightReadingWidget.cpp pianotrain/MidiPerformance.cpp
    pianotrain/Metronome.cpp pianotrain/System.cpp pianotrain/DynamicWrapScoreWidget.cpp pianotrain/ScoreWidget.cpp
    pianotrain/ScoreWidgetBase.cpp pianotrain/ExerciseListWidget.cpp pianotrain/Exercise.cpp pianotrain/SightReadingExercise.cpp
    PARENT_SCOPE
    )
SET(MOC_SRCS ${MOC_SRCS}
    pianotrain/MainWindow.h pianotrain/MidiService.h pianotrain/SightReadingWidget.h pianotrain/MidiPerformance.h
    pianotrain/Metronome.h pianotrain/System.h pianotrain/DynamicWrapScoreWidget.h pianotrain/ScoreWidget.h
    pianotrain/ScoreWidgetBase.h pianotrain/ExerciseListWidget.h pianotrain/Exercise.h pianotrain/SightReadingExercise.h
    PARENT_SCOPE
    )
SET(UIC_SRCS ${UIC_SRCS}
    pianotrain/MainWindow.ui pianotrain/SightReadingWidget.ui  pianotrain/ExerciseListWidget.ui pianotrain/DynamicWrapScoreWidget.ui
    PARENT_SCOPE
    )

SET(INCS ${INCS}
    pianotrain
    PARENT_SCOPE
    )
