#ifndef EGLUTILSVG_H
#define EGLUTILSVG_H

#include "eglutils.h"

class EGLUtilsVG : public EglUtils
{
public:
    EGLUtilsVG();

    virtual ~EGLUtilsVG();

    virtual void clear( float r=0, float g=0, float b=0);
    virtual int loadTexture(const char *filename);
    virtual void releaseTexture(int textureHandle);

};

#endif // EGLUTILSVG_H
