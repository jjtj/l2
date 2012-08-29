
import sys
import os
import utils

if __name__ == "__main__":
    if len(sys.argv) <> 3:
        print "Usage: level-gen.py [input-svg-file] [output-folder]"
        exit(1)
        
    inputFn = os.path.abspath(sys.argv[1])
    outputFolder = os.path.abspath(sys.argv[2])
    
    print 'Level generator:'
    print '----------------'
    print 'Input Svg File: ' + inputFn
    print 'Output Folder: ' + outputFolder
    
    utils.ensureDir(outputFolder)
    
    
    
    