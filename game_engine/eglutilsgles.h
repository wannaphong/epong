/*
 * Copyright (c) 2011 Nokia Corporation.
 */

#ifndef __EGLUTILS_GLES_H__
#define __EGLUTILS_GLES_H__

#include "eglutils.h"

class EglUtilsGles : public EglUtils {
 public:
    EglUtilsGles();
    virtual ~EglUtilsGles();

    virtual void clear( float r=0, float g=0, float b=0);

    /*
    bool createWindow( int displayId, int handle );
    bool swapBuffers();
    // sets the context and surfaces as current
    virtual bool setCurrent();
    // clears the current context
    virtual bool clearContext();
*/

    /** Allocate and load texture from given file
      * @return rendering handle
      */
    virtual int loadTexture(const char *filename);

    /** Release allocated texture resource handle
      */
    virtual void releaseTexture(int textureHandle);

};


#endif
