#ifndef QGLGAMEENGINE_H
#define QGLGAMEENGINE_H

#include <QGLWidget>
#include <QMainWindow>
#include <QTimer>
#include <QTime>
#include <QDebug>
#include <QFile>
#include <QMap>
#include <QtDeclarative/QDeclarativeView>
#include <QStackedWidget>

#include "GameApp.h"
#include "GameEngine.h"
#include "gameglwidget.h"

#ifdef USE_PUSH_AUDIO_OUT
#include "pushaudioout.h"
#endif

#ifdef USE_PULL_AUDIO_OUT
#include "pullaudioout.h"
#endif

#if defined(Q_OS_SYMBIAN) || defined(MY_OS_MEEGO)
#include <QAccelerometer>
QTM_USE_NAMESPACE
#endif

class QGlGameEngine : public QStackedWidget, public GF::GameEngine, public GF::IAudioSource
{
    Q_OBJECT
public:
    explicit QGlGameEngine(QWidget *parent = 0);

    ~QGlGameEngine();

    bool event(QEvent *);
    void keyPressEvent(QKeyEvent *);
#ifdef USE_VKB
    void inputMethodEvent(QInputMethodEvent *);
#endif

signals:
    virtual void quit();

public slots:
#ifdef USE_PUSH_AUDIO_OUT
    virtual void audioTick();
#endif
    void renderTick();


#ifdef USE_QML
    void qmlQuit();
#endif

protected:
    void startGame();
    void pauseGame();

    GF::GameApp *game;
    QTimer timer;

#ifdef USE_PUSH_AUDIO_OUT
    QTimer audioTimer;
    GE::PushAudioOut *m_aout;
#endif

#ifdef USE_PULL_AUDIO_OUT
    GE::PullAudioOut *m_aout;
#endif

public: // engine
    virtual void exit();
    virtual bool startAudio( int frequency, int channels );
    virtual void stopAudio();

    virtual bool queryTextInput( const char *prompt, char *target, int targetLength );

    virtual int loadGLTexture( const char *fileName );
    virtual void releaseTexture(int textureHandle);

    virtual void* loadBinaryResource( const char *fileName, int &sizeTarget );
    virtual void releaseBinaryResource( void *resource );

    virtual void sendEvent(GF::ENGINE_EVENT_TYPE type, int flags);

    virtual int openUrlInBrowser(const char *path8);

    virtual bool startSensors() {qDebug("TODO: startSensors"); }
    virtual void stopSensors() {qDebug("TODO: stopSensors"); }
    virtual const char *adjustPath(const char *inputPath, char *outputPath, int outputPathLength, bool *isOk = 0);
    virtual void setUpdateAndPauseState(bool updatesDisabled, bool pauseResumeDisabled) {qDebug("TODO: setUpdateAndPauseState"); }

    virtual int pullAudio( AUDIO_SAMPLE_TYPE *target, int sampleCount );

#if defined(Q_OS_SYMBIAN) || defined(MY_OS_MEEGO)
    QAccelerometer sensor;
#endif

    QMap<uchar*, QFile*> resourceMap;
    QDeclarativeView *view;

    GameGLWidget *gl;
    QStackedWidget stacked;

    bool gameRunning;

    int audioFrequency;
    int audioChannels;

    QTime previousRenderTick;

    bool splashScreen;

#ifdef USE_VKB
    int lastStringLength;
#endif
};

#endif // QGLGAMEENGINE_H
