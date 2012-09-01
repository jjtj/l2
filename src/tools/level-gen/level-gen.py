import sys
import os
import utils
import inkscape
import re
import xml.etree.ElementTree as ET


def findElementById(tree, id):
    root = tree.getroot()
    root.findAll('//')

def extractDocumentDimension(svgfn):
    tree = ET.parse(svgfn)
    root = tree.getroot()

    w = int(root.attrib['width'])
    h = int(root.attrib['height'])

    return [w,h]

def groupLevelObjects(objects):
    
    print 'Grouping level objects...'
        
    prog = re.compile('level-(\d+)-.+');
        
    group = dict()
    
    for obj in objects:
        result = prog.match(obj['id'])
        if result <> None:
            level = result.group(1)
            isIn = level in group
            
            if isIn == False:
                group[level] = []
                
            group[level].append(obj)
            
    return group

def isCheckPoint(id):
    result = re.match('level-(\d+)-check-point-(\d+)', id)
    if result == None:
        return False
    
    return True

def isTerrain(id):
    result = re.match('level-(\d+)-terrain', id)
    return result <> None

def isTitle(id):
    result = re.match('level-(\d+)-title', id)
    return result <> None

    
def createLevel(lvlNum, objects):
    level = dict(levelNum=lvlNum,   \
                 checkPoints = [],  \
                 terrain=None,
                 title=None)
    
    level['levelNum'] = int(lvlNum)
    for obj in objects:
        objid = obj['id']
        if isCheckPoint(objid):
            level['checkPoints'].append(obj)
        elif isTerrain(objid):
            level['terrain'] = obj
        elif isTitle:
            level['title'] = obj
        else:
            print 'Unexpected object: ' + objid
    
    return level


def collectAndCreateLevels(inputFn):
    objects = inkscape.queryAllObjects(inputFn)
    groups = groupLevelObjects(objects)

    print 'Collect & creating level data...'
    levels = []
    for v in groups:
        lvl = createLevel(v, groups[v])
        levels.append(lvl)

    levels.sort(key=lambda lvl: lvl['levelNum'])
    for lvl in levels:
        print lvl

    return levels

def updateLevelCheckPoints(inputfn, lvl):
    pass


def begin(inputfn, outputfolder):

    dim = extractDocumentDimension(inputfn)
    print 'Dimension: ' + str(dim)

    world = dict(w=dim[0],  \
                 h=dim[1])
    world['level'] = collectAndCreateLevels(inputfn)
        
    
if __name__ == "__main__":
    if len(sys.argv) <> 3:
        print "Usage: level-gen.py [input-svg-file] [output-folder]"
        exit(1)
        
    inputFn = os.path.abspath(sys.argv[1])
    outputFolder = os.path.abspath(sys.argv[2])
    utils.ensureDir(outputFolder)
    
    print 'Level generator:'
    print '----------------'
    print 'Input Svg File: ' + inputFn
    print 'Output Folder: ' + outputFolder

    begin(inputFn, outputFolder)
    
    
    
    