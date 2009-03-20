/***************************************************************************
 *   Copyright (c) 2006 Werner Mayer <werner.wm.mayer@gmx.de>              *
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
# ifdef FC_OS_WIN32
# include <windows.h>
# endif
# ifdef FC_OS_MACOSX
# include <OpenGL/gl.h>
# else
# include <GL/gl.h>
# endif
# include <Inventor/actions/SoCallbackAction.h>
# include <Inventor/actions/SoGetBoundingBoxAction.h>
# include <Inventor/actions/SoGetPrimitiveCountAction.h>
# include <Inventor/actions/SoGLRenderAction.h>
# include <Inventor/actions/SoPickAction.h>
# include <Inventor/actions/SoWriteAction.h>
# include <Inventor/caches/SoNormalCache.h>
# include <Inventor/elements/SoCreaseAngleElement.h>
# include <Inventor/elements/SoNormalBindingElement.h>
# include <Inventor/elements/SoNormalElement.h>
# include <Inventor/errors/SoReadError.h>
# include <Inventor/misc/SoState.h>
#endif

#include "SoFCMeshObject.h"
#include <Base/Console.h>
#include <Base/Exception.h>
#include <Gui/SoFCInteractiveElement.h>
#include <Mod/Mesh/App/Core/MeshIO.h>
#include <Mod/Mesh/App/Core/MeshKernel.h>
#include <Mod/Mesh/App/Core/Elements.h>

using namespace MeshGui;


class SoOutputStreambuf : public std::streambuf
{
public:
    SoOutputStreambuf(SoOutput* o) : out(o)
    {
    }
protected:
    int overflow(int c = EOF)
    {
        if (c != EOF) {
            char z = static_cast<char>(c);
            out->write(z);
        }
        return c;
    }
    std::streamsize xsputn (const char* s, std::streamsize num)
    {
        out->write(s);
        return num;
    }

private:
    SoOutput* out;
};

class SoOutputStream : public std::ostream
{
public:
    SoOutputStream(SoOutput* o) : std::ostream(0), buf(o)
    {
        this->rdbuf(&buf);
    }
private:
    SoOutputStreambuf buf;
};

class SoInputStreambuf : public std::streambuf
{
public:
    SoInputStreambuf(SoInput* o) : inp(o)
    {
        setg (buffer+pbSize,
              buffer+pbSize,
              buffer+pbSize);
    }
protected:
    int underflow()
    {
        if (gptr() < egptr()) {
            return *gptr();
        }

        int numPutback;
        numPutback = gptr() - eback();
        if (numPutback > pbSize) {
            numPutback = pbSize;
        }

        memcpy (buffer+(pbSize-numPutback), gptr()-numPutback, numPutback);

        int num=0;
        for (int i=0; i<bufSize; i++) {
            char c;
            SbBool ok = inp->get(c);
            if (ok) {
                num++;
                buffer[pbSize+i] = c;
                if (c == '\n')
                    break;
            }
            else if (num==0) {
                return EOF;
            }
        }

        setg (buffer+(pbSize-numPutback),
              buffer+pbSize,
              buffer+pbSize+num);

        return *gptr();
    }

private:
    static const int pbSize = 4;
    static const int bufSize = 1024;
    char buffer[bufSize+pbSize];
    SoInput* inp;
};

class SoInputStream : public std::istream
{
public:
    SoInputStream(SoInput* o) : std::istream(0), buf(o)
    {
        this->rdbuf(&buf);
    }
    ~SoInputStream()
    {
    }

private:
    SoInputStreambuf buf;
};

// Defines all required member variables and functions for a
// single-value field
SO_SFIELD_SOURCE(SoSFMeshObject, const Mesh::MeshObject*, const Mesh::MeshObject*);


void SoSFMeshObject::initClass()
{
    // This macro takes the name of the class and the name of the
    // parent class
    SO_SFIELD_INIT_CLASS(SoSFMeshObject, SoSField);
}

// This reads the value of a field from a file. It returns FALSE if the value could not be read
// successfully.
SbBool SoSFMeshObject::readValue(SoInput *in)
{
    if (!in->isBinary()) {
        SoInputStream str(in);
        MeshCore::MeshKernel kernel;
        MeshCore::MeshInput(kernel).LoadMeshNode(str);
        value = new Mesh::MeshObject(kernel);

        // We need to trigger the notification chain here, as this function
        // can be used on a node in a scene graph in any state -- not only
        // during initial scene graph import.
        this->valueChanged();

        return TRUE;
    }

    int32_t countPt;
    in->read(countPt);
    std::vector<float> verts(countPt);
    in->readBinaryArray(&(verts[0]),countPt);

    MeshCore::MeshPointArray rPoints;
    rPoints.reserve(countPt/3);
    for (std::vector<float>::iterator it = verts.begin();
        it != verts.end();) {
            Base::Vector3f p;
            p.x = *it; it++;
            p.y = *it; it++;
            p.z = *it; it++;
            rPoints.push_back(p);
    }

    int32_t countFt;
    in->read(countFt);
    std::vector<int32_t> faces(countFt);
    in->readBinaryArray(&(faces[0]),countFt);

    MeshCore::MeshFacetArray rFacets;
    rFacets.reserve(countFt/3);
    for (std::vector<int32_t>::iterator it = faces.begin();
        it != faces.end();) {
            MeshCore::MeshFacet f;
            f._aulPoints[0] = *it; it++;
            f._aulPoints[1] = *it; it++;
            f._aulPoints[2] = *it; it++;
            rFacets.push_back(f);
    }

    MeshCore::MeshKernel kernel;
    kernel.Adopt(rPoints, rFacets, true);
    value = new Mesh::MeshObject(kernel);

    // We need to trigger the notification chain here, as this function
    // can be used on a node in a scene graph in any state -- not only
    // during initial scene graph import.
    this->valueChanged();

    return TRUE;
}

// This writes the value of a field to a file.
void SoSFMeshObject::writeValue(SoOutput *out) const
{
    if (!out->isBinary()) {
        SoOutputStream str(out);
        MeshCore::MeshOutput(value->getKernel()).SaveMeshNode(str);
        return;
    }

    if (!value) {
        int32_t count = 0;
        out->write(count);
        out->write(count);
        return;
    }
    const MeshCore::MeshPointArray& rPoints = value->getKernel().GetPoints();
    std::vector<float> verts;
    verts.reserve(3*rPoints.size());
    for (MeshCore::MeshPointArray::_TConstIterator it = rPoints.begin();
        it != rPoints.end(); ++it) {
        verts.push_back(it->x);
        verts.push_back(it->y);
        verts.push_back(it->z);
    }

    int32_t countPt = (int32_t)verts.size();
    out->write(countPt);
    out->writeBinaryArray(&(verts[0]),countPt);

    const MeshCore::MeshFacetArray& rFacets = value->getKernel().GetFacets();
    std::vector<uint32_t> faces;
    faces.reserve(3*rFacets.size());
    for (MeshCore::MeshFacetArray::_TConstIterator it = rFacets.begin();
        it != rFacets.end(); ++it) {
        faces.push_back((int32_t)it->_aulPoints[0]);
        faces.push_back((int32_t)it->_aulPoints[1]);
        faces.push_back((int32_t)it->_aulPoints[2]);
    }

    int32_t countFt = (int32_t)faces.size();
    out->write(countFt);
    out->writeBinaryArray((const int32_t*)&(faces[0]),countFt);
}

// -------------------------------------------------------

SO_ELEMENT_SOURCE(SoFCMeshObjectElement);

void SoFCMeshObjectElement::initClass()
{
    SO_ELEMENT_INIT_CLASS(SoFCMeshObjectElement, inherited);
}

void SoFCMeshObjectElement::init(SoState * state)
{
    inherited::init(state);
    this->mesh = 0;
}

SoFCMeshObjectElement::~SoFCMeshObjectElement()
{
}

void SoFCMeshObjectElement::set(SoState * const state, SoNode * const node, const Mesh::MeshObject * const mesh)
{
    SoFCMeshObjectElement * elem = (SoFCMeshObjectElement *)
        SoReplacedElement::getElement(state, classStackIndex, node);
    if (elem) {
        elem->mesh = mesh;
        elem->nodeId = node->getNodeId();
    }
}

const Mesh::MeshObject * SoFCMeshObjectElement::get(SoState * const state)
{
    return SoFCMeshObjectElement::getInstance(state)->mesh;
}

const SoFCMeshObjectElement * SoFCMeshObjectElement::getInstance(SoState * state)
{
    return (const SoFCMeshObjectElement *) SoElement::getConstElement(state, classStackIndex);
}

void SoFCMeshObjectElement::print(FILE * /* file */) const
{
}

