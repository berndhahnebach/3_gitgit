#***************************************************************************
#*                                                                         *
#*   Copyright (c) 2011                                                    *  
#*   Yorik van Havre <yorik@uncreated.net>                                 *  
#*                                                                         *
#*   This program is free software; you can redistribute it and/or modify  *
#*   it under the terms of the GNU Lesser General Public License (LGPL)    *
#*   as published by the Free Software Foundation; either version 2 of     *
#*   the License, or (at your option) any later version.                   *
#*   for detail see the LICENCE text file.                                 *
#*                                                                         *
#*   This program is distributed in the hope that it will be useful,       *
#*   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
#*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
#*   GNU Library General Public License for more details.                  *
#*                                                                         *
#*   You should have received a copy of the GNU Library General Public     *
#*   License along with this program; if not, write to the Free Software   *
#*   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  *
#*   USA                                                                   *
#*                                                                         *
#***************************************************************************

import FreeCAD,FreeCADGui,Draft,ArchComponent,DraftVecUtils,ArchCommands
from FreeCAD import Vector
from PyQt4 import QtCore
from DraftTools import translate

__title__="FreeCAD Wall"
__author__ = "Yorik van Havre"
__url__ = "http://free-cad.sourceforge.net"

def makeWall(baseobj=None,width=None,height=None,align="Center",name=str(translate("Arch","Wall"))):
    '''makeWall(obj,[width],[height],[align],[name]): creates a wall based on the
    given object, which can be a sketch, a draft object, a face or a solid. align
    can be "Center","Left" or "Right"'''
    obj = FreeCAD.ActiveDocument.addObject("Part::FeaturePython",name)
    _Wall(obj)
    _ViewProviderWall(obj.ViewObject)
    if baseobj:
        obj.Base = baseobj
    if width:
        obj.Width = width
    if height:
        obj.Height = height
    obj.Align = align
    if obj.Base:
        obj.Base.ViewObject.hide()
    obj.ViewObject.ShapeColor = ArchCommands.getDefaultColor("Wall")
    return obj

def joinWalls(walls):
    "joins the given list of walls into one sketch-based wall"
    if not walls:
        return None
    if not isinstance(walls,list):
        walls = [walls]
    if not areSameWallTypes(walls):
        return None
    base = walls.pop()
    if base.Base:
        if base.Base.Shape.Faces:
            return None
        if Draft.getType(base.Base) == "Sketch":
            sk = base.Base
        else:
            sk = Draft.makeSketch(base.Base,autoconstraints=True)
            if sk:
                base.Base = sk
    for w in walls:
        if w.Base:
            if not base.Base.Shape.Faces:
                for e in base.Base.Shape.Edges:
                    sk.addGeometry(e)
    FreeCAD.ActiveDocument.recompute()
    return base
    
def mergeShapes(w1,w2):
    "returns a Shape built on two walls that share same properties and have a coincident endpoint"
    if not areSameWallTypes([w1,w2]):
        return None
    if (not hasattr(w1.Base,"Shape")) or (not hasattr(w2.Base,"Shape")):
        return None
    if w1.Base.Shape.Faces or w2.Base.Shape.Faces:
        return None
    
    eds = w1.Base.Shape.Edges + w2.Base.Shape.Edges
    import DraftGeomUtils
    w = DraftGeomUtils.findWires(eds)
    if len(w) == 1:
        print "found common wire"
        normal,width,height = w1.Proxy.getDefaultValues(w1)
        print w[0].Edges
        sh = w1.Proxy.getBase(w1,w[0],normal,width,height)
        print sh
        return sh
    return None

def areSameWallTypes(walls):
    "returns True is all the walls in the given list have same height, width, and alignment"
    for w in walls:
        if Draft.getType(w) != "Wall":
            return False
    for att in ["Width","Height","Align"]:
        value = None
        for w in walls:
            if not hasattr(w,att):
                return False
            if not value:
                value = getattr(w,att)
            else:
                if type(value) == float:
                    if round(value,Draft.precision()) != round(getattr(w,att),Draft.precision()):
                        return False
                else:
                    if value != getattr(w,att):
                        return False
    return True

