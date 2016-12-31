from PythonQt.QtGui import *
from exercises.parts.simplesightreading import *
from lib.util import *

class SightReadingRhythmExercise1(SingleLineSightReadingExercise):
    
    def __init__(self):
        SingleLineSightReadingExercise.__init__(self, "srr1", "Sight Reading Rhythm 1", "Sight Reading Rhythm",
                                                dataPath + "/icons/rhythm.png")
        
        self.setRhythmMode(True)
        
        self.possibleNotes = [ "g" ]
        self.possibleRhythms = {
            (3, 4) : { 1:"/8", 2:"/4", 4:"/2", 6:"/2." },
            (4, 4) : { 1:"/8", 2:"/4", 4:"/2", 8:"/1" }
        }
        self.rhythmStaffMode = True
    
    def measureCompositionPredicate(self, seq, sum):
        return seq.count(1) < 3 and (sum != 0 or countminconsecutive(seq, 1) > 1)

ex = SightReadingRhythmExercise1()
ex.register(mainWindow)
