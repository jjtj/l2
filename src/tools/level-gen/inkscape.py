import os
import sys

INKSCAPE_PATH ="\"C:/Program Files (x86)/Inkscape/inkscape.exe\" "
TEMP_QUERY_FILE = "inter.txt" 

def queryAllObjects(svgfn):
    
    print ''
    print 'Query inkscape objects from ' + svgfn
    print '----------------------------'
    print 'Working...'
    print ''
    
    cmd = _buildQueryCmd(svgfn, "-S")
    execute(cmd)
    
    entities = []
    
    lines = open(TEMP_QUERY_FILE).readlines()
    for ln in lines:
        obj = _parseObjectLine(ln)
        if obj <> None:
            entities.append(obj)
    
    print 'Total %d entities found...' % (len(entities))
    return entities
            

def exportPage(svgfn, outfolder):

    p = os.path.abspath(os.path.join(outfolder, 'export-page.png'))
    cmd = _buildExportCmd(svgfn, "-e " + p)

    print "Inkscape export page:"
    print cmd

    execute(cmd)

    return p


def exportObject(svgfn, id, outfolder):
    outpath = os.path.abspath(os.path.join(outfolder, id + ".png"))
    cmd = _buildExportCmd(svgfn, "-j -i " + id + " -e " + outpath)
    print "Inkscape export object:"
    print cmd

    execute(cmd)

    return outpath


def execute(cmd):
    os.system(cmd)
    
def _buildQueryCmd(svgfn, params):
    cmd = INKSCAPE_PATH + params + " " + svgfn + " > " + TEMP_QUERY_FILE
    return cmd

def _buildExportCmd(svgfn, params):
    cmd = INKSCAPE_PATH + params + " " + svgfn
    return cmd 


def _toInt(s):
    return int(float(s))    

def _parseObjectLine(ln):
    p = ln.split(',')
    if len(p) <> 5:
        return None
    
    return dict(id=p[0],    \
                x=_toInt(p[1]),\
                y=_toInt(p[2]),\
                w=_toInt(p[3]),\
                h=_toInt(p[4]))
    
    