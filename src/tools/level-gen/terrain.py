import os
import inkscape
from PIL import Image, ImageDraw, ImageOps
import math
import sys
import json

__author__ = 'popop_000'

MINIMUM_SIZE_THRESHOLD = 1

class Quad:
    def __init__(self,x,y,w,h):
        self.x = x
        self.y = y
        self.w = w
        self.h = h
        self.isSplit = False
        self.isDeleteMarked = False
        self.children = ()
        self.parent = None

    def lt(self):
        return (self.x, self.y)
    def rt(self):
        return (self.x+self.w-1, self.y)
    def rb(self):
        return (self.x+self.w-1, self.y+self.h-1)
    def lb(self):
        return (self.x,self.y+self.h-1)

    def isLeaf(self):
        return self.hasChild() == False and self.isDeleteMarked == False and self.isSplit == False

    def isInside(self, xy):
        return (xy[0] >= self.x) and (xy[0]<(self.x + self.w)) and\
               (xy[1] >= self.y) and (xy[1]<(self.y + self.h))

    def split(self):
        self.isSplit = True

        w1 = math.floor(self.w/2)
        w2 = self.w - w1

        h1 = math.floor(self.h/2)
        h2 = self.h - h1

        x = self.x
        y = self.y
        cx = self.x + w1
        cy = self.y + h1

        if w1 == 0 or w2 == 0:
            self.children = [Quad(x,y,self.w,h1),
                             Quad(x,y+h1,self.w, h2)]
        elif h1 == 0 or h2 == 0:
            self.children = [Quad(x,y,w1,self.h),
                             Quad(x+w1,y,w2,self.h)]
        else:
            self.children = [Quad(x,y,w1,h1),
                             Quad(cx,y,w2,h1),
                             Quad(cx,cy,w2,h2),
                             Quad(x,cy,w1,h2)]

        for c in self.children:
            c.parent = self

        return self.children

    def canSplit(self):
        return self.w > MINIMUM_SIZE_THRESHOLD or self.h > MINIMUM_SIZE_THRESHOLD

    def hasChild(self):
        return self.children <> None and len(self.children)>0


    def removeChild(self, c):
        if self.children != None:
            if -1 <> self.children.index(c):
                c.parent = None
                self.children.remove(c)

    def markAsDelete(self):
        self.isDeleteMarked = True

    def walk(self,callback,userData):
        if self.children <> None:
            for c in self.children:
                c.walk(callback, userData)

        callback(self, userData)

    def serialize(self):

        splitMark = 0
        if self.isSplit:
            splitMark = 1

        me = [int(self.x),
              int(self.y),
              int(self.w),
              int(self.h),
              splitMark,
              None]
        """
        dict(
            x=self.x,
            y=self.y,
            w=self.w,
            h=self.h,
            s=self.isSplit,
            c=[])
        """

        if len(self.children)>0:
            children = []
            me[5] = children
            for c in self.children:
                if children == None:
                    children = []

                children.append(c.serialize())

        return me

class PixelChecker:
    def __init__(self, xoff, yoff, img):
        self.xoff = xoff
        self.yoff = yoff

        self.img = img
        self.pixels = list(img.getdata())
        self.w = img.size[0]
        self.h = img.size[1]

        self.covermap = dict()
        #self.bbox = self.computeBBox(img)

    def computeBBox(self,img):
        print "Compute bounding box..."
        l=1024*1024
        t=1024*1024
        r=-l
        b=-t

        (w,h) = img.size
        y=0
        while y<h:
            x = 0
            while x<w:
                c = img.getpixel((x,y))
                if c[3] <> 0:
                    l = min(l,x)
                    r = max(r,x)
                    t = min(t,y)
                    b = max(b,y)

                x = x + 1

            y = y + 1

        return Quad(l,t,r-l,b-t)

    def computeAndCacheCoverage(self, quad):
        if quad in self.covermap:
            return self.covermap[quad]

        coverResult = self.checkCoverage(quad)
        self.covermap[quad] = coverResult
        return coverResult

    def isPartialCovered(self, quad):
        return self.computeAndCacheCoverage(quad)[1]
    def isFullCovered(self, quad):
        return self.computeAndCacheCoverage(quad)[0]
    def isNoCovered(self, quad):
        (f,p) = self.computeAndCacheCoverage(quad)
        return f == False and p == False

    def toPhysical(self, xy):
        return (xy[0] - self.xoff, xy[1] - self.yoff)

    def toLogical(self, xy):
        return (xy[0] + self.xoff, xy[1] + self.yoff)

    def checkDetail(self, quad):
        xy = self.toPhysical(quad.lt())
        x = max(xy[0], 0)
        y = max(xy[1], 0)

        x1 = x
        y1 = y
        x2 = x + quad.w
        y2 = y + quad.h

        isFullCover = True
        isPartialCover = False


        while y1<y2:
            x1 = x
            idx = int(y1 * self.w + x1)
            while x1<x2:

                p = self.pixels[idx]

                #p = self.img.getpixel((x1,y1))
                isFullCover = isFullCover and p[3] <> 0
                isPartialCover = isPartialCover or p[3] <> 0

                if isFullCover == False and isPartialCover:
                    return (isFullCover, isPartialCover)

                x1 = x1 + 1
                idx = idx + 1

            y1 = y1 + 1

        if isFullCover:
            isPartialCover = False

        return (isFullCover, isPartialCover)


    def checkCoverage(self, quad):

        #(isFullCovered, isPartialCovered) = self.checkDetail(quad)
        return self.checkDetail(quad)

        """
        xy = self.toPhysical(quad.lt())
        isFullCovered = True
        isPartialCovered = False

        for xy in [quad.lt(), quad.rt(), quad.rb(), quad.lb()]:
            xy = self.toPhysical(xy)

            isCovered =self.isOccupied(xy)
            isFullCovered = isFullCovered and isCovered
            isPartialCovered = isPartialCovered or isCovered

        if(isPartialCovered == True):
            isPartialCovered = (isFullCovered == False)

        if isPartialCovered == False and isFullCovered == False:
            isPartialCovered = quad.isInside(self.toLogical(self.bbox.lt())) or\
                               quad.isInside(self.toLogical(self.bbox.rt())) or\
                               quad.isInside(self.toLogical(self.bbox.rb())) or\
                               quad.isInside(self.toLogical(self.bbox.lb()))

        return (isFullCovered, isPartialCovered)
        """

    def isOccupied(self,xy):
        try:
            pixel = self.img.getpixel(xy)
        except:
            return False

        return pixel[3] <> 0

