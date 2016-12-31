from PythonQt.QtGui import *
from PythonQt.QtCore import *
import random
from lib.util import *

class SimpleSightReadingExercise:
    
    def __init__(self, id, name, category, iconpath = None):
        self.ex = SightReadingExercise()
        self.ex.setID(id)
        self.ex.setName(name)
        self.ex.setCategory(category)
        
        if iconpath != None:
            self.ex.setIcon(QIcon(iconpath))
        
        settings = QSettings()
        
        self.ex.connect("activated()", self.onActivated)
        self.ex.connect("generateRequested()", self.onGenerateRequested)
        
        cw = QGroupBox("Sight Reading Controls")
        cwl = QFormLayout(cw)
        cwl.setHorizontalSpacing(15)
        cw.setLayout(cwl)
        
        self.markerModeBox = QComboBox(cw)
        self.markerModeBox.addItem("Disabled", ScoreWidgetEnums.MarkNone)
        self.markerModeBox.addItem("Mark Notes", ScoreWidgetEnums.MarkAllSimultaneous)
        self.markerModeBox.addItem("Mark Measures", ScoreWidgetEnums.MarkMeasure)
        self.markerModeBox.connect("activated(int)", self.onMarkerModeBoxActivated)
        self.markerModeBox.setCurrentIndex(self.markerModeBox.findData(settings.value("exercise/" + self.ex.getID() + "/marker_mode", ScoreWidgetEnums.MarkAllSimultaneous)))
        cwl.addRow("Marker Mode", self.markerModeBox)
        
        self.numLinesSpinner = QSpinBox(cw)
        self.numLinesSpinner.minimumSize = QSize(150, 0)
        self.numLinesSpinner.setRange(1, 999)
        self.numLinesSpinner.setValue(settings.value("exercise/" + id + "/num_lines", 4))
        self.numLinesSpinner.connect("valueChanged(int)", self.onNumLinesSpinnerValueChanged)
        cwl.addRow("Number of Lines", self.numLinesSpinner)
        
        self.tempoSpinner = QSpinBox(cw)
        self.tempoSpinner.minimumSize = QSize(150, 0)
        self.tempoSpinner.setRange(1, 999)
        self.tempoSpinner.setValue(settings.value("exercise/" + id + "/tempo", 60))
        self.tempoSpinner.connect("valueChanged(int)", self.onTempoSpinnerValueChanged)
        cwl.addRow("Tempo", self.tempoSpinner)
        
        self.subdivsSpinner = QSpinBox(cw)
        self.subdivsSpinner.minimumSize = QSize(150, 0)
        self.subdivsSpinner.setRange(1, 32)
        self.subdivsSpinner.setValue(settings.value("exercise/" + self.ex.getID() + "/metronome_subdivs", 2))
        self.subdivsSpinner.connect("valueChanged(int)", self.onSubdivsSpinnerValueChanged)
        cwl.addRow("Metronome Subdivisions", self.subdivsSpinner)
        
        self.setPerformanceMarkerMode(self.markerModeBox.itemData(self.markerModeBox.currentIndex))
        self.numLines = self.numLinesSpinner.value
        self.setTempo(self.tempoSpinner.value)
        self.setMetronomeNumSubdivisions(self.subdivsSpinner.value)
        
        self.ex.addControlWidget(cw)
        
    def onTempoSpinnerValueChanged(self, tempo):
        self.setTempo(tempo)
        
        settings = QSettings()
        settings.setValue("exercise/" + self.ex.getID() + "/tempo", tempo)
    
    def onSubdivsSpinnerValueChanged(self, subdivs):
        self.setMetronomeNumSubdivisions(subdivs)
        
        settings = QSettings()
        settings.setValue("exercise/" + self.ex.getID() + "/metronome_subdivs", subdivs)
    
    def onMarkerModeBoxActivated(self, index):
        mode = self.markerModeBox.itemData(index)
        self.setPerformanceMarkerMode(mode)
        
        settings = QSettings()
        settings.setValue("exercise/" + self.ex.getID() + "/marker_mode", mode)
    
    def onNumLinesSpinnerValueChanged(self, numLines):
        self.numLines = numLines
        
        settings = QSettings()
        settings.setValue("exercise/" + self.ex.getID() + "/num_lines", numLines)
    
    def _generateExercise(self):
        srw = self.ex.getMainWidget()
        code = self.generateExercise()
        srw.setGMNCode(code)

    def onActivated(self):
        self._generateExercise()
        
    def onGenerateRequested(self):
        self._generateExercise()
    
    def generateExercise(self):
        return "{ [  ] }"
    
    def register(self, mainWindow):
        mainWindow.addExercise(self.ex)
    
    def setTempo(self, tempo):
        srw = self.ex.getMainWidget()
        srw.setTempo(tempo)
        
        metronome = srw.getMetronome()
        metronome.setTicksPerMinute(tempo)
    
    def setMetronomeTicksPerMeasure(self, ticksPerMeasure):
        metronome = self.ex.getMainWidget().getMetronome()
        metronome.setTicksPerMeasure(ticksPerMeasure)
        
    def setMetronomeNumSubdivisions(self, numSubdivs):
        metronome = self.ex.getMainWidget().getMetronome()
        metronome.setNumSubdivisions(numSubdivs)
    
    def setupMetronome(self, ticksPerMeasure, numSubdivs):
        metronome = self.ex.getMainWidget().getMetronome()
        metronome.setTicksPerMeasure(ticksPerMeasure)
        metronome.setNumSubdivisions(numSubdivs)
    
    def setLineCount(self, lcount, plcount = 0):
        srw = self.ex.getMainWidget()
        sw = srw.getScoreWidget()
        sw.setLineCount(lcount)
        sw.setPreviousPageCount(plcount)
    
    def setPerformanceMarkerMode(self, mode):
        srw = self.ex.getMainWidget()
        sw = srw.getScoreWidget()
        sw.setPerformanceMarkerMode(mode)
    
    def setRhythmMode(self, rhythmMode):
        srw = self.ex.getMainWidget()
        srw.setRhythmMode(rhythmMode)




