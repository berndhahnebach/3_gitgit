# ***************************************************************************
# *   Copyright (c) 2009, 2010 Yorik van Havre <yorik@uncreated.net>        *
# *   Copyright (c) 2009, 2010 Ken Cline <cline@frii.com>                   *
# *   Copyright (c) 2019, 2020 Carlo Pavan <carlopav@gmail.com>             *
# *                                                                         *
# *   This program is free software; you can redistribute it and/or modify  *
# *   it under the terms of the GNU Lesser General Public License (LGPL)    *
# *   as published by the Free Software Foundation; either version 2 of     *
# *   the License, or (at your option) any later version.                   *
# *   for detail see the LICENCE text file.                                 *
# *                                                                         *
# *   This program is distributed in the hope that it will be useful,       *
# *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
# *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
# *   GNU Library General Public License for more details.                  *
# *                                                                         *
# *   You should have received a copy of the GNU Library General Public     *
# *   License along with this program; if not, write to the Free Software   *
# *   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  *
# *   USA                                                                   *
# *                                                                         *
# ***************************************************************************
"""Provide the support functions to Draft_Edit for Arch objects."""
## @package gui_edit_arch_objects
# \ingroup DRAFT
# \brief Provide the support functions to Draft_Edit for Arch objects

__title__ = "FreeCAD Draft Edit Tool"
__author__ = ("Yorik van Havre, Werner Mayer, Martin Burbaum, Ken Cline, "
              "Dmitry Chigrin, Carlo Pavan")
__url__ = "https://www.freecadweb.org"


import math
import FreeCAD as App
import DraftVecUtils

from draftutils.translate import translate
import draftutils.utils as utils


# SKETCH: just if it's composed by a single segment-----------------------

def getSketchPts(obj):
    """Return the list of edipoints for the given single line sketch.

    (WallTrace)
    0 : startpoint
    1 : endpoint
    """
    editpoints = []
    if obj.GeometryCount == 1:
        editpoints.append(obj.getGlobalPlacement().multVec(obj.getPoint(0,1)))
        editpoints.append(obj.getGlobalPlacement().multVec(obj.getPoint(0,2)))
        return editpoints
    else:
        _wrn = translate("draft", "Sketch is too complex to edit: "
                                  "it is suggested to use sketcher default editor")
        App.Console.PrintWarning(_wrn + "\n")
        return None


def updateSketch(obj, nodeIndex, v):
    """Move a single line sketch vertex a certain displacement.

    (single segment sketch object, node index as Int, App.Vector)
    move a single line sketch (WallTrace) vertex according to a given App.Vector
    0 : startpoint
    1 : endpoint
    """
    if nodeIndex == 0:
        obj.movePoint(0,1,obj.getGlobalPlacement().inverse().multVec(v))
    elif nodeIndex == 1:
        obj.movePoint(0,2,obj.getGlobalPlacement().inverse().multVec(v))
    obj.recompute()


# WALL---------------------------------------------------------------------

def getWallPts(obj):
    """Return the list of edipoints for the given Arch Wall object.

    0 : height of the wall
    1-to end : base object editpoints, in place with the wall
    """
    editpoints = []
    # height of the wall
    editpoints.append(obj.getGlobalPlacement().multVec(App.Vector(0,0,obj.Height)))
    # try to add here an editpoint based on wall height (maybe should be good to associate it with a circular tracker)
    if obj.Base:
        # base points are added to self.trackers under wall-name key
        basepoints = []
        if utils.get_type(obj.Base) in ["Wire","Circle","Rectangle",
                                        "Polygon", "Sketch"]:
            pass # TODO: make it work again
            #basepoints = self.getEditPoints(obj.Base)
            #for point in basepoints:
            #    editpoints.append(obj.Placement.multVec(point)) #works ok except if App::Part is rotated... why?
    return editpoints


def updateWallTrackers(obj):
    """Update self.trackers[obj.Name][0] to match with given object."""
    pass