def gen(quad, opts):
    if(opts['checker'].isFullCovered(quad)):
        pass
    elif (opts['checker'].isNoCovered(quad)):
        quad.markAsDelete()
    elif quad.canSplit() == False:
        pass
    else:
        newQuads = quad.split()
        for c in newQuads:
            gen(c, opts)

def removeDeleteNode(quad, userData):
    print (quad.x,quad.y,quad.w,quad.h)

    if quad.isDeleteMarked:
        if quad.parent != None:
            quad.parent.removeChild(quad)

        return

    if quad.isLeaf() == False and quad.hasChild() == False:
        quad.markAsDelete()
        if quad.parent != None:
            quad.parent.removeChild(quad)

def paintLeafNode(quad, userData):
    if quad.isLeaf():
        imgDraw = userData[0]
        sx = userData[1]
        sy = userData[2]

        pt1 = quad.lt()
        pt2 = quad.rb()

        pt1 = (pt1[0] - sx, pt1[1] - sy)
        pt2 = (pt2[0] - sx, pt2[1] - sy)
        imgDraw.rectangle([pt1, pt2], fill="red")

def createTerrain(imgfile, sx, sy, w, h, outfolder, outname):
    opts = dict()

    opts['input'] = imgfile

    opts['clipquad'] = Quad(sx, sy, w, h)
    opts['output'] = os.path.join(outfolder, 'terrain-' + outname + '.txt')

    print 'Create terrain...'
    print 'INPUT:' + opts['input']
    print 'OUTPUT:' + opts['output']

    im = Image.open(opts['input'])
    print 'Source image size:'
    print im.format, im.size, im.mode

    (w,h) = im.size

    rootQuad = opts['clipquad']
    opts['checker'] = PixelChecker(sx, sy, im)

    print "Finding occupied regions..."
    gen(rootQuad, opts)

    print "Finalize regions"
    rootQuad.walk(removeDeleteNode, None)

    print "Generating DEBUG image"
    # FOR DEBUGGING
    out = Image.new('RGBA', im.size)
    draw = ImageDraw.Draw(out)
    draw.rectangle([0,0,w,h],fill="yellow",outline=None)
    rootQuad.walk(paintLeafNode, [draw, sx, sy])
    out.save(os.path.join(outfolder, 'terrain-' + outname + '-debug.png'), "PNG")

    print "Convert quad data to JSON."

    """
    print "Save to json..."
    saveToJson(rootQuad, opts)
    """

    return json.dumps(rootQuad.serialize())


def saveToJson(quad, opts):
    ser = quad.serialize()
    jsonout = json.dumps(ser)

    f = open(opts['output'], 'wt')
    f.write(jsonout)
    f.close()


def createFromObject(svgfn, objinfo, worldsize, outfolder):
    outname = objinfo['id']

    sx = objinfo['x']
    sy = objinfo['y']
    w = objinfo['w']
    h = objinfo['h']

    fn = inkscape.exportObject(svgfn, objinfo['id'], outfolder)
    json = createTerrain(fn, sx, sy, w, h, outfolder, outname)
    return dict(fn=fn, json=json)


def createTerrainFromImage(imagefn, outfolder, outname):

    im = Image.open(imagefn)
    print 'Source image size:'
    print im.format, im.size, im.mode

    (w,h) = im.size

    return createTerrain(imagefn, 0,0,w,h,outfolder, outname)