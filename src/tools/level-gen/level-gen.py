import sys
import os
import utils
import inkscape
import terrain
import re
import json
import xml.etree.ElementTree as ET


def findElementById(root, id):
    all = root.findall('.//*')
    for el in all:
        elid = el.get('id', None)
        if elid == id:
            return el

    return None


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


def generateLevelCheckPoint(svgfn,
                            root,
                            lvl,
                            worldSize,
                            outfolder):
    chkpts = lvl['checkPoints']
    for c in chkpts:
        cid = c['id']
        el = findElementById(root, cid)

        if el == None:
            continue

        isLine = el.get('isLine', '0')
        c['isLine'] = (isLine == '1')

        if c['isLine']:
            # We just need a png file
            fn = inkscape.exportObject(svgfn, cid, outfolder)
            c['imgfile'] = fn
        else:
            terrainResult = terrain.createFromObject(svgfn, c, worldSize, outfolder)
            c['imgfile'] = terrainResult['fn']
            c['terrainjson'] = terrainResult['json']

def generateLevelTitle(svgfn, root, lvl, outfolder):
    t = lvl['title']
    tid = t['id']

    fn = inkscape.exportObject(svgfn, tid, outfolder)
    t['imgfile'] = fn



def collectAndCreateLevels(svgfn, worldsize, outfolder):
    objects = inkscape.queryAllObjects(svgfn)
    groups = groupLevelObjects(objects)

    print 'Collect & creating level data...'
    levels = []
    for v in groups:
        lvl = createLevel(v, groups[v])
        levels.append(lvl)

    levels.sort(key=lambda lvl: lvl['levelNum'])
    for lvl in levels:
        print lvl

    tree = ET.parse(svgfn)
    root = tree.getroot()

    for lvl in levels:
        generateLevelCheckPoint(svgfn, root, lvl, worldsize, outfolder)
        generateLevelTitle(svgfn, root, lvl, outfolder)

    for lvl in levels:
        print lvl

    return levels


def createTerrainImage(svgfn, outfolder):
    tree = ET.parse(svgfn)
    root = tree.getroot()

    parent_map = dict((c,p) for p in tree.getiterator() for c in p)

    for el in root.getiterator():
        id = el.get('id', '')
        if isTerrain(id) == False:
            style = el.get('style', '')
            style = style + ";display:none;"
            el.set('style', style)
        else:
            p = parent_map[el]
            while p in parent_map:
                style = p.get('style', '')
                style = style.replace('display:none;', '')
                p.set('style', style)

                p = parent_map[p]


    xmlstr = ET.tostring(root)

    fn = os.path.abspath(os.path.join(outfolder, 'temp.svg'))
    f = open(fn, "w")
    f.write('<?xml version="1.0" encoding="UTF-8" standalone="no"?>')
    f.write(xmlstr)
    f.close()

    return inkscape.exportPage(fn, outfolder)


def generateWorldTerrain(svgfn, outfolder):
    fn = createTerrainImage(svgfn, outfolder)
    print 'Terrain image generated:' + fn

    json = terrain.createTerrainFromImage(fn, outfolder, 'world-terrain')
    print json
    return json


def updateLevelCheckPoints(inputfn, lvl):
    pass


def createSize(w,h):
    return dict(w=w, h=h)


def begin(inputfn, outfolder):

    dim = extractDocumentDimension(inputfn)
    print 'Dimension: ' + str(dim)

    world = dict()
    world['dim'] = createSize(dim[0], dim[1])

    world['level'] = collectAndCreateLevels(inputfn, world['dim'], outfolder)
    world['wolrdTerrain'] = generateWorldTerrain(inputfn, outfolder)

    print 'DUMP world file:'
    jsonstr = json.dumps(world)

    print jsonstr
    jsonpath = os.path.abspath(os.path.join(outfolder, 'world.txt'))

    file = open(jsonpath, "w")
    file.write(jsonstr)
    file.close()

    print 'DONE...'

        
    
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
    
    
    
    