// -------------------------------------------------------

SO_NODE_SOURCE(SoFCMeshObjectNode);

/*!
  Constructor.
*/
SoFCMeshObjectNode::SoFCMeshObjectNode(void)
{
    SO_NODE_CONSTRUCTOR(SoFCMeshObjectNode);

    SO_NODE_ADD_FIELD(mesh, (0));
}

/*!
  Destructor.
*/
SoFCMeshObjectNode::~SoFCMeshObjectNode()
{
}

// Doc from superclass.
void SoFCMeshObjectNode::initClass(void)
{
    SO_NODE_INIT_CLASS(SoFCMeshObjectNode, SoNode, "Node");

    SO_ENABLE(SoGetBoundingBoxAction, SoFCMeshObjectElement);
    SO_ENABLE(SoGLRenderAction, SoFCMeshObjectElement);
    SO_ENABLE(SoPickAction, SoFCMeshObjectElement);
    SO_ENABLE(SoCallbackAction, SoFCMeshObjectElement);
    SO_ENABLE(SoGetPrimitiveCountAction, SoFCMeshObjectElement);
}

// Doc from superclass.
void SoFCMeshObjectNode::doAction(SoAction * action)
{
    SoFCMeshObjectElement::set(action->getState(), this, mesh.getValue());
}

// Doc from superclass.
void SoFCMeshObjectNode::GLRender(SoGLRenderAction * action)
{
    SoFCMeshObjectNode::doAction(action);
}

// Doc from superclass.
void SoFCMeshObjectNode::callback(SoCallbackAction * action)
{
    SoFCMeshObjectNode::doAction(action);
}

// Doc from superclass.
void SoFCMeshObjectNode::pick(SoPickAction * action)
{
    SoFCMeshObjectNode::doAction(action);
}

// Doc from superclass.
void SoFCMeshObjectNode::getBoundingBox(SoGetBoundingBoxAction * action)
{
    SoFCMeshObjectNode::doAction(action);
}

// Doc from superclass.
void SoFCMeshObjectNode::getPrimitiveCount(SoGetPrimitiveCountAction * action)
{
    SoFCMeshObjectNode::doAction(action);
}

// Helper functions: draw vertices
inline void glVertex(const MeshCore::MeshPoint& _v)  
{
    float v[3];
    v[0]=_v.x; v[1]=_v.y;v[2]=_v.z;
    glVertex3fv(v); 
}

// Helper functions: draw normal
inline void glNormal(const Base::Vector3f& _n)
{
    float n[3];
    n[0]=_n.x; n[1]=_n.y;n[2]=_n.z;
    glNormal3fv(n); 
}

// Helper functions: draw normal
inline void glNormal(float* n)
{
    glNormal3fv(n); 
}

// Helper function: convert Vec to SbVec3f
inline SbVec3f sbvec3f(const Base::Vector3f& _v)
{
    return SbVec3f(_v.x, _v.y, _v.z); 
}

SO_NODE_SOURCE(SoFCMeshObjectShape);

void SoFCMeshObjectShape::initClass()
{
    SO_NODE_INIT_CLASS(SoFCMeshObjectShape, SoVertexShape, "VertexShape");
}

SoFCMeshObjectShape::SoFCMeshObjectShape() : MaximumTriangles(100000), meshChanged(true)
{
    SO_NODE_CONSTRUCTOR(SoFCMeshObjectShape);
    setName(SoFCMeshObjectShape::getClassTypeId().getName());
}

void SoFCMeshObjectShape::notify(SoNotList * node)
{
    inherited::notify(node);
    meshChanged = true;
}

/**
 * Either renders the complete mesh or only a subset of the points.
 */
void SoFCMeshObjectShape::GLRender(SoGLRenderAction *action)
{
    if (shouldGLRender(action))
    {
        SoState*  state = action->getState();

        SbBool mode = Gui::SoFCInteractiveElement::get(state);
        const Mesh::MeshObject * mesh = SoFCMeshObjectElement::get(state);
        if (!mesh || mesh->countPoints() == 0) return;

        Binding mbind = this->findMaterialBinding(state);
        Binding nbind = this->findNormalBinding(state);

        const SbVec3f * normals=0;
        const int32_t * nindices=0;
        SbBool normalCacheUsed=FALSE;

        SoMaterialBundle mb(action);
        //SoTextureCoordinateBundle tb(action, true, false);

        SbBool sendNormals = !mb.isColorOnly()/* || tb.isFunction()*/;
        this->getVertexData(state, normals, nindices, sendNormals, normalCacheUsed);

        if (!sendNormals) nbind = OVERALL;
        else if (nbind == OVERALL) {
            if (normals) glNormal3fv(normals[0].getValue());
            else glNormal3f(0.0f, 0.0f, 1.0f);
        }
        else if (normalCacheUsed && nbind == PER_VERTEX) {
            nbind = PER_VERTEX_INDEXED;
        }
        else if (normalCacheUsed && nbind == PER_FACE_INDEXED) {
            nbind = PER_FACE;
        }

        mb.sendFirst();  // make sure we have the correct material
    
        SbBool ccw = TRUE;
        if (SoShapeHintsElement::getVertexOrdering(state) == SoShapeHintsElement::CLOCKWISE) 
            ccw = FALSE;

        if (mode == false || mesh->countFacets() <= this->MaximumTriangles) {
            if (mbind != OVERALL)
                drawFaces(mesh, &mb, mbind, normals, sendNormals, ccw);
            else
                drawFaces(mesh, 0, mbind, normals, sendNormals, ccw);
        }
        else {
            drawPoints(mesh, sendNormals, ccw);
        }

        if (normalCacheUsed) {
            this->readUnlockNormalCache();
        }

        // Disable caching for this node
        SoGLCacheContextElement::shouldAutoCache(state, SoGLCacheContextElement::DONT_AUTO_CACHE);
    }
}

/**
 * Translates current material binding into the internal Binding enum.
 */
SoFCMeshObjectShape::Binding
SoFCMeshObjectShape::findMaterialBinding(SoState * const state) const
{
    Binding binding = OVERALL;
    SoMaterialBindingElement::Binding matbind = SoMaterialBindingElement::get(state);

    switch (matbind) {
    case SoMaterialBindingElement::OVERALL:
        binding = OVERALL;
        break;
    case SoMaterialBindingElement::PER_VERTEX:
        binding = PER_VERTEX_INDEXED;
        break;
    case SoMaterialBindingElement::PER_VERTEX_INDEXED:
        binding = PER_VERTEX_INDEXED;
        break;
    case SoMaterialBindingElement::PER_PART:
    case SoMaterialBindingElement::PER_FACE:
        binding = PER_FACE_INDEXED;
        break;
    case SoMaterialBindingElement::PER_PART_INDEXED:
    case SoMaterialBindingElement::PER_FACE_INDEXED:
        binding = PER_FACE_INDEXED;
        break;
    default:
        break;
    }
    return binding;
}

