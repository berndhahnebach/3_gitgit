# -*- coding: utf-8 -*-

# ***************************************************************************
# *                                                                         *
# *   Copyright (c) 2014 Yorik van Havre <yorik@uncreated.net>              *
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

import ArchPanel
import FreeCAD
import Part
import Path
import PathScripts.PathAreaOp as PathAreaOp
import PathScripts.PathLog as PathLog
import PathScripts.PathOp as PathOp
import PathScripts.PathProfileBase as PathProfileBase
import PathScripts.PathUtils as PathUtils
import numpy

# from PathScripts.PathUtils import depth_params
from PySide import QtCore

if False:
    PathLog.setLevel(PathLog.Level.DEBUG, PathLog.thisModule())
    PathLog.trackModule(PathLog.thisModule())
else:
    PathLog.setLevel(PathLog.Level.INFO, PathLog.thisModule())

# Qt translation handling
def translate(context, text, disambig=None):
    return QtCore.QCoreApplication.translate(context, text, disambig)

__title__ = "Path Profile Faces Operation"
__author__ = "sliptonic (Brad Collette)"
__url__ = "http://www.freecadweb.org"
__doc__ = "Path Profile operation based on faces."
__created__ = "2014"
__scriptVersion__ = "2e testing"
__lastModified__ = "2019-06-11 14:30 CST"


