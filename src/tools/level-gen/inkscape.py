
import os
import sys

INKSCAPE_PATH ="\"C:/Program Files (x86)/Inkscape/inkscape.exe\" " 

class Inkscape:
    def __init__(self, svgfn):
        self.svgfn = svgfn
        
    def exportPage(self):
        
        pass
    
    def execute(self, cmd):
        os.system(cmd)
    
    def buildQueryCmd(self, params, query):
        cmd =  INKSCAPE_PATH + params + " " + self.svgfn + " > inter.txt"
        return cmd
    
    def buildExportCmd(self, params):
        cmd = INKSCAPE_PATH + params + " " + self.svgfn
        return cmd
        