class _CommandWall:
    "the Arch Wall command definition"
    def GetResources(self):
        return {'Pixmap'  : 'Arch_Wall',
                'MenuText': QtCore.QT_TRANSLATE_NOOP("Arch_Wall","Wall"),
                'Accel': "W, A",
                'ToolTip': QtCore.QT_TRANSLATE_NOOP("Arch_Wall","Creates a wall object from scratch or from a selected object (wire, face or solid)")}
        
    def Activated(self):

        global QtGui, QtCore
        from PyQt4 import QtGui, QtCore

        self.Width = 0.1
        self.Height = 1
        self.Align = "Center"
        self.continueCmd = False
        p = FreeCAD.ParamGet("User parameter:BaseApp/Preferences/Mod/Arch")
        self.JOIN_WALLS = p.GetBool("joinWallSketches")
        sel = FreeCADGui.Selection.getSelection()
        done = False
        self.existing = []
        if sel:
            import Draft
            if Draft.getType(sel[0]) != "Wall":
                FreeCAD.ActiveDocument.openTransaction(str(translate("Arch","Create Wall")))
                FreeCADGui.doCommand('import Arch')
                for obj in sel:
                    FreeCADGui.doCommand('Arch.makeWall(FreeCAD.ActiveDocument.'+obj.Name+')')
                FreeCAD.ActiveDocument.commitTransaction()
                FreeCAD.ActiveDocument.recompute()
                done = True
        if not done:
            import DraftTrackers
            self.points = []
            self.tracker = DraftTrackers.boxTracker()
            FreeCADGui.Snapper.getPoint(callback=self.getPoint,extradlg=self.taskbox())

    def getPoint(self,point=None,obj=None):
        "this function is called by the snapper when it has a 3D point"
        if obj:
            if Draft.getType(obj) == "Wall":
                if not obj in self.existing:
                    self.existing.append(obj)
        if point == None:
            self.tracker.finalize()
            return
        self.points.append(point)
        if len(self.points) == 1:
            self.tracker.on()
            FreeCADGui.Snapper.getPoint(last=self.points[0],callback=self.getPoint,movecallback=self.update,extradlg=self.taskbox())
        elif len(self.points) == 2:
            import Part
            l = Part.Line(self.points[0],self.points[1])
            self.tracker.finalize()
            FreeCAD.ActiveDocument.openTransaction(str(translate("Arch","Create Wall")))
            FreeCADGui.doCommand('import Arch')
            FreeCADGui.doCommand('import Part')
            FreeCADGui.doCommand('trace=Part.Line(FreeCAD.'+str(l.StartPoint)+',FreeCAD.'+str(l.EndPoint)+')')
            if not self.existing:
                # no existing wall snapped, just add a default wall
                self.addDefault(l)
            else:
                if self.JOIN_WALLS:
                    # join existing subwalls first if possible, then add the new one
                    w = joinWalls(self.existing)
                    if w:
                        if areSameWallTypes([w,self]):
                            FreeCADGui.doCommand('FreeCAD.ActiveDocument.'+w.Name+'.Base.addGeometry(trace)')
                        else:
                            # if not possible, add new wall as addition to the existing one
                            self.addDefault(l)
                            FreeCADGui.doCommand('Arch.addComponents(FreeCAD.ActiveDocument.'+FreeCAD.ActiveDocument.Objects[-1].Name+',FreeCAD.ActiveDocument.'+w.Name+')')
                    else:
                        self.addDefault(l)
                else:
                    # add new wall as addition to the first existing one
                    self.addDefault(l)
                    FreeCADGui.doCommand('Arch.addComponents(FreeCAD.ActiveDocument.'+FreeCAD.ActiveDocument.Objects[-1].Name+',FreeCAD.ActiveDocument.'+self.existing[0].Name+')')
            FreeCAD.ActiveDocument.commitTransaction()
            FreeCAD.ActiveDocument.recompute()
            if self.continueCmd:
                self.Activated()

    def addDefault(self,l):
        FreeCADGui.doCommand('base=FreeCAD.ActiveDocument.addObject("Sketcher::SketchObject","'+str(translate('Arch','WallTrace'))+'")')
        FreeCADGui.doCommand('base.addGeometry(trace)')
        FreeCADGui.doCommand('Arch.makeWall(base,width='+str(self.Width)+',height='+str(self.Height)+',align="'+str(self.Align)+'")')

    def update(self,point):
        "this function is called by the Snapper when the mouse is moved"
        b = self.points[0]
        n = FreeCAD.DraftWorkingPlane.axis
        bv = point.sub(b)
        dv = bv.cross(n)
        dv = DraftVecUtils.scaleTo(dv,self.Width/2)
        if self.Align == "Center":
            self.tracker.update([b,point])
        elif self.Align == "Left":
            self.tracker.update([b.add(dv),point.add(dv)])
        else:
            dv = DraftVecUtils.neg(dv)
            self.tracker.update([b.add(dv),point.add(dv)])

    def taskbox(self):
        "sets up a taskbox widget"
        w = QtGui.QWidget()
        w.setWindowTitle(str(translate("Arch","Wall options")))
        lay0 = QtGui.QVBoxLayout(w)
        lay1 = QtGui.QHBoxLayout()
        lay0.addLayout(lay1)
        label1 = QtGui.QLabel(str(translate("Arch","Width")))
        lay1.addWidget(label1)
        value1 = QtGui.QDoubleSpinBox()
        value1.setDecimals(2)
        value1.setValue(self.Width)
        lay1.addWidget(value1)
        lay2 = QtGui.QHBoxLayout()
        lay0.addLayout(lay2)
        label2 = QtGui.QLabel(str(translate("Arch","Height")))
        lay2.addWidget(label2)
        value2 = QtGui.QDoubleSpinBox()
        value2.setDecimals(2)
        value2.setValue(self.Height)
        lay2.addWidget(value2)
        lay3 = QtGui.QHBoxLayout()
        lay0.addLayout(lay3)
        label3 = QtGui.QLabel(str(translate("Arch","Alignment")))
        lay3.addWidget(label3)
        value3 = QtGui.QComboBox()
        items = ["Center","Left","Right"]
        value3.addItems(items)
        value3.setCurrentIndex(items.index(self.Align))
        lay3.addWidget(value3)
        value4 = QtGui.QCheckBox(str(translate("Arch","Continue")))
        lay0.addWidget(value4)
        QtCore.QObject.connect(value1,QtCore.SIGNAL("valueChanged(double)"),self.setWidth)
        QtCore.QObject.connect(value2,QtCore.SIGNAL("valueChanged(double)"),self.setHeight)
        QtCore.QObject.connect(value3,QtCore.SIGNAL("currentIndexChanged(int)"),self.setAlign)
        QtCore.QObject.connect(value4,QtCore.SIGNAL("stateChanged(int)"),self.setContinue)
        return w
        
    def setWidth(self,d):
        self.Width = d
        self.tracker.width(d)

    def setHeight(self,d):
        self.Height = d
        self.tracker.height(d)

    def setAlign(self,i):
        self.Align = ["Center","Left","Right"][i]

    def setContinue(self,i):
        self.continueCmd = bool(i)
        