class ObjectProfile(PathProfileBase.ObjectProfile):
    '''Proxy object for Profile operations based on faces.'''

    def baseObject(self):
        '''baseObject() ... returns super of receiver
        Used to call base implementation in overwritten functions.'''
        return super(self.__class__, self)

    def areaOpFeatures(self, obj):
        '''baseObject() ... returns super of receiver
        Used to call base implementation in overwritten functions.'''
        # return PathOp.FeatureBaseFaces | PathOp.FeatureBasePanels | PathOp.FeatureRotation
        return PathOp.FeatureBaseFaces | PathOp.FeatureBasePanels

    def initAreaOp(self, obj):
        '''initAreaOp(obj) ... adds properties for hole, circle and perimeter processing.'''
        # Face specific Properties
        obj.addProperty("App::PropertyBool", "processHoles", "Profile", QtCore.QT_TRANSLATE_NOOP("App::Property", "Profile holes as well as the outline"))
        obj.addProperty("App::PropertyBool", "processPerimeter", "Profile", QtCore.QT_TRANSLATE_NOOP("App::Property", "Profile the outline"))
        obj.addProperty("App::PropertyBool", "processCircles", "Profile", QtCore.QT_TRANSLATE_NOOP("App::Property", "Profile round holes"))

        if not hasattr(obj, 'ReverseDirection'):
            obj.addProperty('App::PropertyBool', 'ReverseDirection', 'Rotation', QtCore.QT_TRANSLATE_NOOP('App::Property', 'Reverse direction of pocket operation.'))
        if not hasattr(obj, 'InverseAngle'):
            obj.addProperty('App::PropertyBool', 'InverseAngle', 'Rotation', QtCore.QT_TRANSLATE_NOOP('App::Property', 'Inverse the angle. Example: -22.5 -> 22.5 degrees.'))
        if not hasattr(obj, 'B_AxisErrorOverride'):
            obj.addProperty('App::PropertyBool', 'B_AxisErrorOverride', 'Rotation', QtCore.QT_TRANSLATE_NOOP('App::Property', 'Match B rotations to model (error in FreeCAD rendering).'))
        if not hasattr(obj, 'AttemptInverseAngle'):
            obj.addProperty('App::PropertyBool', 'AttemptInverseAngle', 'Rotation', QtCore.QT_TRANSLATE_NOOP('App::Property', 'Attempt the inverse angle for face access if original rotation fails.'))

        self.baseObject().initAreaOp(obj)

    def areaOpShapes(self, obj):
        '''areaOpShapes(obj) ... returns envelope for all base shapes or wires for Arch.Panels.'''
        PathLog.track()
        PathLog.debug("----- areaOpShapes() in PathProfileFaces.py")

        if obj.UseComp:
            self.commandlist.append(Path.Command("(Compensated Tool Path. Diameter: " + str(self.radius * 2) + ")"))
        else:
            self.commandlist.append(Path.Command("(Uncompensated Tool Path)"))

        shapes = []
        self.profileshape = []

        baseSubsTuples = []
        subCount = 0
        allTuples = []

        if obj.Base:  # The user has selected subobjects from the base.  Process each.
            if obj.EnableRotation != 'Off':
                for p in range(0, len(obj.Base)):
                    (base, subsList) = obj.Base[p]
                    for sub in subsList:
                        subCount += 1
                        shape = getattr(base.Shape, sub)
                        if isinstance(shape, Part.Face):
                            rtn = False
                            (norm, surf) = self.getFaceNormAndSurf(shape)
                            (rtn, angle, axis, praInfo)= self.pocketRotationAnalysis(obj, norm, surf, prnt=True)
                            PathLog.info("praInfo: \n" + str(praInfo))
                            if rtn is True:
                                (clnBase, angle, clnStock, tag) = self.applyRotationalAnalysis(obj, base, angle, axis, subCount)
                                # Verify faces are correctly oriented - InverseAngle might be necessary
                                faceIA = getattr(clnBase.Shape, sub)
                                (norm, surf) = self.getFaceNormAndSurf(faceIA)
                                (rtn, praAngle, praAxis, praInfo) = self.pocketRotationAnalysis(obj, norm, surf, prnt=False)
                                if rtn is True:
                                    PathLog.error("  --Face not aligned after initial rotation.")
                                    if obj.AttemptInverseAngle is True:
                                        (clnBase, clnStock, angle) = self.applyInverseAngle(obj, clnBase, clnStock, axis, angle)
                                    else:
                                        msg = "Consider toggling the 'InverseAngle' property and recomputing."
                                        PathLog.error(msg)
                                        title = 'Rotation Warning'
                                        self.guiMessage(title, msg, False)
                                else:
                                    PathLog.debug("  --Face appears to be oriented correctly.")

                                tup = clnBase, sub, tag, angle, axis, clnStock
                            else:
                                PathLog.debug(str(sub) + ": no rotation used")
                                axis = 'X'
                                angle = 0.0
                                tag = base.Name + '_' + axis + str(angle).replace('.', '_')
                                stock = PathUtils.findParentJob(obj).Stock
                                tup = base, sub, tag, angle, axis, stock
                            # Eif
                            allTuples.append(tup)
                        # Eif
                    # Efor
                # Efor
                if subCount > 1:
                    msg = "Multiple faces in Base Geometry."
                    msg += "  Depth settings will be applied to all faces."
                    msg = translate("Path", msg)
                    PathLog.error(msg)
                    #title = translate("Path", "Depth Warning")
                    #self.guiMessage(title, msg)
                (Tags, Grps) = self.sortTuplesByIndex(allTuples, 2) # return (TagList, GroupList)
                subList = []
                for o in range(0, len(Tags)):
                    subList = []
                    for (base, sub, tag, angle, axis, stock) in Grps[o]:
                        subList.append(sub)
                    pair = base, subList, angle, axis, stock
                    baseSubsTuples.append(pair)
                # Efor                       
            else:
                PathLog.info("Use Rotation feature(property) is 'Off'.")
                stock = PathUtils.findParentJob(obj).Stock
                for (base, subList) in obj.Base:
                    baseSubsTuples.append((base, subList, 0.0, 'X', stock))

            # for base in obj.Base:
            for (base, subsList, angle, axis, stock) in baseSubsTuples:
                holes = []
                faces = []

                for sub in subsList:
                    shape = getattr(base.Shape, sub)
                    if isinstance(shape, Part.Face):
                        faces.append(shape)
                        if numpy.isclose(abs(shape.normalAt(0, 0).z), 1):  # horizontal face
                            for wire in shape.Wires[1:]:
                                holes.append((base.Shape, wire))
                    else:
                        ignoreSub = base.Name + '.' + sub
                        msg = translate('Path', "Found a selected object which is not a face. Ignoring: {}".format(ignoreSub))
                        PathLog.error(msg)
                        FreeCAD.Console.PrintWarning(msg)
                        # return
                
                for shape, wire in holes:
                    f = Part.makeFace(wire, 'Part::FaceMakerSimple')
                    drillable = PathUtils.isDrillable(shape, wire)
                    if (drillable and obj.processCircles) or (not drillable and obj.processHoles):
                        # Recalculate depthparams
                        (strDep, finDep) = self.calculateStartFinalDepths(obj, shape, stock)
                        PathLog.debug("Adjusted face depths strDep: {}, and finDep: {}".format(self.strDep, self.finDep))
                        finish_step = obj.FinishDepth.Value if hasattr(obj, "FinishDepth") else 0.0
                        self.depthparams = PathUtils.depth_params(
                            clearance_height=obj.ClearanceHeight.Value,
                            safe_height=obj.SafeHeight.Value,
                            start_depth=strDep, #obj.StartDepth.Value,
                            step_down=obj.StepDown.Value,
                            z_finish_step=finish_step,
                            final_depth=finDep, #obj.FinalDepth.Value,
                            user_depths=None)
                        env = PathUtils.getEnvelope(shape, subshape=f, depthparams=self.depthparams)
                        PathLog.track()
                        # shapes.append((env, True))
                        tup = env, True, 'pathProfileFaces', angle, axis, strDep, finDep
                        shapes.append(tup)

                if len(faces) > 0:
                    profileshape = Part.makeCompound(faces)
                    self.profileshape.append(profileshape)

                if obj.processPerimeter:
                    PathLog.track()
                    if profileshape:
                        # Recalculate depthparams
                        (strDep, finDep) = self.calculateStartFinalDepths(obj, profileshape, stock)
                        PathLog.debug("Adjusted face depths strDep: {}, and finDep: {}".format(self.strDep, self.finDep))
                        finish_step = obj.FinishDepth.Value if hasattr(obj, "FinishDepth") else 0.0
                        self.depthparams = PathUtils.depth_params(
                            clearance_height=obj.ClearanceHeight.Value,
                            safe_height=obj.SafeHeight.Value,
                            start_depth=strDep, #obj.StartDepth.Value,
                            step_down=obj.StepDown.Value,
                            z_finish_step=finish_step,
                            final_depth=finDep, #obj.FinalDepth.Value,
                            user_depths=None)
                    else:
                        strDep = obj.StartDepth.Value
                        finDep = obj.FinalDepth.Value
                    try:
                        env = PathUtils.getEnvelope(base.Shape, subshape=profileshape, depthparams=self.depthparams)
                    except Exception as ee:
                        PathLog.error(translate('Path', 'getEnvelope() failed to return an object.'))
                    else:
                        # shapes.append((env, False))
                        tup = env, False, 'pathProfileFaces', angle, axis, strDep, finDep
                        shapes.append(tup)
                else:
                    for shape in faces:
                        # Recalculate depthparams
                        (strDep, finDep) = self.calculateStartFinalDepths(obj, shape, stock)
                        finish_step = obj.FinishDepth.Value if hasattr(obj, "FinishDepth") else 0.0
                        self.depthparams = PathUtils.depth_params(
                            clearance_height=obj.ClearanceHeight.Value,
                            safe_height=obj.SafeHeight.Value,
                            start_depth=strDep, #obj.StartDepth.Value,
                            step_down=obj.StepDown.Value,
                            z_finish_step=finish_step,
                            final_depth=finDep, #obj.FinalDepth.Value,
                            user_depths=None)
                        env = PathUtils.getEnvelope(base.Shape, subshape=shape, depthparams=self.depthparams)
                        tup = env, False, 'pathProfileFaces', angle, axis, strDep, finDep
                        shapes.append(tup)
                # Eif
            # Efor
        else:  # Try to build targets from the job base
            if 1 == len(self.model) and hasattr(self.model[0], "Proxy"):
                if isinstance(self.model[0].Proxy, ArchPanel.PanelSheet):  # process the sheet
                    if obj.processCircles or obj.processHoles:
                        for shape in self.model[0].Proxy.getHoles(self.model[0], transform=True):
                            for wire in shape.Wires:
                                drillable = PathUtils.isDrillable(self.model[0].Proxy, wire)
                                if (drillable and obj.processCircles) or (not drillable and obj.processHoles):
                                    f = Part.makeFace(wire, 'Part::FaceMakerSimple')
                                    env = PathUtils.getEnvelope(self.model[0].Shape, subshape=f, depthparams=self.depthparams)
                                    # shapes.append((env, True))
                                    tup = env, True, 'pathProfileFaces', 0.0, 'X', obj.StartDepth.Value, obj.FinalDepth.Value
                                    shapes.append(tup)

                    if obj.processPerimeter:
                        for shape in self.model[0].Proxy.getOutlines(self.model[0], transform=True):
                            for wire in shape.Wires:
                                f = Part.makeFace(wire, 'Part::FaceMakerSimple')
                                env = PathUtils.getEnvelope(self.model[0].Shape, subshape=f, depthparams=self.depthparams)
                                # shapes.append((env, False))
                                tup = env, False, 'pathProfileFaces', 0.0, 'X', obj.StartDepth.Value, obj.FinalDepth.Value
                                shapes.append(tup)

        self.removalshapes = shapes
        PathLog.debug("%d shapes" % len(shapes))

        return shapes

    def areaOpSetDefaultValues(self, obj, job):
        '''areaOpSetDefaultValues(obj, job) ... sets default values for hole, circle and perimeter processing.'''
        self.baseObject().areaOpSetDefaultValues(obj, job)

        obj.processHoles = False
        obj.processCircles = False
        obj.processPerimeter = True
        obj.ReverseDirection = False
        obj.InverseAngle = False
        obj.AttemptInverseAngle = False
        obj.B_AxisErrorOverride = False


def SetupProperties():
    setup = PathProfileBase.SetupProperties()
    setup.append("processHoles")
    setup.append("processPerimeter")
    setup.append("processCircles")
    setup.append("ReverseDirection")
    setup.append("InverseAngle")
    setup.append("B_AxisErrorOverride")
    setup.append("AttemptInverseAngle")
    return setup

def Create(name, obj = None):
    '''Create(name) ... Creates and returns a Profile based on faces operation.'''
    if obj is None:
        obj = FreeCAD.ActiveDocument.addObject("Path::FeaturePython", name)
    proxy = ObjectProfile(obj, name)
    return obj
