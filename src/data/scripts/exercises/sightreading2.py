from PythonQt.QtGui import *
from exercises.parts.simplesightreading import *
from lib.util import *

class SightReadingExercise2(SingleLineSightReadingExercise):
    
    def __init__(self):
        SingleLineSightReadingExercise.__init__(self, "sr2", "Sight Reading 2", "Sight Reading",
                                                dataPath + "/icons/sightreading.png")
        
        self.possibleNotes = [ "c", "d", "e", "f", "g" ]
        self.possibleRhythms = {
            (3, 4) : { 1:"/8", 2:"/4", 4:"/2", 6:"/2." },
            (4, 4) : { 1:"/8", 2:"/4", 4:"/2", 8:"/1" }
        }
    
    #def measureCompositionPredicate(self, seq, sum):
    #    return seq.count(1) < 3 and (sum != 0 or countminconsecutive(seq, 1) > 1)

ex = SightReadingExercise2()
ex.register(mainWindow)
