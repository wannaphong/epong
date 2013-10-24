#include "gameglwidget.h"

#ifdef Q_OS_SYMBIAN
// memory profile helper
#include "memprofile.h"
#include <EGL/egl.h>
#endif

#ifdef USE_GOOM_MONITOR
// engine requests 10 MB of graphics memory from goom
const int KRequestGraphicsMemoryAmount = 10*1024*1024;
#endif

static void Profiling()
{
#if defined(Q_OS_SYMBIAN) && !defined(NDEBUG)
    static int iFrameCount = 0;
    iFrameCount++;

    static int lastUsedPrivate = 0;

    EGLDisplay aDisp = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    int aTotalGRAM = 0;
    int aUsedGRAM = 0;
    int aUsedPrivate = 0;
    int aUsedShared = 0;
    int free = 0;

    GetProfilingExtensionData(aDisp, aTotalGRAM, aUsedGRAM, aUsedPrivate,
                    aUsedShared);
    free = aTotalGRAM - aUsedGRAM;

    if (lastUsedPrivate != aUsedPrivate)
    {
        qDebug("Frame %d", iFrameCount);
        qDebug(
            "MEMORY_STATS_TOTAL: %d USED: %d USEDPRIVATE: %d USEDSHARED: %d FREE: %d DELTA:%d\n",
        aTotalGRAM, aUsedGRAM, aUsedPrivate, aUsedShared, free,
        lastUsedPrivate - aUsedPrivate);
    }
    lastUsedPrivate = aUsedPrivate;
#endif
}


GameGLWidget::GameGLWidget(GF::GameApp *game0, QWidget *parent) :
    QGLWidget(QGLFormat(QGL::NoDepthBuffer), parent), game(game0),
    gamePrepared(false)
{
    qDebug("GameGLWidget game=%x", game0);
    Profiling();
}

GameGLWidget::~GameGLWidget()
{
    qDebug("~GameGLWidget");
    uninitializeGL();
    Profiling();
}

void GameGLWidget::uninitializeGL()
{
    qDebug("uninitializeGL");

    if(gamePrepared && game)
    {
        game->release();
        gamePrepared=false;
    }

#ifdef USE_GOOM_MONITOR
    iGoom.Close();
#endif
    doneCurrent();

}

void GameGLWidget::initializeGL()
{
    qDebug("initializeGL");
    Profiling();

#ifdef USE_GOOM_MONITOR
    int err=iGoom.Connect();
    qDebug("GOOM ret = %d", err);

    if(err==KErrNone)
    {
        for(int i=0;i<10;i++)
        {
            int err = iGoom.RequestFreeMemory(KRequestGraphicsMemoryAmount);
            qDebug("GOOM RequestFreeMemory ret = %d", err);
            if(err==KErrNone)
                break;
            else
                User::After(200000);
        }
    }
#endif

    Profiling();

    if(game)
    {
        game->prepare();
        gamePrepared=true;
    }

#ifdef USE_GOOM_MONITOR
    // iGoom.MemoryAllocationsComplete();
#endif

    qDebug("initializeGL done");
    Profiling();
}

void GameGLWidget::resizeGL(int w, int h)
{
    qDebug("ResizeGL: %dx%d", w, h) ;
    glViewport(0,0, w,h );
    if(game) game->resize(w, h);
    qDebug("ResizeGL done") ;
    Profiling();
}

void GameGLWidget::paintGL()
{
//    qDebug("paintGL") ;
    Profiling();

    glClearColor(0,0,0,0);
    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

    if(game) game->render();
}

