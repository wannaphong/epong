/*
 * Copyright  2011 Nokia Corporation.
 * Implementation of GameEngine with Qt and native OpenGL ES 2.0
 *
 */


#ifndef GameWindow_H
#define GameWindow_H

#include <QtCore/qmetatype.h>
#include <QObject>
#include <QWidget>
#include "GameEngine.h"
#include "GameApp.h"
#include <EGL/egl.h>
#include "audioout.h"
#include "accelerometerfilter.h"
#include "AudioInterfaces.h"
#include "eglutils.h"
#include <QPaintEngine>

#if defined(Q_OS_SYMBIAN) || defined(Q_WS_MAEMO_5) || defined(MY_OS_MEEGO)
    // For silent-profile detection
    #include <QSystemDeviceInfo>
    QTM_USE_NAMESPACE
#endif

#ifdef MY_OS_MEEGO
#include <QtSensors/QAccelerometer>
#include <QtMobility/qmobilityglobal.h>
QTM_USE_NAMESPACE
#endif

#ifdef Q_OS_SYMBIAN
        // For listening GOOM-events
    #include <QSymbianEvent>
    // For Symbian hardwarevolume keys
    #include <remconcoreapitargetobserver.h>
    class CRemConInterfaceSelector;
    class CRemConCoreApiTarget;
#endif

#ifdef MY_OS_MEEGO
    // for periodical temporary blanking pause
#include <qmsystem2/qmdisplaystate.h>
#endif

#ifdef IAP
#include "../epong_iap_impl/miniiapqtclient.h";
#endif

namespace GF {

class QtGameEngine : public QWidget, public GameEngine, public IAudioSource
{
    Q_OBJECT
public:
    explicit QtGameEngine(QWidget *parent = 0);
    virtual ~QtGameEngine();

        // Time passed since last frame
    float getFrameTime() { return m_frameTime; }
    float getFPS() { return m_fps; }

    int loadGLTexture( const char *fileName );
    void releaseTexture(int textureHandle);

    void* loadBinaryResource( const char *fileName, int &sizeTarget );
    void releaseBinaryResource( void *resource );

    // Override paintevent.
    void paintEvent(QPaintEvent *e) { };

    QPaintEngine *paintEngine() { return 0; }



        // Audio status / control
    bool isProfileSilent();

    void create();
    void destroy();
    void pause();
    void resume();

    void volumeUp();
    void volumeDown();


    void exit();
    int openUrlInBrowser(const char *path8);
    //float getVolume() { return 0.5f; }
    int pullAudio( AUDIO_SAMPLE_TYPE *target, int sampleCount );
    //void readAudioStream( void * target, int count );

    void sendEvent(ENGINE_EVENT_TYPE type, int flags);
    bool queryTextInput( const char *prompt, char *target, int targetLength );

#ifdef Q_OS_SYMBIAN
    bool symbianEventFilter( const QSymbianEvent* symbianEvent );
#endif

    const char* adjustPath(const char *inputPath, char *outputPath, int outputPathLength, bool *isOk = 0);
    bool startSensors();
    void stopSensors();
    virtual void setUpdateAndPauseState(bool updatesDisabled, bool pauseResumeDisabled);
#ifdef IAP
    virtual MiniIAPClientApi *getMiniIAPClient();
#endif
#ifdef MY_OS_MEEGO
    virtual bool isSwipeEnabled();
#endif
protected:
    bool createEGLUtils();
    AccelerometerFilter *acceleration;
#if defined(Q_OS_SYMBIAN) || defined(MY_OS_MEEGO)
    QAccelerometer sensor;
#endif
    bool profileSilent;
    GE::AudioOut *m_aout;
    int currentVolumeSlot;

    bool m_pauseResumeDisabled;
    bool m_updatesDisabled;

    void mousePressEvent ( QMouseEvent * event );
    void mouseReleaseEvent ( QMouseEvent * event );
    void mouseMoveEvent ( QMouseEvent * event );
    void keyPressEvent( QKeyEvent *event );
    bool event(QEvent *event);
    bool touchEventsEnabled;


    // The game we are running
    GameApp *runningGame;


    unsigned int getTickCount();
    bool eventFilter(QObject *object, QEvent *event);
    void resizeEvent( QResizeEvent *event );


    void doFrame();



    void timerEvent(QTimerEvent *event);

    bool TestEGLError(const char* pszLocation);
    void CleanupAndExit ( EGLDisplay eglDisplay );

    EglUtils *eglUtils;

    // Time calculation
    unsigned int m_frameCount;
    unsigned int m_prevTime;
    unsigned int m_prevFpsTime;
    unsigned int m_currentTime;
    float m_frameTime;
    float m_fps;
#ifdef MY_OS_MEEGO
    // for periodical temporary blanking pause
    ::MeeGo::QmDisplayState qmDisplayState;
    // for storing the current sweep enabled / disabled state
    bool m_swipeEnabled;
#endif



    bool m_paused;
    int timerid;


#ifdef Q_OS_SYMBIAN
    // To handle the hardware volume keys on Symbian
    class Observer : public MRemConCoreApiTargetObserver
    {
    public:
        Observer(QtGameEngine *ge) : gameEngine(ge) {}
        virtual void MrccatoCommand(TRemConCoreApiOperationId aOperationId,
                                    TRemConCoreApiButtonAction /*aButtonAct*/)
        {
            switch ( aOperationId ) {
            case ERemConCoreApiVolumeDown:
                gameEngine->volumeDown();
                break;
            case ERemConCoreApiVolumeUp:
                gameEngine->volumeUp();
                break;
            default:
                break;
            }
        }
    protected:
        QtGameEngine *gameEngine;
    };

    Observer *m_Observer;
    CRemConInterfaceSelector *m_Selector;
    CRemConCoreApiTarget *m_Target;
#endif

#ifdef IAP
    MiniIAPQtClient *m_miniIAPClient;
#endif

};

}

#endif // GameWindow_H
