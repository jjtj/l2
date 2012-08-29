import os
import sys

def ensureDir(name):
    if os.path.exists(name) == False:
        os.mkdir(name)
        
    