def updateWall(obj, nodeIndex, v):
    if nodeIndex == 0:
        delta= obj.getGlobalPlacement().inverse().multVec(v)
        vz = DraftVecUtils.project(delta, App.Vector(0, 0, 1))
        if vz.Length > 0:
            obj.Height = vz.Length
    elif nodeIndex > 0:
        if obj.Base:
            if utils.get_type(obj.Base) in ["Wire", "Circle", "Rectangle",
                                            "Polygon", "Sketch"]:
                pass #TODO: make it work again
                #self.update(obj.Base, nodeIndex - 1, 
                #    obj.Placement.inverse().multVec(v))
    obj.recompute()


# WINDOW-------------------------------------------------------------------

def getWindowPts(obj):
    editpoints = []
    pos = obj.Base.Placement.Base
    h = float(obj.Height) + pos.z
    normal = obj.Normal
    angle = normal.getAngle(App.Vector(1, 0, 0))
    editpoints.append(pos)
    editpoints.append(App.Vector(pos.x + float(obj.Width) * math.cos(angle-math.pi / 2.0),
                                            pos.y + float(obj.Width) * math.sin(angle-math.pi / 2.0),
                                            pos.z))
    editpoints.append(App.Vector(pos.x, pos.y, h))
    return editpoints


def updateWindow(obj, nodeIndex, v):
    pos = obj.Base.Placement.Base
    if nodeIndex == 0:
        obj.Base.Placement.Base = v
        obj.Base.recompute()
    if nodeIndex == 1:
        obj.Width = pos.sub(v).Length
        obj.Base.recompute()
    if nodeIndex == 2:
        obj.Height = pos.sub(v).Length
        obj.Base.recompute()
    for obj in obj.Hosts:
        obj.recompute()
    obj.recompute()


# STRUCTURE----------------------------------------------------------------

def getStructurePts(obj):
    if obj.Nodes:
        editpoints = []
        # TODO: make it work again
        #self.originalDisplayMode = obj.ViewObject.DisplayMode
        #self.originalPoints = obj.ViewObject.NodeSize
        #self.originalNodes = obj.ViewObject.ShowNodes
        #self.obj.ViewObject.DisplayMode = "Wireframe"
        #self.obj.ViewObject.NodeSize = 1
        ## self.obj.ViewObject.ShowNodes = True
        #for p in obj.Nodes:
        #    if self.pl:
        #        p = self.pl.multVec(p)
        #    editpoints.append(p)
        #return editpoints
    else:
        return None


def updateStructure(obj, nodeIndex, v):
    nodes = obj.Nodes
    nodes[nodeIndex] = obj.Placement.inverse().multVec(v)
    obj.Nodes = nodes


# SPACE--------------------------------------------------------------------

def getSpacePts(obj):
    try:
        editpoints = []
        editpoints.append(obj.ViewObject.Proxy.getTextPosition(obj.ViewObject))
        return editpoints
    except:
        pass


def updateSpace(obj, nodeIndex, v):
    if nodeIndex == 0:
        obj.ViewObject.TextPosition = v


# PANELS-------------------------------------------------------------------

def getPanelCutPts(obj):
    editpoints = []
    if obj.TagPosition.Length == 0:
        pos = obj.Shape.BoundBox.Center
    else:
        pos = obj.Placement.multVec(obj.TagPosition)
    editpoints.append(pos)
    return editpoints


def updatePanelCut(obj, nodeIndex, v):
    if nodeIndex == 0:
        obj.TagPosition = obj.Placement.inverse().multVec(v)


def getPanelSheetPts(obj):
    editpoints = []
    editpoints.append(obj.Placement.multVec(obj.TagPosition))
    for o in obj.Group:
        editpoints.append(obj.Placement.multVec(o.Placement.Base))
    return editpoints


def updatePanelSheet(obj, nodeIndex, v):
    if nodeIndex == 0:
        obj.TagPosition = obj.Placement.inverse().multVec(v)
    else:
        obj.Group[nodeIndex-1].Placement.Base = obj.Placement.inverse().multVec(v)