/**
 * translates current normal binding into the internal Binding enum.
 */
SoFCMeshObjectShape::Binding
SoFCMeshObjectShape::findNormalBinding(SoState * const state) const
{
    Binding binding = PER_VERTEX_INDEXED;
    SoNormalBindingElement::Binding normbind =
        (SoNormalBindingElement::Binding) SoNormalBindingElement::get(state);

    switch (normbind) {
    case SoNormalBindingElement::OVERALL:
        binding = OVERALL;
        break;
    case SoNormalBindingElement::PER_VERTEX:
        binding = PER_VERTEX;
        break;
    case SoNormalBindingElement::PER_VERTEX_INDEXED:
        binding = PER_VERTEX_INDEXED;
        break;
    case SoNormalBindingElement::PER_PART:
    case SoNormalBindingElement::PER_FACE:
        binding = PER_FACE;
        break;
    case SoNormalBindingElement::PER_PART_INDEXED:
    case SoNormalBindingElement::PER_FACE_INDEXED:
        binding = PER_FACE_INDEXED;
        break;
    default:
        break;
    }
    return binding;
}

SbBool SoFCMeshObjectShape::generateDefaultNormals(SoState * state, SoNormalBundle * bundle)
{
    return FALSE;
}

SbBool SoFCMeshObjectShape::generateDefaultNormals(SoState * state, SoNormalCache * cache)
{
    SbBool ccw = TRUE;
    if (SoShapeHintsElement::getVertexOrdering(state) ==
        SoShapeHintsElement::CLOCKWISE) ccw = FALSE;

    const Mesh::MeshObject * mesh = SoFCMeshObjectElement::get(state);
    assert(mesh);

    SoNormalBindingElement::Binding normbind =
        SoNormalBindingElement::get(state);

    switch (normbind) {
    case SoNormalBindingElement::PER_VERTEX:
    case SoNormalBindingElement::PER_VERTEX_INDEXED:
        {/*
        cache->generatePerVertex(&(coords[0]),
                                 &(coordIndex[0]),
                                 coordIndex.size(),
                                 SoCreaseAngleElement::get(state, this->getNodeType() == SoNode::VRML1),
                                 NULL,
                                 ccw);*/
            SbVec3f* n = generatePerVertexNormals(mesh, 
                SoCreaseAngleElement::get(state, this->getNodeType() == SoNode::VRML1), ccw);
            cache->set(mesh->countPoints(), n);
        }
        break;
    case SoNormalBindingElement::PER_FACE:
    case SoNormalBindingElement::PER_FACE_INDEXED:
    case SoNormalBindingElement::PER_PART:
    case SoNormalBindingElement::PER_PART_INDEXED:
        /*cache->generatePerFace(&(coords[0]),
                               &(coordIndex[0]),
                               coordIndex.size(),
                               ccw);*/
        break;
    default:
        break;
    }
    return TRUE;
}

namespace MeshGui {

template<typename T>
T clamp(T value, T min, T max)
{
    return std::max<T>(min, std::min<T>(max, value));
}

static void
calc_normal_vec(const MeshCore::MeshKernel& mesh, unsigned long facenum, 
                std::vector<unsigned long> & faceArray, 
                float threshold, Base::Vector3f & vertnormal)
{
    // start with face normal vector
    Base::Vector3f facenormal = mesh.GetFacet(facenum).GetNormal();
    vertnormal = facenormal;

    unsigned long n = faceArray.size();
    unsigned long currface;

    for (unsigned long i = 0; i < n; i++) {
        currface = faceArray[i];
        if (currface != facenum) { // check all but this face
            Base::Vector3f normal = mesh.GetFacet(currface).GetNormal();  // everything is ok
            if ((normal * facenormal) > threshold) {
                // smooth towards this face
                vertnormal += normal;
            }
        }
    }
}
}

SbVec3f* SoFCMeshObjectShape::generatePerVertexNormals(const Mesh::MeshObject * mesh,
                                                       const float crease_angle,
                                                       const SbBool ccw) const
{
    std::vector<std::vector<unsigned long> > vertexFaceArray(mesh->countPoints());
    const MeshCore::MeshPointArray& p = mesh->getKernel().GetPoints();
    const MeshCore::MeshFacetArray& f = mesh->getKernel().GetFacets();
    unsigned long facenum = 0;
    for (MeshCore::MeshFacetArray::_TConstIterator it = f.begin(); it != f.end(); ++it) {
        vertexFaceArray[it->_aulPoints[0]].push_back(facenum);
        vertexFaceArray[it->_aulPoints[1]].push_back(facenum);
        vertexFaceArray[it->_aulPoints[2]].push_back(facenum);
        facenum++;
    }

    float threshold = (float)cos(MeshGui::clamp<float>(crease_angle, 0.0f, (float)M_PI));
    SbVec3f* normals = new SbVec3f[mesh->countPoints()];
    facenum = 0;
    Base::Vector3f normal;
    for (MeshCore::MeshFacetArray::_TConstIterator it = f.begin(); it != f.end(); ++it) {
        for (int i=0; i<3; i++) {
            calc_normal_vec(mesh->getKernel(), facenum, vertexFaceArray[it->_aulPoints[i]],
                            threshold, normal);
        }
    }

#if 0
    for (std::vector<std::vector<unsigned long> >::iterator it = 
        vertexFaceArray.begin(); it != vertexFaceArray.end(); ++it) {
        Base::Vector3f normal;
        calc_normal_vec(mesh->getKernel(), facenum, *it, threshold, normal);
        //Base::Vector3f facenormal;
        //for (std::vector<unsigned long>::iterator jt = it->begin(); jt != it->end(); ++jt) {
        //    Base::Vector3f n = mesh->getKernel().GetFacet(f[*jt]).GetNormal();
        //    if (jt == it->begin()) {
        //        facenormal = n;
        //        normal = SbVec3f(n.x,n.y,n.z);
        //    }
        //    else if (facenormal * n > threshold) {
        //        normal += SbVec3f(n.x,n.y,n.z);
        //    }
        //}
        normal.Normalize();
        normals[facenum++].setValue(normal.x,normal.y,normal.z);
    }
#endif

    return normals;
}

/**
 * Convenience method that returns the current coordinate and normal
 * element.
 */
SbBool SoFCMeshObjectShape::getVertexData(SoState * state,
                                          const SbVec3f *& normals,
                                          const int32_t *& nindices,
                                          const SbBool needNormals,
                                          SbBool & normalCacheUsed)
{
#if 1
    normals = 0;
    if (needNormals) {
        normals = SoNormalElement::getInstance(state)->getArrayPtr();
    }

    normalCacheUsed = FALSE;
    nindices = NULL;
    if (needNormals) {
//        nindices = this->normalIndex.getValues(0);
//        if (this->normalIndex.getNum() <= 0 || nindices[0] < 0) nindices = NULL;

        if (normals == NULL) {
            SoNormalCache * nc = this->generateAndReadLockNormalCache(state);
            normals = nc->getNormals();
            nindices = nc->getIndices();
            normalCacheUsed = TRUE;

            // if no normals were generated, unlock normal cache before
            // returning
            if (normals == NULL) {
                this->readUnlockNormalCache();
                normalCacheUsed = FALSE;
            }
        }
    }
#endif
    return TRUE;
}

/**
 * Renders the triangles of the complete mesh.
 * FIXME: Do it the same way as Coin did to have only one implementation which is controled by defines
 * FIXME: Implement using different values of transparency for each vertex or face
 */
