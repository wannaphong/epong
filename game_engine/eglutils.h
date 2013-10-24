/*
 * Copyright (c) 2011 Nokia Corporation.
 */


#ifndef __EGLUTILS_H__
#define __EGLUTILS_H__

#include <EGL/egl.h>


class EglUtils {
public:
    EglUtils();
    virtual ~EglUtils();

    /**
     * displayId = -1 = default
     */
    virtual bool createWindow( int displayId, int handle );
    virtual bool swapBuffers();
    // sets the context and surfaces as current
    virtual bool setCurrent();
    // clears the current context
    virtual bool clearContext();

    /** Clear framebuffer with given color
      */
    virtual void clear( float r=0, float g=0, float b =0) = 0;

    /** Allocate and load texture from given file
      * @return rendering handle
      */
    virtual int loadTexture(const char *filename)=0;

    /** Release allocated texture resource handle
      */
    virtual void releaseTexture(int textureHandle)=0;

protected:
    bool fatalError();
    bool testEGLError(const char* pszLocation);

    EGLNativeWindowType         storedWindowHandle;

    // EGL variables
    EGLDisplay			eglDisplay;
    EGLConfig			eglConfig;
    EGLSurface			eglSurface;
    EGLContext			eglContext;


};


#endif
