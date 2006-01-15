/***************************************************************************
 *   Copyright (c) 2005 Werner Mayer <werner.wm.mayer@gmx.de>              *
 *                                                                         *
 *   This file is part of the FreeCAD CAx development system.              *
 *                                                                         *
 *   This library is free software; you can redistribute it and/or         *
 *   modify it under the terms of the GNU Library General Public           *
 *   License as published by the Free Software Foundation; either          *
 *   version 2 of the License, or (at your option) any later version.      *
 *                                                                         *
 *   This library  is distributed in the hope that it will be useful,      *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU Library General Public License for more details.                  *
 *                                                                         *
 *   You should have received a copy of the GNU Library General Public     *
 *   License along with this library; see the file COPYING.LIB. If not,    *
 *   write to the Free Software Foundation, Inc., 59 Temple Place,         *
 *   Suite 330, Boston, MA  02111-1307, USA                                *
 *                                                                         *
 ***************************************************************************/


#include "PreCompiled.h"
#ifndef _PreComp_
# include <fcntl.h>
# include <ios>
#endif

#include <Base/Console.h>
#include <Base/Exception.h>
#include <Base/Sequencer.h>
#include "FeatureMeshCurvature.h"
#include "MeshCurvature.h"
#include "MeshAlgos.h"

#include "Core/MeshIO.h"


using namespace Mesh;
using namespace MeshCore;

PROPERTY_SOURCE(Mesh::FeatureMeshCurvature, Mesh::MeshFeature)



FeatureMeshCurvature::FeatureMeshCurvature(void)
{
  ADD_PROPERTY(Source,(0));
}

int FeatureMeshCurvature::execute(void)
{
  MeshFeature *pcFeat  = dynamic_cast<MeshFeature*>(Source.getValue());
  if(!pcFeat || pcFeat->getStatus() != Valid)
    return 1;

  MeshWithProperty& rMesh = pcFeat->getMesh();
  MeshAlgos::calcVertexCurvature( &rMesh );

  return 0;
}

MeshWithProperty& FeatureMeshCurvature::getMesh()
{
  MeshFeature *pcFeat  = dynamic_cast<MeshFeature*>(Source.getValue());
  if(!pcFeat || pcFeat->getStatus() != Valid)
    return MeshFeature::getMesh();

  return pcFeat->getMesh();
}
