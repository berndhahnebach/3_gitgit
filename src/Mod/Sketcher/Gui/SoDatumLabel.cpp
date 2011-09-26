/***************************************************************************
 *   Copyright (c) 2011                                                    *
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
# include <cfloat>
# include <algorithm>
# include <QFontMetrics>
# include <QGLWidget>
# include <QPainter>
# include <QPen>
# include <Inventor/SoPrimitiveVertex.h>
# include <Inventor/actions/SoGLRenderAction.h>
# include <Inventor/misc/SoState.h>
# include <math.h>
#endif

#include <Inventor/elements/SoFontNameElement.h>
#include <Inventor/elements/SoFontSizeElement.h>
#include <Inventor/elements/SoModelMatrixElement.h>
#include <Inventor/elements/SoProjectionMatrixElement.h>
#include <Inventor/elements/SoViewingMatrixElement.h>
#include <Inventor/elements/SoViewVolumeElement.h>
#include <Inventor/elements/SoViewportRegionElement.h>

#include "SoDatumLabel.h"
#include <Gui/BitmapFactory.h>

using namespace Gui;

// ------------------------------------------------------

SO_NODE_SOURCE(SoDatumLabel);

void SoDatumLabel::initClass()
{
    SO_NODE_INIT_CLASS(SoDatumLabel, SoShape, "Shape");
}


SoDatumLabel::SoDatumLabel()
{
    SO_NODE_CONSTRUCTOR(SoDatumLabel);
    SO_NODE_ADD_FIELD(string, (""));
    SO_NODE_ADD_FIELD(textColor, (SbVec3f(1.0f,1.0f,1.0f)));
    SO_NODE_ADD_FIELD(name, ("Helvetica"));
    SO_NODE_ADD_FIELD(size, (12));

    this->bbx = 0;
    this->bby = 0;
}

void SoDatumLabel::drawImage()
{
    const SbString* s = string.getValues(0);
    int num = string.getNum();
    if (num == 0) {
        this->image = SoSFImage();
        return;
    }

    QFont font(QString::fromAscii(name.getValue()), size.getValue());
    QFontMetrics fm(font);
    QString str = QString::fromUtf8(s[0].getString());
    int w = fm.width(str);
    int h = fm.height();

    // No Valid text
    if(!w) {
        this->image = SoSFImage();
        return;
    }
    const SbColor& t = textColor.getValue();
    QColor front;
    front.setRgbF(t[0],t[1], t[2]);

    QImage image(w, h,QImage::Format_ARGB32_Premultiplied);
    image.fill(0x00000000);
    
    QPainter painter(&image);
    painter.setRenderHint(QPainter::Antialiasing);

    painter.setPen(front);
    painter.setFont(font);
    painter.drawText(0,0,w,h, Qt::AlignLeft , str);
    painter.end();

    SoSFImage sfimage;
    Gui::BitmapFactory().convert(image, sfimage);
    this->image = sfimage;
}

void SoDatumLabel::computeBBox(SoAction *action, SbBox3f &box, SbVec3f &center)
{

    SbVec2s size;
    int nc;


    const unsigned char * dataptr = this->image.getValue(size, nc);
    if(dataptr == NULL)
        return;

    float srcw = size[0];
    float srch = size[1];

    
    SoState *state =  action->getState();
    
    const SbViewVolume & vv = SoViewVolumeElement::get(state);
    float scale = vv.getWorldToScreenScale(SbVec3f(0.f,0.f,0.f), 1.f);

    float aspectRatio =  (float) srcw / (float) srch;

    float height = scale / (float) srcw;
    float width  = aspectRatio * (float) height;
    if(action->getTypeId() != SoGLRenderAction::getClassTypeId()) {
        width = this->bbx;
        height = this->bby;
    
    }
    SbVec3f min (-width / 2, -height / 2, 0.f);
    SbVec3f max (width  / 2, height  / 2, 0.f);
    box.setBounds(min, max);
    center.setValue(0.f,0.f,0.f);
}

void SoDatumLabel::generatePrimitives(SoAction * action)
{
    
    SoPrimitiveVertex pv;
    SoState *state =  action->getState();

    // Get the size
    SbVec2s size;
    int nc;

    const unsigned char * dataptr = this->image.getValue(size, nc);
    if(dataptr == NULL)
        return;
    
    float srcw = size[0];
    float srch = size[1];

    const SbViewVolume & vv = SoViewVolumeElement::get(state);
    float scale = vv.getWorldToScreenScale(SbVec3f(0.f,0.f,0.f), 1.f);

    float aspectRatio =  (float) srcw / (float) srch;

    float height = scale / (float) srcw;
    float width  = aspectRatio * (float) height;

    if(action->getTypeId() != SoGLRenderAction::getClassTypeId()) {
        width = this->bbx;
        height = this->bby;

    }


    
    beginShape(action, QUADS);
    
    pv.setNormal( SbVec3f(0.f, 0.f, 1.f) );

    // Set coordinates
    pv.setPoint( SbVec3f(-width / 2, height / 2, 0.f) );
    shapeVertex(&pv);

    pv.setPoint( SbVec3f(-width / 2, -height / 2, 0.f) );
    shapeVertex(&pv);

    pv.setPoint( SbVec3f(width / 2, -height / 2, 0.f) );
    shapeVertex(&pv);
    
    pv.setPoint( SbVec3f(width / 2, height / 2, 0.f) );
    shapeVertex(&pv);

    endShape();
}

void SoDatumLabel::GLRender(SoGLRenderAction * action)
{
    SoState *state = action->getState();
    if(!shouldGLRender(action))
        return;

    drawImage();

    SbVec2s size;
    int nc;
    const unsigned char * dataptr = this->image.getValue(size, nc);
    if (dataptr == NULL) return; // no image

    int srcw = size[0];
    int srch = size[1];
    int skipx = 0;
    int skipy = 0;

//     glDisable(GL_LIGHTING);
    glPixelStorei(GL_UNPACK_ROW_LENGTH, srcw);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 4);

    // Copy the text bitmap into memory and bind
    GLuint myTexture;
    // generate a texture
    glGenTextures(1, &myTexture);

    glDisable(GL_DEPTH_TEST);
    // enable texturing
    glEnable(GL_TEXTURE_2D);

    glBindTexture(GL_TEXTURE_2D, myTexture);

    glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    glTexImage2D(GL_TEXTURE_2D, 0, nc, srcw, srch, 0, GL_RGBA, GL_UNSIGNED_BYTE,(const GLvoid*)  dataptr);

    // Use Stored Texture
    glEnable(GL_BLEND);

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        // Create the quad to hold texture
    glBegin(GL_QUADS);

    const SbViewVolume & vv = SoViewVolumeElement::get(state);
    float scale = vv.getWorldToScreenScale(SbVec3f(0.f,0.f,0.f), 1.f);
                    
    float aspectRatio =  (float) srcw / (float) srch;
    float height = scale / (float) srcw;
    float width  = aspectRatio * (float) height;

    this->bbx = width;
    this->bby = height;
    
    glTexCoord2f(0.f, 1.f); glVertex2f(-width/ 2, height / 2);
    glTexCoord2f(0.f, 0.f); glVertex2f(-width / 2,-height / 2);
    glTexCoord2f(1.f, 0.f); glVertex2f( width/ 2,-height / 2);
    glTexCoord2f(1.f, 1.f); glVertex2f( width / 2, height / 2);
    
    glEnd();

    // Reset the Mode
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_TEXTURE_2D);
    glDisable(GL_BLEND);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
}
 
