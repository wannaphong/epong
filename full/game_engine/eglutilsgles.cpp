/*
 * Copyright (c) 2011 Nokia Corporation.
 */

#include <QDebug>
#include <QString>
#include <QImage>
#include "eglutilsgles.h"
#include <stdio.h>
#include <assert.h>

#ifdef USE_GLES11
#include <GLES/gl.h>
#endif
#ifdef USE_GLES20
#include <GLES2/gl2.h>
#endif


EglUtilsGles::EglUtilsGles() : EglUtils()
{
}

EglUtilsGles::~EglUtilsGles()
{
}

void EglUtilsGles::clear( float r, float g, float b) {
    glClearColor( r,g,b, 0.0f );
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}


int EglUtilsGles::loadTexture(const char *fn)
{
    // clear previous error
    glGetError();

    char testr[256];
    sprintf(testr, ":/%s", fn );
    QString filename = QString( testr );
    QImage image = QImage( filename );

    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    GLuint* pTexData = new GLuint[image.width() * image.height()];
    GLuint* sdata = (GLuint*)image.bits();
    GLuint* tdata = pTexData;

    for (int y=0; y<image.height(); y++)
        for (int x=0; x<image.width(); x++) {
            //*tdata = *sdata;
            *tdata = ((*sdata&255) << 16) | (((*sdata>>8)&255) << 8)
                    | (((*sdata>>16)&255) << 0) | (((*sdata>>24)&255) << 24);
            sdata++;
            tdata++;
    };

#ifdef USE_GLES20
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST );
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
#else
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
#endif
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image.width(), image.height(), 0,
                 GL_RGBA, GL_UNSIGNED_BYTE, pTexData);

#ifdef USE_GLES20
    glGenerateMipmap(GL_TEXTURE_2D);
#endif
#ifndef NDEBUG
    GLint error = glGetError();    
    if(error)
    {
    qDebug("error: texture upload returned %x",error);
    }
#endif

    delete [] pTexData;
    return texture;
}

void EglUtilsGles::releaseTexture(int textureHandle)
{
    if(textureHandle) glDeleteTextures(1, (const GLuint *)&textureHandle);
}
