#include <QInputEvent>
#include <QCoreApplication>
#include <QTimer>
#include <QStackedWidget>
#include <QFile>
#include <QApplication>
#include <QDesktopServices>
#include <QUrl>
#include <QTouchEvent>
#include <QInputDialog>
#include <string.h>

#include "qglgameengine.h"
#include "PongApp.h"

#ifdef USE_HAPTIC
#include <QFeedbackEffect>
#endif


QGlGameEngine::QGlGameEngine(QWidget *parent) :
    QStackedWidget(parent), gl(0), view(0), audioFrequency(0), audioChannels(0),
    gameRunning(true), splashScreen(false)
  #if defined(USE_PUSH_AUDIO_OUT)
      , m_aout(0)
  #endif
  #if defined(USE_PULL_AUDIO_OUT)
      , m_aout(0)
  #endif
{
    qDebug("QGlGameEngine");
    setAutoFillBackground( false );

    setAttribute(Qt::WA_AcceptTouchEvents);
    setAttribute(Qt::WA_LockLandscapeOrientation);

#ifdef USE_VKB
    setAttribute(Qt::WA_InputMethodEnabled);
    setFocusPolicy(Qt::StrongFocus);
#endif

    qDebug("game create");
    game = GF::createGameApplication(this);

#ifdef USE_QML
    splashScreen = true;
    view = new QDeclarativeView(this);
    qDebug("qml splash screen %x", view);

    // note queuedconnection type, which makes it possible to setSource from
    // directly from slot invoked from qml
    QObject::connect((QObject*)view->engine(), SIGNAL(quit()), this, SLOT(qmlQuit()), Qt::QueuedConnection);
#ifdef Q_OS_SYMBIAN
    view->setSource(QUrl("qrc:/qml/splash.qml"));
#else
    view->setSource(QUrl("qrc:/qml/splash.qml"));
#endif
    addWidget(view);
#else
    gameRunning=false;
    startGame();
#endif
    qDebug("QGlGameEngine done");
}

QGlGameEngine::~QGlGameEngine()
{
    qDebug("~QGlGameEngine");

    timer.stop();
    #if defined(Q_OS_SYMBIAN) || defined(MY_OS_MEEGO)
    sensor.stop();
    #endif

    stopAudio();

    delete gl;
}

#ifdef USE_QML
void QGlGameEngine::qmlQuit()
{
    // set empty qml document
    if(view) view->setSource(QUrl());

    splashScreen=false;
    gameRunning=false;
    startGame();
}
#endif

void QGlGameEngine::startGame()
{
    qDebug("startGame");

    if(gameRunning)
    {
        qDebug("already running, skip start");
        return;
    }

    gameRunning=true;

    if(!gl)
    {
        gl = new GameGLWidget(game);
        qDebug("gamegl %x", gl);
        addWidget(gl);
        setCurrentWidget(gl);
    }


#if defined(Q_OS_SYMBIAN) || defined(MY_OS_MEEGO)
    sensor.start();
#endif

    previousRenderTick = QTime::currentTime();

    qDebug("timer");
    QObject::connect(&timer, SIGNAL(timeout()), this, SLOT(renderTick()));
    //timer.setInterval(16);
    timer.start();

    // resume paused audio
    if(audioFrequency)
    {
        qDebug("resume audio");
        startAudio(audioFrequency, audioChannels);
    }

    if(game) game->notifyEvent(GF::NE_RESUMED, 0);

    qDebug("start done");
}

void QGlGameEngine::pauseGame()
{
    qDebug("pauseGame");

    if(!gameRunning)
    {
        qDebug("not running, skip pause");
    }
    gameRunning=false;

    timer.stop();
    #if defined(Q_OS_SYMBIAN) || defined(MY_OS_MEEGO)
    sensor.stop();
    #endif

    stopAudio();

    game->notifyEvent(GF::NE_PAUSED, 1);
}


#ifdef USE_VKB
void QGlGameEngine::inputMethodEvent(QInputMethodEvent *ie)
{
    ie->accept();
    QByteArray barray = ie->preeditString().toAscii();
    int currentLength = barray.length();
    if (currentLength>lastStringLength) {
        game->keyEvent(GF::KEY_CHARACTER, -(int)barray[currentLength-1]);
    } else {
        // backspace pressed
        if (currentLength<lastStringLength) {
            // implement here.
           game->keyEvent(GF::KEY_CHARACTER, -8);      // backspace
        }
    }
    lastStringLength = currentLength;
}
#endif


void QGlGameEngine::keyPressEvent(QKeyEvent *event)
{
    switch (event->key())
    {
    case Qt::Key_Up:
        game->keyEvent(GF::KEY_DOWN, GF::UP);
        break;
    case Qt::Key_Down:
        game->keyEvent(GF::KEY_DOWN, GF::DOWN);
        break;
    case Qt::Key_Space:
        game->keyEvent(GF::KEY_DOWN, GF::FIRE);
        break;
    default:
        break;
    }

#ifdef USE_VKB
        QByteArray barray = event->text().toAscii();
        if (barray.length() > 0) {
            qDebug("keyPressEvent characters '%s'", barray.data());
            game->keyEvent(GF::KEY_CHARACTER, -(int)barray[0]);
        }
#endif

}