void SoFCMeshObjectShape::drawFaces(const Mesh::MeshObject * mesh, SoMaterialBundle* mb,
                                    Binding bind, const SbVec3f * normals,
                                    SbBool needNormals, SbBool ccw) const
{
    const MeshCore::MeshPointArray & rPoints = mesh->getKernel().GetPoints();
    const MeshCore::MeshFacetArray & rFacets = mesh->getKernel().GetFacets();
    bool perVertex = (mb && bind == PER_VERTEX_INDEXED);
    bool perFace = (mb && bind == PER_FACE_INDEXED);

    if (needNormals)
    {
        glBegin(GL_TRIANGLES);
        if (ccw) {
            // counterclockwise ordering
            for (MeshCore::MeshFacetArray::_TConstIterator it = rFacets.begin(); it != rFacets.end(); ++it)
            {
                const MeshCore::MeshPoint& v0 = rPoints[it->_aulPoints[0]];
                const MeshCore::MeshPoint& v1 = rPoints[it->_aulPoints[1]];
                const MeshCore::MeshPoint& v2 = rPoints[it->_aulPoints[2]];

                // Calculate the normal n = (v1-v0)x(v2-v0)
                float n[3];
                n[0] = (v1.y-v0.y)*(v2.z-v0.z)-(v1.z-v0.z)*(v2.y-v0.y);
                n[1] = (v1.z-v0.z)*(v2.x-v0.x)-(v1.x-v0.x)*(v2.z-v0.z);
                n[2] = (v1.x-v0.x)*(v2.y-v0.y)-(v1.y-v0.y)*(v2.x-v0.x);
    
                if(perFace)
                mb->send(it-rFacets.begin(), TRUE);
                glNormal(n);
                if(perVertex)
                mb->send(it->_aulPoints[0], TRUE);
                //glNormal3fv(normals[it->_aulPoints[0]].getValue());
                glVertex(v0);
                if(perVertex)
                mb->send(it->_aulPoints[1], TRUE);
                //glNormal3fv(normals[it->_aulPoints[1]].getValue());
                glVertex(v1);
                if(perVertex)
                mb->send(it->_aulPoints[2], TRUE);
                //glNormal3fv(normals[it->_aulPoints[2]].getValue());
                glVertex(v2);
            }
        }
        else {
            // clockwise ordering
            for (MeshCore::MeshFacetArray::_TConstIterator it = rFacets.begin(); it != rFacets.end(); ++it)
            {
                const MeshCore::MeshPoint& v0 = rPoints[it->_aulPoints[0]];
                const MeshCore::MeshPoint& v1 = rPoints[it->_aulPoints[1]];
                const MeshCore::MeshPoint& v2 = rPoints[it->_aulPoints[2]];

                // Calculate the normal n = -(v1-v0)x(v2-v0)
                float n[3];
                n[0] = -((v1.y-v0.y)*(v2.z-v0.z)-(v1.z-v0.z)*(v2.y-v0.y));
                n[1] = -((v1.z-v0.z)*(v2.x-v0.x)-(v1.x-v0.x)*(v2.z-v0.z));
                n[2] = -((v1.x-v0.x)*(v2.y-v0.y)-(v1.y-v0.y)*(v2.x-v0.x));

                glNormal(n);
                glVertex(v0);
                glVertex(v1);
                glVertex(v2);
            }
        }
        glEnd();
    }
    else {
        glBegin(GL_TRIANGLES);
        for (MeshCore::MeshFacetArray::_TConstIterator it = rFacets.begin(); it != rFacets.end(); ++it)
        {
            glVertex(rPoints[it->_aulPoints[0]]);
            glVertex(rPoints[it->_aulPoints[1]]);
            glVertex(rPoints[it->_aulPoints[2]]);
        }
        glEnd();
    }
}

/**
 * Renders the gravity points of a subset of triangles.
 */
void SoFCMeshObjectShape::drawPoints(const Mesh::MeshObject * mesh, SbBool needNormals, SbBool ccw) const
{
    const MeshCore::MeshPointArray & rPoints = mesh->getKernel().GetPoints();
    const MeshCore::MeshFacetArray & rFacets = mesh->getKernel().GetFacets();
    int mod = rFacets.size()/MaximumTriangles+1;

    float size = std::min<float>((float)mod,3.0f);
    glPointSize(size);

    if (needNormals)
    {
        glBegin(GL_POINTS);
        int ct=0;
        if (ccw) {
            for (MeshCore::MeshFacetArray::_TConstIterator it = rFacets.begin(); it != rFacets.end(); ++it, ct++)
            {
                if (ct%mod==0) {
                    const MeshCore::MeshPoint& v0 = rPoints[it->_aulPoints[0]];
                    const MeshCore::MeshPoint& v1 = rPoints[it->_aulPoints[1]];
                    const MeshCore::MeshPoint& v2 = rPoints[it->_aulPoints[2]];

                    // Calculate the normal n = (v1-v0)x(v2-v0)
                    float n[3];
                    n[0] = (v1.y-v0.y)*(v2.z-v0.z)-(v1.z-v0.z)*(v2.y-v0.y);
                    n[1] = (v1.z-v0.z)*(v2.x-v0.x)-(v1.x-v0.x)*(v2.z-v0.z);
                    n[2] = (v1.x-v0.x)*(v2.y-v0.y)-(v1.y-v0.y)*(v2.x-v0.x);

                    // Calculate the center point p=(v0+v1+v2)/3
                    float p[3];
                    p[0] = (v0.x+v1.x+v2.x)/3.0f;
                    p[1] = (v0.y+v1.y+v2.y)/3.0f;
                    p[2] = (v0.z+v1.z+v2.z)/3.0f;
                    glNormal3fv(n);
                    glVertex3fv(p);
                }
            }
        }
        else {
            for (MeshCore::MeshFacetArray::_TConstIterator it = rFacets.begin(); it != rFacets.end(); ++it, ct++)
            {
                if (ct%mod==0) {
                    const MeshCore::MeshPoint& v0 = rPoints[it->_aulPoints[0]];
                    const MeshCore::MeshPoint& v1 = rPoints[it->_aulPoints[1]];
                    const MeshCore::MeshPoint& v2 = rPoints[it->_aulPoints[2]];

                    // Calculate the normal n = -(v1-v0)x(v2-v0)
                    float n[3];
                    n[0] = -((v1.y-v0.y)*(v2.z-v0.z)-(v1.z-v0.z)*(v2.y-v0.y));
                    n[1] = -((v1.z-v0.z)*(v2.x-v0.x)-(v1.x-v0.x)*(v2.z-v0.z));
                    n[2] = -((v1.x-v0.x)*(v2.y-v0.y)-(v1.y-v0.y)*(v2.x-v0.x));
      
                    // Calculate the center point p=(v0+v1+v2)/3
                    float p[3];
                    p[0] = (v0.x+v1.x+v2.x)/3.0f;
                    p[1] = (v0.y+v1.y+v2.y)/3.0f;
                    p[2] = (v0.z+v1.z+v2.z)/3.0f;
                    glNormal3fv(n);
                    glVertex3fv(p);
                }
            }
        }
        glEnd();
    }
    else {
        glBegin(GL_POINTS);
        int ct=0;
        for (MeshCore::MeshFacetArray::_TConstIterator it = rFacets.begin(); it != rFacets.end(); ++it, ct++)
        {
            if (ct%mod==0) {
                const MeshCore::MeshPoint& v0 = rPoints[it->_aulPoints[0]];
                const MeshCore::MeshPoint& v1 = rPoints[it->_aulPoints[1]];
                const MeshCore::MeshPoint& v2 = rPoints[it->_aulPoints[2]];
                // Calculate the center point p=(v0+v1+v2)/3
                float p[3];
                p[0] = (v0.x+v1.x+v2.x)/3.0f;
                p[1] = (v0.y+v1.y+v2.y)/3.0f;
                p[2] = (v0.z+v1.z+v2.z)/3.0f;
                glVertex3fv(p);
            }
        }
        glEnd();
    }
}

