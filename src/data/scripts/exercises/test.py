from PythonQt.QtGui import *
from exercises.parts.simplesightreading import *
from lib.util import *

class TestExercise(SimpleSightReadingExercise):
    
    def __init__(self):
        SimpleSightReadingExercise.__init__(self, "test", "Test", "Test")
        
        self.setLineCount(3)
    
    def generateExercise(self):
        code = '''
            {
                [
                    \\pageFormat<10cm,25cm,2mm,2mm,2mm,2mm>
                    
                    c/4 \\space<4mm> e/4 \\space<4mm> f/2 \\space<4mm> |
                    g/8 \\space<3mm> f/8 \\space<4mm> c/4 \\space<4mm> e/2
                ]
            }
        '''
        return code

ex = TestExercise()
#ex.register(mainWindow)
