import os
import sys

def ensureDir(path):
    if os.path.exists(path) == False:
        os.mkdir(path)
    
    