// test bbox intersection
//static SbBool
//SoFCMeshObjectShape_ray_intersect(SoRayPickAction * action, const SbBox3f & box)
//{
//    if (box.isEmpty()) return FALSE;
//    return action->intersect(box, TRUE);
//}

/**
 * Calculates picked point based on primitives generated by subclasses.
 */
void
SoFCMeshObjectShape::rayPick(SoRayPickAction * action)
{
    //if (this->shouldRayPick(action)) {
    //    this->computeObjectSpaceRay(action);

    //    const SoBoundingBoxCache* bboxcache = getBoundingBoxCache();
    //    if (!bboxcache || !bboxcache->isValid(action->getState()) ||
    //        SoFCMeshObjectShape_ray_intersect(action, bboxcache->getProjectedBox())) {
    //        this->generatePrimitives(action);
    //    }
    //}
    inherited::rayPick(action);
}

/** Sets the point indices, the geometric points and the normal for each triangle.
 * If the number of triangles exceeds \a MaximumTriangles then only a triangulation of
 * a rough model is filled in instead. This is due to performance issues.
 * \see createTriangleDetail().
 */
void SoFCMeshObjectShape::generatePrimitives(SoAction* action)
{
    SoState*  state = action->getState();
    const Mesh::MeshObject* mesh = SoFCMeshObjectElement::get(state);
    if (!mesh)
        return;
    const MeshCore::MeshPointArray & rPoints = mesh->getKernel().GetPoints();
    const MeshCore::MeshFacetArray & rFacets = mesh->getKernel().GetFacets();
    if (rPoints.size() < 3)
        return;
    if (rFacets.size() < 1)
        return;

    // get material binding
    Binding mbind = this->findMaterialBinding(state);

    // Create the information when moving over or picking into the scene
    SoPrimitiveVertex vertex;
    SoPointDetail pointDetail;
    SoFaceDetail faceDetail;

    vertex.setDetail(&pointDetail);

    beginShape(action, TRIANGLES, &faceDetail);
    try 
    {
        for (MeshCore::MeshFacetArray::_TConstIterator it = rFacets.begin(); it != rFacets.end(); ++it)
        {
            const MeshCore::MeshPoint& v0 = rPoints[it->_aulPoints[0]];
            const MeshCore::MeshPoint& v1 = rPoints[it->_aulPoints[1]];
            const MeshCore::MeshPoint& v2 = rPoints[it->_aulPoints[2]];

            // Calculate the normal n = (v1-v0)x(v2-v0)
            SbVec3f n;
            n[0] = (v1.y-v0.y)*(v2.z-v0.z)-(v1.z-v0.z)*(v2.y-v0.y);
            n[1] = (v1.z-v0.z)*(v2.x-v0.x)-(v1.x-v0.x)*(v2.z-v0.z);
            n[2] = (v1.x-v0.x)*(v2.y-v0.y)-(v1.y-v0.y)*(v2.x-v0.x);

            // Set the normal
            vertex.setNormal(n);

            // Vertex 0
            if (mbind == PER_VERTEX_INDEXED || mbind == PER_FACE_INDEXED) {
                pointDetail.setMaterialIndex(it->_aulPoints[0]);
                vertex.setMaterialIndex(it->_aulPoints[0]);
            }
            pointDetail.setCoordinateIndex(it->_aulPoints[0]);
            vertex.setPoint(sbvec3f(v0));
            shapeVertex(&vertex);

            // Vertex 1
            if (mbind == PER_VERTEX_INDEXED || mbind == PER_FACE_INDEXED) {
                pointDetail.setMaterialIndex(it->_aulPoints[1]);
                vertex.setMaterialIndex(it->_aulPoints[1]);
            }
            pointDetail.setCoordinateIndex(it->_aulPoints[1]);
            vertex.setPoint(sbvec3f(v1));
            shapeVertex(&vertex);

            // Vertex 2
            if (mbind == PER_VERTEX_INDEXED || mbind == PER_FACE_INDEXED) {
                pointDetail.setMaterialIndex(it->_aulPoints[2]);
                vertex.setMaterialIndex(it->_aulPoints[2]);
            }
            pointDetail.setCoordinateIndex(it->_aulPoints[2]);
            vertex.setPoint(sbvec3f(v2));
            shapeVertex(&vertex);

            // Increment for the next face
            faceDetail.incFaceIndex();
        }
    }
    catch (const Base::MemoryException&) {
        Base::Console().Log("Not enough memory to generate primitives\n");
    }

    endShape();
}

/**
 * If the number of triangles exceeds \a MaximumTriangles 0 is returned. This means that the client programmer needs to implement itself to get the
 * index of the picked triangle. If the number of triangles doesn't exceed \a MaximumTriangles SoShape::createTriangleDetail() gets called.
 * Against the default OpenInventor implementation which returns 0 as well Coin3d fills in the point and face indices.
 */
SoDetail * SoFCMeshObjectShape::createTriangleDetail(SoRayPickAction * action,
                                              const SoPrimitiveVertex * v1,
                                              const SoPrimitiveVertex * v2,
                                              const SoPrimitiveVertex * v3,
                                              SoPickedPoint * pp)
{
    SoDetail* detail = inherited::createTriangleDetail(action, v1, v2, v3, pp);
    return detail;
}

/**
 * Sets the bounding box of the mesh to \a box and its center to \a center.
 */
void SoFCMeshObjectShape::computeBBox(SoAction *action, SbBox3f &box, SbVec3f &center)
{
    SoState*  state = action->getState();
    const Mesh::MeshObject * mesh = SoFCMeshObjectElement::get(state);
    if (mesh && mesh->countPoints() > 0) {
        Base::BoundBox3f cBox = mesh->getKernel().GetBoundBox();
        box.setBounds(SbVec3f(cBox.MinX,cBox.MinY,cBox.MinZ),
                      SbVec3f(cBox.MaxX,cBox.MaxY,cBox.MaxZ));
        Base::Vector3f mid = cBox.CalcCenter();
        center.setValue(mid.x,mid.y,mid.z);
    }
    else {
        box.setBounds(SbVec3f(0,0,0), SbVec3f(0,0,0));
        center.setValue(0.0f,0.0f,0.0f);
    }
}

/**
 * Adds the number of the triangles to the \a SoGetPrimitiveCountAction.
 */
void SoFCMeshObjectShape::getPrimitiveCount(SoGetPrimitiveCountAction * action)
{
    if (!this->shouldPrimitiveCount(action)) return;
    SoState*  state = action->getState();
    const Mesh::MeshObject * mesh = SoFCMeshObjectElement::get(state);
    action->addNumTriangles(mesh->countFacets());
    action->addNumPoints(mesh->countPoints());
}

/**
 * Counts the number of triangles. If a mesh is not set yet it returns 0.
 */
unsigned int SoFCMeshObjectShape::countTriangles(SoAction * action) const
{
    SoState*  state = action->getState();
    const Mesh::MeshObject * mesh = SoFCMeshObjectElement::get(state);
    return (unsigned int)mesh->countFacets();
}

// -------------------------------------------------------

SO_NODE_SOURCE(SoFCMeshSegmentShape);

void SoFCMeshSegmentShape::initClass()
{
    SO_NODE_INIT_CLASS(SoFCMeshSegmentShape, SoShape, "Shape");
}

SoFCMeshSegmentShape::SoFCMeshSegmentShape() : MaximumTriangles(100000)
{
    SO_NODE_CONSTRUCTOR(SoFCMeshSegmentShape);
    SO_NODE_ADD_FIELD(index, (0));
}

/**
 * Either renders the complete mesh or only a subset of the points.
 */