class _Wall(ArchComponent.Component):
    "The Wall object"
    def __init__(self,obj):
        ArchComponent.Component.__init__(self,obj)
        obj.addProperty("App::PropertyLength","Width","Base",
                        str(translate("Arch","The width of this wall. Not used if this wall is based on a face")))
        obj.addProperty("App::PropertyLength","Height","Base",
                        str(translate("Arch","The height of this wall. Keep 0 for automatic. Not used if this wall is based on a solid")))
        obj.addProperty("App::PropertyEnumeration","Align","Base",
                        str(translate("Arch","The alignment of this wall on its base object, if applicable")))
        obj.addProperty("App::PropertyVector","Normal","Base",
                        str(translate("Arch","The normal extrusion direction of this object (keep (0,0,0) for automatic normal)")))
        obj.addProperty("App::PropertyBool","ForceWire","Base",
                        str(translate("Arch","If True, if this wall is based on a face, it will use its border wire as trace, and disconsider the face.")))
        obj.Align = ['Left','Right','Center']
        obj.ForceWire = False
        self.Type = "Wall"
        obj.Width = 0.1
        obj.Height = 0
        
    def execute(self,obj):
        self.createGeometry(obj)
        
    def onChanged(self,obj,prop):
        self.hideSubobjects(obj,prop)
        if prop in ["Base","Height","Width","Align","Additions","Subtractions"]:
            self.createGeometry(obj)
            
    def getDefaultValues(self,obj):
        "returns normal,width,height values from this wall"
        width = 1.0
        if hasattr(obj,"Width"):
            if obj.Width:
                width = obj.Width
        height = normal = None
        if hasattr(obj,"Height"):
            if obj.Height:
                height = obj.Height
            else:
                for p in obj.InList:
                    if Draft.getType(p) == "Floor":
                        height = p.Height
        if not height: 
            height = 1.0
        if hasattr(obj,"Normal"):
            if obj.Normal == Vector(0,0,0):
                normal = Vector(0,0,1)
            else:
                normal = Vector(obj.Normal)
        else:
            normal = Vector(0,0,1)
        return normal,width,height
            
    def getBase(self,obj,wire,normal,width,height):
        "returns a full shape from a base wire"
        import DraftGeomUtils,Part
        flat = False
        if hasattr(obj.ViewObject,"DisplayMode"):
            flat = (obj.ViewObject.DisplayMode == "Flat 2D")
        dvec = DraftGeomUtils.vec(wire.Edges[0]).cross(normal)
        if not DraftVecUtils.isNull(dvec):
            dvec.normalize()
        if obj.Align == "Left":
            dvec = dvec.multiply(width)
            w2 = DraftGeomUtils.offsetWire(wire,dvec)
            w1 = Part.Wire(DraftGeomUtils.sortEdges(wire.Edges))
            sh = DraftGeomUtils.bind(w1,w2)
        elif obj.Align == "Right":
            dvec = dvec.multiply(width)
            dvec = DraftVecUtils.neg(dvec)
            w2 = DraftGeomUtils.offsetWire(wire,dvec)
            w1 = Part.Wire(DraftGeomUtils.sortEdges(wire.Edges))
            sh = DraftGeomUtils.bind(w1,w2)
        elif obj.Align == "Center":
            dvec = dvec.multiply(width/2)
            w1 = DraftGeomUtils.offsetWire(wire,dvec)
            dvec = DraftVecUtils.neg(dvec)
            w2 = DraftGeomUtils.offsetWire(wire,dvec)
            sh = DraftGeomUtils.bind(w1,w2)
        # fixing self-intersections
        sh.fix(0.1,0,1)
        if height and (not flat):
            norm = Vector(normal).multiply(height)
            sh = sh.extrude(norm)
        return sh

    def createGeometry(self,obj):
        "builds the wall shape"

        if not obj.Base:
            return

        import Part, DraftGeomUtils
        
        pl = obj.Placement
        normal,width,height = self.getDefaultValues(obj)

        # computing shape
        base = None
        if obj.Base.isDerivedFrom("Part::Feature"):
            if not obj.Base.Shape.isNull():
                if obj.Base.Shape.isValid():
                    base = obj.Base.Shape.copy()
                    if base.Solids:
                        pass
                    elif (len(base.Faces) == 1) and (not obj.ForceWire):
                        if height:
                            norm = normal.multiply(height)
                            base = base.extrude(norm)
                    elif len(base.Wires) == 1:
                        temp = None
                        for wire in obj.Base.Shape.Wires:
                            sh = self.getBase(obj,wire,normal,width,height)
                            if temp:
                                temp = temp.fuse(sh)
                            else:
                                temp = sh
                        base = temp
                    elif base.Edges:
                        wire = Part.Wire(base.Edges)
                        if wire:
                            sh = self.getBase(obj,wire,normal,width,height)
                            if sh:
                                base = sh
                    else:
                        base = None
                        FreeCAD.Console.PrintError(str(translate("Arch","Error: Invalid base object")))
                    
        elif obj.Base.isDerivedFrom("Mesh::Feature"):
            if obj.Base.Mesh.isSolid():
                if obj.Base.Mesh.countComponents() == 1:
                    sh = ArchCommands.getShapeFromMesh(obj.Base.Mesh)
                    if sh.isClosed() and sh.isValid() and sh.Solids and (not sh.isNull()):
                        base = sh
                    else:
                        FreeCAD.Console.PrintWarning(str(translate("Arch","This mesh is an invalid solid")))
                        obj.Base.ViewObject.show()
                        
        base = self.processSubShapes(obj,base)
        
        if base:
            if not base.isNull():
                if base.isValid() and base.Solids:
                    if base.Volume < 0:
                        base.reverse()
                    if base.Volume < 0:
                        FreeCAD.Console.PrintError(str(translate("Arch","Couldn't compute the wall shape")))
                        return
                    try:
                        base = base.removeSplitter()
                    except:
                        FreeCAD.Console.PrintError(str(translate("Arch","Error removing splitter from wall shape")))
                    obj.Shape = base
                    if not DraftGeomUtils.isNull(pl):
                        obj.Placement = pl

class _ViewProviderWall(ArchComponent.ViewProviderComponent):
    "A View Provider for the Wall object"

    def __init__(self,vobj):
        ArchComponent.ViewProviderComponent.__init__(self,vobj)

    def getIcon(self):
        import Arch_rc
        return ":/icons/Arch_Wall_Tree.svg"

    def getDisplayModes(self,vobj):
        return ["Flat 2D"]

    def setDisplayMode(self,mode):
        self.Object.Proxy.createGeometry(self.Object)
        if mode == "Flat 2D":
            return "Flat Lines"
        else:
            return mode

    def attach(self,vobj):
        self.Object = vobj.Object
        return

FreeCADGui.addCommand('Arch_Wall',_CommandWall())
