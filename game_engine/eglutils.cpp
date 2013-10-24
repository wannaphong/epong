#include <QDebug>
#include "eglutils.h"

EglUtils::EglUtils()
{
    // EGL variables
    eglDisplay	= 0;
    eglConfig	= 0;
    eglSurface	= 0;
    eglContext	= 0;
}

EglUtils::~EglUtils() {

}



const EGLint eglConfigAttribs[]={
    EGL_SURFACE_TYPE, EGL_WINDOW_BIT,

#ifndef MY_OS_MEEGO
    // note Nokia 500 OpenVG doesnt show corrently if buffer size 24 is not defined
    EGL_BUFFER_SIZE, 24,
    EGL_DEPTH_SIZE, 0,
#else
    EGL_DEPTH_SIZE, 4,
#endif

#ifdef USE_GLES11
    EGL_RENDERABLE_TYPE, EGL_OPENGL_ES_BIT,
#endif
#ifdef USE_GLES20
    EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
#endif
#ifdef USE_OPENVG
    EGL_RENDERABLE_TYPE, EGL_OPENVG_BIT,
#endif
    EGL_NONE,
};

const EGLint eglContextAttribs[]={
#ifdef USE_GLES11
    EGL_CONTEXT_CLIENT_VERSION, 1,
#endif
#ifdef USE_GLES20
    EGL_CONTEXT_CLIENT_VERSION, 2,
#endif
    EGL_NONE,
};

bool EglUtils::createWindow( int displayId, int handle ) {
    if (handle==-1) displayId = (int)EGL_DEFAULT_DISPLAY;
    if (displayId==-1) displayId = (int)EGL_DEFAULT_DISPLAY;
    eglDisplay = eglGetDisplay( (EGLNativeDisplayType)displayId);

    qDebug() << "eglGetDisplay()" << eglDisplay;

    EGLint iMajorVersion, iMinorVersion;
    if (!eglInitialize(eglDisplay, &iMajorVersion, &iMinorVersion))
    {
        qDebug() << "Error: eglInitialize() failed.\n";
        return fatalError();
    }
    qDebug() << "eglInitialize() major:" << iMajorVersion << " minor: " << iMinorVersion;

#ifdef USE_OPENVG
    eglBindAPI(EGL_OPENVG_API);
#endif


    EGLConfig eglConfigs[10];
    EGLint iConfigs;
    if (!eglChooseConfig(eglDisplay, eglConfigAttribs, eglConfigs, 10, &iConfigs))  {
        qDebug() << "Error: eglChooseConfig() failed.\n";
        return fatalError();
    }

    qDebug() << "theQtGameEngine->eglChooseConfig()" << iConfigs;

    storedWindowHandle = (EGLNativeWindowType)handle;

/*#ifndef MY_OS_MEEGO
    eglConfig = eglConfigs[0];
    eglSurface = eglCreateWindowSurface(eglDisplay, eglConfig, storedWindowHandle, NULL);
    qDebug() << "eglCreateWindowSurface(): " << eglSurface;
    if (!testEGLError("eglCreateWindowSurface")) {
        return fatalError();
    }
#else*/
    EGLint iErr = EGL_SUCCESS;
    for (int ii = 0; ii < iConfigs; ii++) {
        eglSurface = eglCreateWindowSurface(eglDisplay, eglConfigs[ii], storedWindowHandle, 0);
        qDebug() << "eglCreateWindowSurface(): " << eglSurface;
        iErr = eglGetError();
        if (iErr == EGL_SUCCESS) {
            eglConfig = eglConfigs[ii];
            qDebug() << "eglCreateWindowSurface success";
            break;
        }
        else {
            qDebug() << "eglCreateWindowSurface failed" << eglConfig << (iErr == EGL_BAD_CONFIG?"was bad config":"");
        }
    }
    if (!eglConfig) {
        qDebug() << "eglCreateWindowSurface failed";
        return fatalError();
    }
//#endif


    eglContext = eglCreateContext(eglDisplay, eglConfig, 0, eglContextAttribs);
    qDebug() << "theQtGameEngine->eglCreateContext()";

    if (!testEGLError("eglCreateContext")) {
        return fatalError();
    }

    eglMakeCurrent(eglDisplay, eglSurface, eglSurface, eglContext);
    qDebug() << "theQtGameEngine->eglMakeCurrent()";

    if (!testEGLError("eglMakeCurrent")) {
        return fatalError();
    }

    return true;
}


bool EglUtils::testEGLError(const char* pszLocation) {
    EGLint iErr = eglGetError();
    if (iErr != EGL_SUCCESS) {
        qDebug() <<  "%s failed (%d).\n" <<  pszLocation << iErr;
        return false;
    }
    return true;
}

// sets the context and surfaces as current
bool EglUtils::setCurrent() {
    eglMakeCurrent(eglDisplay, eglSurface, eglSurface, eglContext);
    if (!testEGLError("eglMakeCurrent")) {
        return fatalError();
    }
}

// clears the current context
bool EglUtils::clearContext() {
    return eglMakeCurrent(eglDisplay, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT) ;
}

bool EglUtils::fatalError() {
    eglMakeCurrent(eglDisplay, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT) ;
    eglTerminate(eglDisplay);
    eglDisplay = 0;     // error
    return false;
}


bool EglUtils::swapBuffers() {
    if (!eglSwapBuffers(eglDisplay, eglSurface)) {
        EGLint errval = eglGetError();
        /*
        EXPLANATION:
        http://library.forum.nokia.com/index.jsp?topic=/Nokia_Symbian3_Developers_Library/GUID-894AB487-C127-532D-852B-37CB0DEA1440.html
        On the Symbian platform, EGL handles the window resize in the next
        call to eglSwapBuffers(), which resizes the surface to match the new
        window size. If the preserve buffer option is in use, this function
        also copies across all the pixels from the old surface that overlap
        the new surface, although the exact details depend on the
        implementation.If the surface resize fails, eglSwapBuffers() returns
        EGL_FALSE and an EGL_BAD_ALLOC error is raised. This may mean that
        the implementation does not support the resizing of a surface or that
        there is not enough memory available (on a platform with GPU, this
        would be GPU rather than system memory). Applications must always
        monitor whether eglSwapBuffers() fails after a window resize.
        When it does fail, the application should do the following:
        Call eglDestroySurface() to destroy the current EGL window surface.
        Call eglCreateWindowSurface() to recreate the EGL window surface.
        This may cause a noticeable flicker and so should be done only when
        necessary.
       */

        //qDebug() << "eglSwapbuffers failed with error: " << errval;
        if (errval==EGL_BAD_ALLOC || errval==EGL_BAD_SURFACE) {
            if (errval==EGL_BAD_ALLOC)
                //qDebug() << "Error was bad alloc, .. taking care of it.";

            eglDestroySurface( eglDisplay, eglSurface );

            eglSurface = eglCreateWindowSurface(eglDisplay,
                                                eglConfig,
                                                storedWindowHandle,
                                                0);

        } else return fatalError();
    }

    return true;
}