void SoFCMeshSegmentShape::GLRender(SoGLRenderAction *action)
{
    if (shouldGLRender(action))
    {
        SoState*  state = action->getState();

        SbBool mode = Gui::SoFCInteractiveElement::get(state);
        const Mesh::MeshObject * mesh = SoFCMeshObjectElement::get(state);
        if (!mesh) return;

        Binding mbind = this->findMaterialBinding(state);

        SoMaterialBundle mb(action);
        //SoTextureCoordinateBundle tb(action, true, false);

        SbBool needNormals = !mb.isColorOnly()/* || tb.isFunction()*/;
        mb.sendFirst();  // make sure we have the correct material
    
        SbBool ccw = TRUE;
        if (SoShapeHintsElement::getVertexOrdering(state) == SoShapeHintsElement::CLOCKWISE) 
            ccw = FALSE;

        if (mode == false || mesh->countFacets() <= this->MaximumTriangles) {
            if (mbind != OVERALL)
                drawFaces(mesh, &mb, mbind, needNormals, ccw);
            else
                drawFaces(mesh, 0, mbind, needNormals, ccw);
        }
        else {
            drawPoints(mesh, needNormals, ccw);
        }

        // Disable caching for this node
        SoGLCacheContextElement::shouldAutoCache(state, SoGLCacheContextElement::DONT_AUTO_CACHE);
    }
}

/**
 * Translates current material binding into the internal Binding enum.
 */
SoFCMeshSegmentShape::Binding SoFCMeshSegmentShape::findMaterialBinding(SoState * const state) const
{
    Binding binding = OVERALL;
    SoMaterialBindingElement::Binding matbind = SoMaterialBindingElement::get(state);

    switch (matbind) {
    case SoMaterialBindingElement::OVERALL:
        binding = OVERALL;
        break;
    case SoMaterialBindingElement::PER_VERTEX:
        binding = PER_VERTEX_INDEXED;
        break;
    case SoMaterialBindingElement::PER_VERTEX_INDEXED:
        binding = PER_VERTEX_INDEXED;
        break;
    case SoMaterialBindingElement::PER_PART:
    case SoMaterialBindingElement::PER_FACE:
        binding = PER_FACE_INDEXED;
        break;
    case SoMaterialBindingElement::PER_PART_INDEXED:
    case SoMaterialBindingElement::PER_FACE_INDEXED:
        binding = PER_FACE_INDEXED;
        break;
    default:
        break;
    }
    return binding;
}

/**
 * Renders the triangles of the complete mesh.
 * FIXME: Do it the same way as Coin did to have only one implementation which is controled by defines
 * FIXME: Implement using different values of transparency for each vertex or face
 */
void SoFCMeshSegmentShape::drawFaces(const Mesh::MeshObject * mesh, SoMaterialBundle* mb,
                                    Binding bind, SbBool needNormals, SbBool ccw) const
{
    const MeshCore::MeshPointArray & rPoints = mesh->getKernel().GetPoints();
    const MeshCore::MeshFacetArray & rFacets = mesh->getKernel().GetFacets();
    if (mesh->countSegments() <= this->index.getValue())
        return;
    const std::vector<unsigned long> rSegm = mesh->getSegment
        (this->index.getValue()).getIndices();
    bool perVertex = (mb && bind == PER_VERTEX_INDEXED);
    bool perFace = (mb && bind == PER_FACE_INDEXED);

    if (needNormals)
    {
        glBegin(GL_TRIANGLES);
        if (ccw) {
            // counterclockwise ordering
            for (std::vector<unsigned long>::const_iterator it = rSegm.begin(); it != rSegm.end(); ++it)
            {
                const MeshCore::MeshFacet& f = rFacets[*it];
                const MeshCore::MeshPoint& v0 = rPoints[f._aulPoints[0]];
                const MeshCore::MeshPoint& v1 = rPoints[f._aulPoints[1]];
                const MeshCore::MeshPoint& v2 = rPoints[f._aulPoints[2]];

                // Calculate the normal n = (v1-v0)x(v2-v0)
                float n[3];
                n[0] = (v1.y-v0.y)*(v2.z-v0.z)-(v1.z-v0.z)*(v2.y-v0.y);
                n[1] = (v1.z-v0.z)*(v2.x-v0.x)-(v1.x-v0.x)*(v2.z-v0.z);
                n[2] = (v1.x-v0.x)*(v2.y-v0.y)-(v1.y-v0.y)*(v2.x-v0.x);
    
                if(perFace)
                mb->send(*it, TRUE);
                glNormal(n);
                if(perVertex)
                mb->send(f._aulPoints[0], TRUE);
                glVertex(v0);
                if(perVertex)
                mb->send(f._aulPoints[1], TRUE);
                glVertex(v1);
                if(perVertex)
                mb->send(f._aulPoints[2], TRUE);
                glVertex(v2);
            }
        }
        else {
            // clockwise ordering
            for (std::vector<unsigned long>::const_iterator it = rSegm.begin(); it != rSegm.end(); ++it)
            {
                const MeshCore::MeshFacet& f = rFacets[*it];
                const MeshCore::MeshPoint& v0 = rPoints[f._aulPoints[0]];
                const MeshCore::MeshPoint& v1 = rPoints[f._aulPoints[1]];
                const MeshCore::MeshPoint& v2 = rPoints[f._aulPoints[2]];

                // Calculate the normal n = -(v1-v0)x(v2-v0)
                float n[3];
                n[0] = -((v1.y-v0.y)*(v2.z-v0.z)-(v1.z-v0.z)*(v2.y-v0.y));
                n[1] = -((v1.z-v0.z)*(v2.x-v0.x)-(v1.x-v0.x)*(v2.z-v0.z));
                n[2] = -((v1.x-v0.x)*(v2.y-v0.y)-(v1.y-v0.y)*(v2.x-v0.x));

                glNormal(n);
                glVertex(v0);
                glVertex(v1);
                glVertex(v2);
            }
        }
        glEnd();
    }
    else {
        glBegin(GL_TRIANGLES);
        for (std::vector<unsigned long>::const_iterator it = rSegm.begin(); it != rSegm.end(); ++it)
        {
            const MeshCore::MeshFacet& f = rFacets[*it];
            glVertex(rPoints[f._aulPoints[0]]);
            glVertex(rPoints[f._aulPoints[1]]);
            glVertex(rPoints[f._aulPoints[2]]);
        }
        glEnd();
    }
}

/**
 * Renders the gravity points of a subset of triangles.
 */