class SingleLineSightReadingExercise(SimpleSightReadingExercise):
    
    def __init__(self, id, name, category, iconpath = None):
        SimpleSightReadingExercise.__init__(self, id, name, category, iconpath)
        
        self.possibleNotes = []
        self.possibleRhythms = {}
        self.numBarsPerLine = 3
        self.rhythmStaffMode = False
        
        self.setLineCount(3)
    
    def measureCompositionPredicate(self, seq, sum):
        return True
    
    def generateExercise(self):
        timesigs = self.possibleRhythms.keys()
        
        timesig = timesigs[random.randrange(len(timesigs))]
        
        self.setMetronomeTicksPerMeasure(timesig[0])
        code = "{ [ \\meter<\"" + str(timesig[0]) + "/" + str(timesig[1]) + "\"> \\pageFormat<20cm,25cm,2mm,2mm,2mm,2mm>"
        
        if self.rhythmStaffMode:
            code = code + " \\staffFormat<style=\"1-line\">"
        
        for line in range(self.numLines):
            if line != 0:
                code = code + " \\newPage "
            
            for i in range(3):
                if line != 0 and i != 0:
                    code = code + " | "
                
                rhythm = randomCompositionPredicate(self.possibleRhythms[timesig].keys(),
                                                    max(self.possibleRhythms[timesig].keys()),
                                                    self.measureCompositionPredicate)
            
                for nrhythm in rhythm:
                    pitch = self.possibleNotes[random.randrange(len(self.possibleNotes))]
                    code = code + " " + str(pitch) + self.possibleRhythms[timesig][nrhythm]
        
        code = code + " ] }"
        return code
    
    