bool QGlGameEngine::event(QEvent *event)
{
    switch (event->type())
    {
    case QEvent::TouchBegin:
    case QEvent::TouchUpdate:
    case QEvent::TouchEnd:
    {
        QList<QTouchEvent::TouchPoint> touchPoints = static_cast<QTouchEvent *>(event)->touchPoints();
        foreach (const QTouchEvent::TouchPoint &touchPoint, touchPoints)
        {
            int ind = touchPoint.id();
            switch (touchPoint.state()) {
            case Qt::TouchPointPressed:
            {
                QPoint p = touchPoint.pos().toPoint();
                game->mouseEvent( GF::MOUSE_DOWN, p.x(), p.y(), ind );
                continue;
            }
            case Qt::TouchPointMoved:
            {
                QPoint p = touchPoint.pos().toPoint();
                game->mouseEvent( GF::MOUSE_MOVE, p.x(), p.y(), ind );
                continue;
            }
            case Qt::TouchPointReleased:
            {
                QPoint p = touchPoint.pos().toPoint();
                game->mouseEvent( GF::MOUSE_UP, p.x(), p.y(), ind );
                continue;
            }
            default:
            case Qt::TouchPointStationary:
            {
                // The touch point did NOT move
                continue;
            }
            }
        }
        event->accept();
        break;
    }
    case QEvent::MouseButtonPress:
        qDebug("MouseButtonPress");
    {
        QMouseEvent *me = static_cast<QMouseEvent*>(event);
        game->mouseEvent( GF::MOUSE_DOWN, me->x(), me->y(), 0);
    }
        break;
    case QEvent::MouseMove:
    {
        QMouseEvent *me = static_cast<QMouseEvent*>(event);
        game->mouseEvent( GF::MOUSE_MOVE, me->x(), me->y(), 0);
    }
        break;
    case QEvent::MouseButtonRelease:
        qDebug("MouseButtonRelease");
    {
        QMouseEvent *me = static_cast<QMouseEvent*>(event);
        game->mouseEvent( GF::MOUSE_UP, me->x(), me->y(), 0);
    }
        break;
    case QEvent::WindowActivate:
        qDebug("WindowActivate");
        if(!splashScreen) startGame();
        break;
    case QEvent::WindowDeactivate:
        qDebug("WindowDeactivate");
        if(!splashScreen) pauseGame();
        break;

    default:
        return QWidget::event(event);
    }
    return true;
}

int QGlGameEngine::loadGLTexture( const char *fileName )
{
    qDebug("loadGLTexture %s", fileName);

    if(!gl)
    {
        qDebug("No GL context initialized yet");
        return 0;
    }

    QString name = QString(":/")+QString(fileName);
    GLuint handle = 0;
    QImage texture;
    if(texture.load(name))
    {
        // bindTexture has flipped y with default bind options
        handle=gl->bindTexture(texture, GL_TEXTURE_2D, GL_RGBA, QGLContext::LinearFilteringBindOption | QGLContext::MipmapBindOption);
    }
    else
    {
        qDebug("load texture failed");
    }
    qDebug("loadGLTexture ret %d", handle);
    return handle;
}

void QGlGameEngine::releaseTexture(int textureHandle) {
    qDebug("releaseTexture %d", textureHandle);
    if(gl) gl->deleteTexture(textureHandle);
}

int QGlGameEngine::openUrlInBrowser(const char *path8) {
    qDebug("openUrlInBrowser %s", path8);
    QDesktopServices::openUrl(QUrl(QString(path8)));
    return 0;
}

void QGlGameEngine::exit()
{
    QApplication::exit(0);
}

void QGlGameEngine::sendEvent(GF::ENGINE_EVENT_TYPE type, int flags)
{
    switch(type)
    {
    case GF::HAPTIC_FEEDBACK:
#ifdef USE_HAPTIC
        QFeedbackEffect::playThemeEffect(QFeedbackEffect::ThemeBasicButton);
#endif
        break;
    case GF::VIRTUAL_KEYBOARD:
#ifdef USE_VKB
        if(flags) {
            qDebug() << "Opening VKB.";
            lastStringLength = 0;
            QApplication::postEvent(this,new QEvent(QEvent::RequestSoftwareInputPanel));
        } else {
            qDebug() << "Closing VKB.";
            QApplication::postEvent(this,new QEvent(QEvent::CloseSoftwareInputPanel));
        }
#endif
        break;
    case GF::EXIT:
        QApplication::exit(0);
        break;
    default:
        break;
    }
}