void SoFCMeshSegmentShape::drawPoints(const Mesh::MeshObject * mesh, SbBool needNormals, SbBool ccw) const
{
    const MeshCore::MeshPointArray & rPoints = mesh->getKernel().GetPoints();
    const MeshCore::MeshFacetArray & rFacets = mesh->getKernel().GetFacets();
    if (mesh->countSegments() <= this->index.getValue())
        return;
    const std::vector<unsigned long> rSegm = mesh->getSegment
        (this->index.getValue()).getIndices();
    int mod = rSegm.size()/MaximumTriangles+1;

    float size = std::min<float>((float)mod,3.0f);
    glPointSize(size);

    if (needNormals)
    {
        glBegin(GL_POINTS);
        int ct=0;
        if (ccw) {
            for (std::vector<unsigned long>::const_iterator it = rSegm.begin(); it != rSegm.end(); ++it, ct++)
            {
                if (ct%mod==0) {
                    const MeshCore::MeshFacet& f = rFacets[*it];
                    const MeshCore::MeshPoint& v0 = rPoints[f._aulPoints[0]];
                    const MeshCore::MeshPoint& v1 = rPoints[f._aulPoints[1]];
                    const MeshCore::MeshPoint& v2 = rPoints[f._aulPoints[2]];

                    // Calculate the normal n = (v1-v0)x(v2-v0)
                    float n[3];
                    n[0] = (v1.y-v0.y)*(v2.z-v0.z)-(v1.z-v0.z)*(v2.y-v0.y);
                    n[1] = (v1.z-v0.z)*(v2.x-v0.x)-(v1.x-v0.x)*(v2.z-v0.z);
                    n[2] = (v1.x-v0.x)*(v2.y-v0.y)-(v1.y-v0.y)*(v2.x-v0.x);

                    // Calculate the center point p=(v0+v1+v2)/3
                    float p[3];
                    p[0] = (v0.x+v1.x+v2.x)/3.0f;
                    p[1] = (v0.y+v1.y+v2.y)/3.0f;
                    p[2] = (v0.z+v1.z+v2.z)/3.0f;
                    glNormal3fv(n);
                    glVertex3fv(p);
                }
            }
        }
        else {
            for (std::vector<unsigned long>::const_iterator it = rSegm.begin(); it != rSegm.end(); ++it, ct++)
            {
                if (ct%mod==0) {
                    const MeshCore::MeshFacet& f = rFacets[*it];
                    const MeshCore::MeshPoint& v0 = rPoints[f._aulPoints[0]];
                    const MeshCore::MeshPoint& v1 = rPoints[f._aulPoints[1]];
                    const MeshCore::MeshPoint& v2 = rPoints[f._aulPoints[2]];

                    // Calculate the normal n = -(v1-v0)x(v2-v0)
                    float n[3];
                    n[0] = -((v1.y-v0.y)*(v2.z-v0.z)-(v1.z-v0.z)*(v2.y-v0.y));
                    n[1] = -((v1.z-v0.z)*(v2.x-v0.x)-(v1.x-v0.x)*(v2.z-v0.z));
                    n[2] = -((v1.x-v0.x)*(v2.y-v0.y)-(v1.y-v0.y)*(v2.x-v0.x));
      
                    // Calculate the center point p=(v0+v1+v2)/3
                    float p[3];
                    p[0] = (v0.x+v1.x+v2.x)/3.0f;
                    p[1] = (v0.y+v1.y+v2.y)/3.0f;
                    p[2] = (v0.z+v1.z+v2.z)/3.0f;
                    glNormal3fv(n);
                    glVertex3fv(p);
                }
            }
        }
        glEnd();
    }
    else {
        glBegin(GL_POINTS);
        int ct=0;
        for (std::vector<unsigned long>::const_iterator it = rSegm.begin(); it != rSegm.end(); ++it, ct++)
        {
            if (ct%mod==0) {
                const MeshCore::MeshFacet& f = rFacets[*it];
                const MeshCore::MeshPoint& v0 = rPoints[f._aulPoints[0]];
                const MeshCore::MeshPoint& v1 = rPoints[f._aulPoints[1]];
                const MeshCore::MeshPoint& v2 = rPoints[f._aulPoints[2]];
                // Calculate the center point p=(v0+v1+v2)/3
                float p[3];
                p[0] = (v0.x+v1.x+v2.x)/3.0f;
                p[1] = (v0.y+v1.y+v2.y)/3.0f;
                p[2] = (v0.z+v1.z+v2.z)/3.0f;
                glVertex3fv(p);
            }
        }
        glEnd();
    }
}

/** Sets the point indices, the geometric points and the normal for each triangle.
 * If the number of triangles exceeds \a MaximumTriangles then only a triangulation of
 * a rough model is filled in instead. This is due to performance issues.
 * \see createTriangleDetail().
 */
void SoFCMeshSegmentShape::generatePrimitives(SoAction* action)
{
    SoState*  state = action->getState();
    const Mesh::MeshObject* mesh = SoFCMeshObjectElement::get(state);
    if (!mesh)
        return;
    const MeshCore::MeshPointArray & rPoints = mesh->getKernel().GetPoints();
    const MeshCore::MeshFacetArray & rFacets = mesh->getKernel().GetFacets();
    if (rPoints.size() < 3)
        return;
    if (rFacets.size() < 1)
        return;
    if (mesh->countSegments() <= this->index.getValue())
        return;
    const std::vector<unsigned long> rSegm = mesh->getSegment
        (this->index.getValue()).getIndices();

    // get material binding
    Binding mbind = this->findMaterialBinding(state);

    // Create the information when moving over or picking into the scene
    SoPrimitiveVertex vertex;
    SoPointDetail pointDetail;
    SoFaceDetail faceDetail;

    vertex.setDetail(&pointDetail);

    beginShape(action, TRIANGLES, &faceDetail);
    try 
    {
        for (std::vector<unsigned long>::const_iterator it = rSegm.begin(); it != rSegm.end(); ++it)
        {
            const MeshCore::MeshFacet& f = rFacets[*it];
            const MeshCore::MeshPoint& v0 = rPoints[f._aulPoints[0]];
            const MeshCore::MeshPoint& v1 = rPoints[f._aulPoints[1]];
            const MeshCore::MeshPoint& v2 = rPoints[f._aulPoints[2]];

            // Calculate the normal n = (v1-v0)x(v2-v0)
            SbVec3f n;
            n[0] = (v1.y-v0.y)*(v2.z-v0.z)-(v1.z-v0.z)*(v2.y-v0.y);
            n[1] = (v1.z-v0.z)*(v2.x-v0.x)-(v1.x-v0.x)*(v2.z-v0.z);
            n[2] = (v1.x-v0.x)*(v2.y-v0.y)-(v1.y-v0.y)*(v2.x-v0.x);

            // Set the normal
            vertex.setNormal(n);

            // Vertex 0
            if (mbind == PER_VERTEX_INDEXED || mbind == PER_FACE_INDEXED) {
                pointDetail.setMaterialIndex(f._aulPoints[0]);
                vertex.setMaterialIndex(f._aulPoints[0]);
            }
            pointDetail.setCoordinateIndex(f._aulPoints[0]);
            vertex.setPoint(sbvec3f(v0));
            shapeVertex(&vertex);

            // Vertex 1
            if (mbind == PER_VERTEX_INDEXED || mbind == PER_FACE_INDEXED) {
                pointDetail.setMaterialIndex(f._aulPoints[1]);
                vertex.setMaterialIndex(f._aulPoints[1]);
            }
            pointDetail.setCoordinateIndex(f._aulPoints[1]);
            vertex.setPoint(sbvec3f(v1));
            shapeVertex(&vertex);

            // Vertex 2
            if (mbind == PER_VERTEX_INDEXED || mbind == PER_FACE_INDEXED) {
                pointDetail.setMaterialIndex(f._aulPoints[2]);
                vertex.setMaterialIndex(f._aulPoints[2]);
            }
            pointDetail.setCoordinateIndex(f._aulPoints[2]);
            vertex.setPoint(sbvec3f(v2));
            shapeVertex(&vertex);

            // Increment for the next face
            faceDetail.incFaceIndex();
        }
    }
    catch (const Base::MemoryException&) {
        Base::Console().Log("Not enough memory to generate primitives\n");
    }

    endShape();
}

/**
 * Sets the bounding box of the mesh to \a box and its center to \a center.
 */
void SoFCMeshSegmentShape::computeBBox(SoAction *action, SbBox3f &box, SbVec3f &center)
{
    box.setBounds(SbVec3f(0,0,0), SbVec3f(0,0,0));
    center.setValue(0.0f,0.0f,0.0f);

    SoState*  state = action->getState();
    const Mesh::MeshObject * mesh = SoFCMeshObjectElement::get(state);
    if (mesh && mesh->countSegments() > this->index.getValue()) {
        const Mesh::Segment& segm = mesh->getSegment(this->index.getValue());
        const std::vector<unsigned long>& indices = segm.getIndices();
        Base::BoundBox3f cBox;
        if (!indices.empty()) {
            const MeshCore::MeshPointArray& rPoint = mesh->getKernel().GetPoints();
            const MeshCore::MeshFacetArray& rFaces = mesh->getKernel().GetFacets();

            for (std::vector<unsigned long>::const_iterator it = indices.begin();
                it != indices.end(); ++it) {
                    const MeshCore::MeshFacet& face = rFaces[*it];
                    cBox &= rPoint[face._aulPoints[0]];
                    cBox &= rPoint[face._aulPoints[1]];
                    cBox &= rPoint[face._aulPoints[2]];
            }
            
            box.setBounds(SbVec3f(cBox.MinX,cBox.MinY,cBox.MinZ),
                          SbVec3f(cBox.MaxX,cBox.MaxY,cBox.MaxZ));
            Base::Vector3f mid = cBox.CalcCenter();
            center.setValue(mid.x,mid.y,mid.z);
        }
    }
}

/**
 * Adds the number of the triangles to the \a SoGetPrimitiveCountAction.
 */
void SoFCMeshSegmentShape::getPrimitiveCount(SoGetPrimitiveCountAction * action)
{
    if (!this->shouldPrimitiveCount(action)) return;
    SoState*  state = action->getState();
    const Mesh::MeshObject * mesh = SoFCMeshObjectElement::get(state);
    if (mesh && mesh->countSegments() > this->index.getValue()) {
        const Mesh::Segment& segm = mesh->getSegment(this->index.getValue());
        action->addNumTriangles(segm.getIndices().size());
    }
}

// -------------------------------------------------------

SO_NODE_SOURCE(SoFCMeshObjectBoundary);

void SoFCMeshObjectBoundary::initClass()
{
    SO_NODE_INIT_CLASS(SoFCMeshObjectBoundary, SoShape, "Shape");
}

SoFCMeshObjectBoundary::SoFCMeshObjectBoundary()
{
    SO_NODE_CONSTRUCTOR(SoFCMeshObjectBoundary);
}

/**
 * Renders the open edges only.
 */
void SoFCMeshObjectBoundary::GLRender(SoGLRenderAction *action)
{
    if (shouldGLRender(action))
    {
        SoState*  state = action->getState();
        const Mesh::MeshObject * mesh = SoFCMeshObjectElement::get(state);
        if (!mesh) return;

        SoMaterialBundle mb(action);
        SoTextureCoordinateBundle tb(action, TRUE, FALSE);
        SoLazyElement::setLightModel(state, SoLazyElement::BASE_COLOR);
        mb.sendFirst();  // make sure we have the correct material

        drawLines(mesh);

        // Disable caching for this node
        SoGLCacheContextElement::shouldAutoCache(state, SoGLCacheContextElement::DONT_AUTO_CACHE);
    }
}

/**
 * Renders the triangles of the complete mesh.
 */
void SoFCMeshObjectBoundary::drawLines(const Mesh::MeshObject * mesh) const
{
    const MeshCore::MeshPointArray & rPoints = mesh->getKernel().GetPoints();
    const MeshCore::MeshFacetArray & rFacets = mesh->getKernel().GetFacets();

    // When rendering open edges use the given line width * 3 
    GLfloat lineWidth;
    glGetFloatv(GL_LINE_WIDTH, &lineWidth);
    glLineWidth(3.0f*lineWidth);

    // Use the data structure directly and not through MeshFacetIterator as this
    // class is quite slowly (at least for rendering)
    glBegin(GL_LINES);
    for (MeshCore::MeshFacetArray::_TConstIterator it = rFacets.begin(); it != rFacets.end(); ++it) {
        for (int i=0; i<3; i++) {
            if (it->_aulNeighbours[i] == ULONG_MAX) {
                glVertex(rPoints[it->_aulPoints[i]]);
                glVertex(rPoints[it->_aulPoints[(i+1)%3]]);
            }
        }
    }

    glEnd();
}

void SoFCMeshObjectBoundary::generatePrimitives(SoAction* action)
{
    // do not create primitive information as an SoFCMeshObjectShape
    // should already be used that delivers the information
    SoState*  state = action->getState();
    const Mesh::MeshObject* mesh = SoFCMeshObjectElement::get(state);
    if (!mesh)
        return;
    const MeshCore::MeshPointArray & rPoints = mesh->getKernel().GetPoints();
    const MeshCore::MeshFacetArray & rFacets = mesh->getKernel().GetFacets();

    // Create the information when moving over or picking into the scene
    SoPrimitiveVertex vertex;
    SoPointDetail pointDetail;
    SoLineDetail lineDetail;

    vertex.setDetail(&pointDetail);

    beginShape(action, LINES, &lineDetail);
    for (MeshCore::MeshFacetArray::_TConstIterator it = rFacets.begin(); it != rFacets.end(); ++it)
    {
        for (int i=0; i<3; i++) {
            if (it->_aulNeighbours[i] == ULONG_MAX) {
                const MeshCore::MeshPoint& v0 = rPoints[it->_aulPoints[i]];
                const MeshCore::MeshPoint& v1 = rPoints[it->_aulPoints[(i+1)%3]];

                // Vertex 0
                pointDetail.setCoordinateIndex(it->_aulPoints[i]);
                vertex.setPoint(sbvec3f(v0));
                shapeVertex(&vertex);

                // Vertex 1
                pointDetail.setCoordinateIndex(it->_aulPoints[(i+1)%3]);
                vertex.setPoint(sbvec3f(v1));
                shapeVertex(&vertex);

                // Increment for the next open edge
                lineDetail.incLineIndex();
            }
        }
    }

    endShape();
}

/**
 * Sets the bounding box of the mesh to \a box and its center to \a center.
 */
void SoFCMeshObjectBoundary::computeBBox(SoAction *action, SbBox3f &box, SbVec3f &center)
{
    SoState*  state = action->getState();
    const Mesh::MeshObject * mesh = SoFCMeshObjectElement::get(state);
    if (!mesh)
        return;
    const MeshCore::MeshPointArray & rPoints = mesh->getKernel().GetPoints();
    if (rPoints.size() > 0) {
        Base::BoundBox3f cBox;
        for (MeshCore::MeshPointArray::_TConstIterator it = rPoints.begin(); it != rPoints.end(); ++it)
            cBox &= (*it);
        box.setBounds(SbVec3f(cBox.MinX,cBox.MinY,cBox.MinZ),
                      SbVec3f(cBox.MaxX,cBox.MaxY,cBox.MaxZ));
        Base::Vector3f mid = cBox.CalcCenter();
        center.setValue(mid.x,mid.y,mid.z);
    }
    else {
        box.setBounds(SbVec3f(0,0,0), SbVec3f(0,0,0));
        center.setValue(0.0f,0.0f,0.0f);
    }
}

/**
 * Adds the number of the triangles to the \a SoGetPrimitiveCountAction.
 */
void SoFCMeshObjectBoundary::getPrimitiveCount(SoGetPrimitiveCountAction * action)
{
    if (!this->shouldPrimitiveCount(action)) return;
    SoState*  state = action->getState();
    const Mesh::MeshObject * mesh = SoFCMeshObjectElement::get(state);
    if (!mesh)
        return;
    const MeshCore::MeshFacetArray & rFaces = mesh->getKernel().GetFacets();

    // Count number of open edges first
    int ctEdges=0;
    for (MeshCore::MeshFacetArray::_TConstIterator jt = rFaces.begin(); jt != rFaces.end(); ++jt) {
        for (int i=0; i<3; i++) {
            if (jt->_aulNeighbours[i] == ULONG_MAX) {
                ctEdges++;
            }
        }
    }

    action->addNumLines(ctEdges);
}