bool QGlGameEngine::queryTextInput( const char *prompt, char *target, int targetLength )
{
    setUpdatesEnabled(false); // prevents WSERV 10 panic when rotating display while dialog is shown
    QString input = QInputDialog::getText( this, QString(prompt), QString(prompt) );
    setUpdatesEnabled(true);

    QByteArray chinput = input.toLatin1();
    chinput.truncate(targetLength-1);
    strcpy(target, chinput.constData());

#if defined(Q_OS_SYMBIAN)
    // symbian shows cba buttons after text dialog,
    // restoring fullscreen seems to cure it
    showFullScreen();
#endif
    return true;
}


bool QGlGameEngine::startAudio( int frequency, int channels )
{
    // TODO: GE API has fixed frequency, we should be able to pass frequency to the device
    qDebug("startAudio %d, %d", frequency, channels);

    audioFrequency = frequency;
    audioChannels = channels;

#ifdef USE_PUSH_AUDIO_OUT
    if(!m_aout)
    {
        m_aout = new GE::PushAudioOut( this, this );
        m_aout->startAudio();

        // symbian needs timer to
        if(m_aout->needsManualTick())
        {
            QObject::connect(&audioTimer, SIGNAL(timeout()), this, SLOT(audioTick()));
            audioTimer.setInterval(10);
            audioTimer.start();
        }
    }
#endif
#ifdef USE_PULL_AUDIO_OUT
    if (m_aout == 0)
    {
        qDebug() << "creating new pullaudioout ... ";
        m_aout = new GE::PullAudioOut( this, this );
        m_aout->startAudio();
    }
#endif
}

void QGlGameEngine::stopAudio()
{
    qDebug("stopAudio");
#ifdef USE_PUSH_AUDIO_OUT
    if(m_aout)
    {
        audioTimer.stop();
        m_aout->stopAudio();
        delete m_aout;
        m_aout=NULL;
    }
#endif
#ifdef USE_PULL_AUDIO_OUT
    if (m_aout)
    {
        m_aout->stopAudio();
        delete m_aout;
        m_aout = NULL;
    }
#endif
}

#ifdef USE_PUSH_AUDIO_OUT
void QGlGameEngine::audioTick()
{
    //qDebug("audioTick");
    if (m_aout && m_aout->needsManualTick()) m_aout->tick();
}
#endif

int QGlGameEngine::pullAudio( AUDIO_SAMPLE_TYPE *target, int sampleCount )
{
    game->readAudioStream(target, sampleCount);
    return sampleCount;
}

const char *QGlGameEngine::adjustPath(const char *inputPath, char *outputPath, int outputPathLength, bool *isOk)
{
    qDebug("adjustPath %s", inputPath);
    if(isOk) *isOk=outputPathLength>strlen(inputPath);

    outputPath[0]=':';
    outputPath[1]='/';
    return strncpy(outputPath+2, inputPath, outputPathLength-2);
}

void* QGlGameEngine::loadBinaryResource( const char *fileName, int &sizeTarget )
{
    qDebug() << "Loading binary resource : " << QString( fileName );
    // Load file from Qt resource
    QFile *qfile = new QFile(QString(":/") +QString( fileName ), this);
    sizeTarget = 0;
    if (qfile->open( QFile::ReadOnly) == false) return 0;
    sizeTarget = (int)qfile->size();
    if (sizeTarget<1) return 0;
    uchar *ptr=qfile->map(0, sizeTarget);
    qDebug("Resource mapped to %x", ptr);
    resourceMap[ptr]=qfile;
    return ptr;
}

void QGlGameEngine::releaseBinaryResource( void *resource )
{
    uchar *ptr = (uchar*)resource;
    if(resourceMap.contains(ptr))
    {
        qDebug() << "Release binary resource : " << resourceMap[ptr]->fileName();
        resourceMap[ptr]->unmap(ptr);
        resourceMap[ptr]->close();
        resourceMap.remove(ptr);
    }
}

void QGlGameEngine::renderTick()
{
//    qDebug("renderTick");

#if defined(Q_OS_SYMBIAN) || defined(MY_OS_MEEGO)
    QAccelerometerReading *acc=sensor.reading();
    if(acc)
    {
        // send acceleration data to game as 16.16 fixedpoint
        // qDebug("sensor %1.3f, %1.3f, %1.3f", acc->x(), acc->y(), acc->z() );
        game->sensorEvent(GF::ACCELEROMETER, acc->x()*65536, acc->y()*65536, acc->z()*65536);
    }
#endif

//    qDebug("game %x gl %x", game, gl);
    if(game && gl)
    {
        QTime now = QTime::currentTime();


        int ms = previousRenderTick.msecsTo(now);

        previousRenderTick=now;


        if(ms>1000) ms=1000;

        game->update(ms/1000.0f);
        gl->updateGL();
    }
    else
    {
    }
}
