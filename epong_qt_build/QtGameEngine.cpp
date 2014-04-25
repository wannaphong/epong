/*
 * Copyright (c) 2011-2014 Microsoft Mobile.
 *
 */

#include <QDesktopServices>
#include <QUrl>
#include <QApplication>
#include <QTime>
#include <QImage>
#include <QtGlobal>
#include <QResizeEvent>

#ifdef USE_GLES11
#include <GLES/gl.h>
#include "eglutilsgles.h"
#endif
#ifdef USE_GLES20
#include <GLES2/gl2.h>
#include "eglutilsgles.h"
#endif
#ifdef USE_OPENVG
#include "eglutilsvg.h"
#endif


#include <QKeyEvent>
#include "pushaudioout.h"
#include "pullaudioout.h"


// for text input
#include <QString>
#include <QInputDialog>


#ifdef Q_OS_LINUX
#include <QX11Info>
#endif

#ifdef Q_OS_SYMBIAN
#include <mw/apgtask.h>
#include <mw/coecntrl.h>
#include <w32std.h>
#endif

#include <QDesktopServices>
#include <QtCore/QDebug>
#include <QDir>
#include "QtGameEngine.h"
#include "GameApp.h"

#ifdef MY_OS_MEEGO
#include <QX11Info>
#include <X.h>
#include <Xlib.h>
#include <Xatom.h>
#endif
#if defined(Q_OS_SYMBIAN) || defined(Q_WS_MAEMO_5) || defined(MY_OS_MEEGO)
    #include <QSystemDeviceInfo>
    #include "accelerometerfilter.h"
    QTM_USE_NAMESPACE
#endif

#ifdef Q_OS_SYMBIAN
    // TODO: Where exists these values?
    static const int KGoomMemoryLowEvent = 0x10282DBF;
    static const int KGoomMemoryGoodEvent = 0x20026790;

    #include <remconcoreapitarget.h>
    #include <remconinterfaceselector.h>
#endif

#if !defined(Q_OS_WIN32) && !defined(Q_WS_X11)
#include <QFeedbackEffect>
#endif

#ifdef MY_OS_MEEGO
#include <QDesktopWidget>
#include <QApplication>
#endif

using namespace GF;

QtGameEngine::QtGameEngine(QWidget *parent) :
    QWidget(parent),
    profileSilent(false),
    currentVolumeSlot(80),
    m_paused(false),
    eglUtils(0)
{
#if defined(Q_WS_X11)
    m_paused=true;
#endif
    m_pauseResumeDisabled = false;
    m_updatesDisabled = false;
    touchEventsEnabled = false;
    runningGame = 0;
    timerid = 0;
    setAutoFillBackground( false );
    setAttribute (Qt::WA_OpaquePaintEvent);
    setAttribute( Qt::WA_NoSystemBackground );
    setAttribute( Qt::WA_NativeWindow );
    setAttribute( Qt::WA_PaintOnScreen, true );
    setAttribute( Qt::WA_StyledBackground,false);
    setAttribute( Qt::WA_PaintUnclipped );
    m_aout = 0;

#if defined(Q_OS_SYMBIAN) || defined(MY_OS_MEEGO)
    setAttribute( Qt::WA_AcceptTouchEvents );
#endif
    acceleration = new AccelerometerFilter();
#if defined(Q_OS_SYMBIAN) || defined(MY_OS_MEEGO)
    sensor.addFilter(acceleration);
#endif
#ifdef MY_OS_MEEGO
    m_swipeEnabled = true;
#endif
#ifdef MY_OS_MEEGO
     QDesktopWidget* desk = QApplication::desktop();
     if(desk) {
        int width  = desk->width();
        int height = desk->height();
        qDebug() << "width" << width << "height" << height;
        setMinimumHeight(height);
        setMinimumWidth(width);
     }
#endif


    m_currentTime = 0;
    m_prevTime = m_currentTime;
    m_fps = 0.0f;
    m_frameTime = 0.0f;

#ifdef Q_OS_SYMBIAN
        // Create'n'launch the Symbian hw-volume-key observer
    Observer *m_Observer = new Observer(this);
    m_Selector = CRemConInterfaceSelector::NewL();
    m_Target = CRemConCoreApiTarget::NewL(*m_Selector, *m_Observer);
    m_Selector->OpenTargetL();
#endif
#ifdef IAP
    m_miniIAPClient = 0;
#endif
}

unsigned int QtGameEngine::getTickCount() {
    QTime now = QTime::currentTime();
    return (now.hour()*3600+now.minute()*60 + now.second())*1000 + now.msec();
}

void QtGameEngine::exit() {
    qApp->exit(0);
};


QtGameEngine::~QtGameEngine() {
    //qDebug()<< "QtGameEngine::~QtGameEngine()";

    if (m_aout) {
        delete m_aout;
        m_aout = 0;
    }


#ifdef Q_OS_SYMBIAN
    delete m_Target;
#endif
#ifdef IAP
    if (m_miniIAPClient) delete m_miniIAPClient;
#endif

};

    // Display a textinput dialog and return users input to the requester
bool QtGameEngine::queryTextInput( const char *prompt, char *target, int targetLength ) {
    setUpdateAndPauseState(true, true);
    QString input = QInputDialog::getText( this, QString(prompt), QString(prompt) );
    if (input.isEmpty()) {
        strcpy (target, "");
    	setUpdateAndPauseState(false, false);
#ifdef MY_OS_MEEGO
        eglUtils->clearContext();
#endif
        return false;
    }
    QByteArray chinput = input.toLatin1();
    chinput.truncate(targetLength-1);
    const char *rdata =chinput.constData();
    strcpy( target, rdata );
#ifdef MY_OS_MEEGO
    eglUtils->setCurrent();
#endif
    setUpdateAndPauseState(false, false);
    return true;
};


int QtGameEngine::openUrlInBrowser(const char *path8) {
    //qDebug() << "Opening url in browser : " << QString( path8 );
    QDesktopServices::openUrl(QUrl(QString(path8)));
    return 0;
};


void QtGameEngine::volumeUp() {
    currentVolumeSlot+=10;
    if (currentVolumeSlot>100) currentVolumeSlot = 100;
    if (runningGame)
        runningGame->notifyEvent( NE_VOLUME_CHANGED, currentVolumeSlot );
};


void QtGameEngine::volumeDown() {
    currentVolumeSlot-=10;
    if (currentVolumeSlot<0) currentVolumeSlot = 0;
    if (runningGame)
        runningGame->notifyEvent( NE_VOLUME_CHANGED, currentVolumeSlot );
};


bool QtGameEngine::isProfileSilent() {
#if !defined(Q_OS_WIN32) && !defined(Q_WS_X11)
    QSystemDeviceInfo info;
    if (info.currentProfile() == QSystemDeviceInfo::SilentProfile) return true; else return false;
#else
    return false;
#endif
};


int QtGameEngine::pullAudio( AUDIO_SAMPLE_TYPE *target, int sampleCount ) {
    if (!runningGame) return 0;
    if (m_paused) return 0;
    runningGame->readAudioStream( target, sampleCount );
    return sampleCount;
};

bool QtGameEngine::startSensors() {
    //qDebug() << "QtGameEngine::startSensors";
#ifdef MY_OS_MEEGO
    Display *dpy = QX11Info::display();
    Atom atom;

    unsigned int customRegion[4];
    customRegion[0] = 0;
    customRegion[1] = 0;
    customRegion[2] = 854;
    customRegion[3] = 480;

    atom = XInternAtom(dpy, "_MEEGOTOUCH_CUSTOM_REGION", False);
    XChangeProperty(dpy, effectiveWinId(),
            atom, XA_CARDINAL, 32, PropModeReplace,
            reinterpret_cast<unsigned char *>(&customRegion[0]), 4);
    m_swipeEnabled = false;

    // for periodical temporary blanking pause
    //qDebug() << "preventing blanking";
    qmDisplayState.setBlankingPause();
#endif
#if defined(Q_OS_SYMBIAN) || defined(MY_OS_MEEGO)
    return sensor.start();
#else
    return false;
#endif
}

void QtGameEngine::stopSensors() {
    //qDebug() << "QtGameEngine::stopSensors";
#ifdef MY_OS_MEEGO
    Display *dpy = QX11Info::display();
    Atom atom;

    atom = XInternAtom(dpy, "_MEEGOTOUCH_CUSTOM_REGION", False);
    if(XDeleteProperty(dpy, effectiveWinId(), atom) < 0){
        qWarning("XDeleteProperty for _MEEGOTOUCH_CUSTOM_REGION returns <0");
    }
    m_swipeEnabled = true;
#endif
#if defined(Q_OS_SYMBIAN) || defined(MY_OS_MEEGO)
    sensor.stop();
#endif
}

bool QtGameEngine::eventFilter(QObject *object, QEvent *event) {
    // http://doc.trolltech.com/4.7/qevent.html#QEvent
    if (event->type() == QEvent::ActivationChange) {
        if (m_paused) m_paused = false; else m_paused = true;
        qDebug() << "ActivationChangeFiltered: paused ==" << m_paused;
        if (!m_pauseResumeDisabled) {
            if (m_paused) pause(); else resume();
        } else {
            qDebug() << "Ignoring paused/resume since textintputdialog is on.";
        }
        return false;
    }
#ifdef MY_OS_MEEGO
    else if (event->type() == QEvent::WindowStateChange )
    {
        QWindowStateChangeEvent *change = static_cast<QWindowStateChangeEvent *>(event);

        Qt::WindowStates current = windowState();
        Qt::WindowStates old = change->oldState();
        //qDebug() << "widget is minimized in eventFilter?" << isMinimized();

        // did minimized flag change?
        if ((current ^ old) & Qt::WindowMinimized) {
            if (current & Qt::WindowMinimized) {
                // In MeeGo, the gl surface is somehow broken by the QMeegoGraphicsSystem.
                // This is why we must kill the timer, to prevent errors resulting gl painting.
                //qDebug() << "HIDING";
                killTimer(timerid);
                timerid = 0;
            }
        }
        return false;
    }
#endif
    else {
        // Standard event processing
        return QObject::eventFilter(object, event);
    }
    return false;
}

void QtGameEngine::destroy() {
    qDebug() << "QtGameEngine::destroy()";
    if (runningGame) {
        runningGame->release();
        delete runningGame;
        runningGame = 0;
    }
    if (eglUtils) delete eglUtils;
    eglUtils = 0;
    if (m_aout) delete m_aout;
    m_aout = 0;
    killTimer(timerid);
    timerid = 0;

};

void QtGameEngine::create() {
    setAttribute(Qt::WA_NoSystemBackground);
    createEGLUtils();

    m_currentTime = getTickCount();
    m_prevTime = m_currentTime;
    m_fps = 0.0f;
    m_frameTime = 0.0f;

    runningGame = createGameApplication( this );

    // audioout
#ifdef USE_PUSH_AUDIO_OUT
    m_aout = new GE::PushAudioOut( this, this );
#else
    m_aout = new GE::PullAudioOut(this, this);
#endif
    //qDebug() << "Calling resume";
    resume();
    //qDebug() << "::create finished()";
}


void QtGameEngine::doFrame() {
    m_prevTime = m_currentTime;
    m_currentTime = getTickCount();
    m_frameTime = (float)( m_currentTime - m_prevTime ) * 0.001f;
    if (m_frameTime>0.05f) m_frameTime = 0.05f;

#ifdef EPONG_DEBUG
    // count how many frames displayed in a second and calculate fps
    m_frameCount++;
    unsigned int delta=m_currentTime-m_prevFpsTime;
    if (delta > 1000)
    {
        m_fps = m_frameCount / ((float)(delta) * 0.001f);
        m_prevFpsTime=m_currentTime;
        m_frameCount=0;
        QString str;
        str.sprintf("%.2f fps",m_fps);
        qDebug() << str;
    }
#endif
    if (eglUtils != 0) eglUtils->clear();

    if (!m_updatesDisabled) {
        // 1st audio update
        if (m_aout && m_aout->needsManualTick() == true)
            m_aout->tick(); // Manual tick

        if (runningGame) {
            runningGame->sensorEvent( ACCELEROMETER,
                                     (int)(acceleration->readings[0] * 65536.0f),
                                     (int)(acceleration->readings[1] * 65536.0f),
                                     (int)(acceleration->readings[2] * 65536.0f) );

            runningGame->update( m_frameTime );
        }

        // 2nd audio update
        if (m_aout && m_aout->needsManualTick() == true)
            m_aout->tick(); // Manual tick

        if (runningGame) {
            runningGame->render();
        };


        if (eglUtils != 0 && !eglUtils->swapBuffers()) {

            // fatal error, cleanup and exit
            qApp->exit(0);
        }
    }
    else {
        update();
    }
}

bool QtGameEngine::createEGLUtils() {
    if (eglUtils) delete eglUtils;
    eglUtils = 0;

    int display = -1;            // = EGL_DEFAULT_DISPLAY;
    #if defined(Q_OS_LINUX) || defined(MY_OS_MEEGO)
        display  = (int)this->x11Info().display();
    #endif
    #ifdef Q_OS_WIN32
        HWND hwnd = this->winId();
        HDC dc = GetWindowDC( hwnd );
        display = (int)dc;
    #endif
    #ifdef Q_OS_SYMBIAN
        display = -1;
    #endif


    int pwd = -1;
#ifdef Q_OS_SYMBIAN
    pwd = (int)(void*)(this->winId()->DrawableWindow());
#else
    pwd = (int)this->winId();
#endif



#ifdef USE_OPENVG
    eglUtils = new EGLUtilsVG();
#else
    eglUtils = new EglUtilsGles();
#endif

    return eglUtils->createWindow(display, pwd);
}

void QtGameEngine::resizeEvent( QResizeEvent *event ) {
    if (event->size().width() == event->oldSize().width() &&
            event->size().height() == event->oldSize().height()) return;

    int w = event->size().width();
    int h = event->size().height();
    //qDebug() << "Setting viewport..";

    // TODO move to eglutils
#if defined(USE_GLES11) || defined(USE_GLES20)
    glViewport(0,0, w, h);
#endif
    //qDebug() << "resizeEvent: " << w << "," << h;

    if (runningGame) {
        runningGame->resize( w, h );
    };
    // Should we call resize here?

    QWidget::resizeEvent( event );
};


void QtGameEngine::keyPressEvent( QKeyEvent *event )
{
    switch (event->key()) {
        case Qt::Key_Up:
            runningGame->keyEvent( GF::KEY_DOWN, GF::UP );
            break;

        case Qt::Key_Down:
            runningGame->keyEvent( GF::KEY_DOWN, GF::DOWN );
            break;

        case Qt::Key_Space:
            runningGame->keyEvent( GF::KEY_DOWN, GF::FIRE );
            break;
        default:
            // not handling
            break;
        }
}



    // Basic mouse controlling
void QtGameEngine::mousePressEvent ( QMouseEvent * event ) {
    if (touchEventsEnabled) return;     // Ignore basic event if touches are enabled
    if (runningGame) {
        runningGame->mouseEvent( MOUSE_DOWN, event->x(), event->y(),0 );
        event->accept();
    };
};


void QtGameEngine::mouseReleaseEvent ( QMouseEvent * event ) {
    if (touchEventsEnabled) return;     // Ignore basic event if touches are enabled
    if (runningGame) {
        runningGame->mouseEvent( MOUSE_UP, event->x(), event->y(),0);
        event->accept();
    };
};


void QtGameEngine::mouseMoveEvent ( QMouseEvent * event ) {
    if (touchEventsEnabled) return;     // Ignore basic event if touches are enabled
    if (runningGame) {
        runningGame->mouseEvent( MOUSE_MOVE, event->x(), event->y(),0);
        event->accept();
    };
};


    // For multitouch - controls
bool QtGameEngine::event(QEvent *event)
{
    switch (event->type()) {
    case QEvent::TouchBegin:
    case QEvent::TouchUpdate:
    case QEvent::TouchEnd:
    {
        touchEventsEnabled = true;
        QList<QTouchEvent::TouchPoint> touchPoints = static_cast<QTouchEvent *>(event)->touchPoints();
        foreach (const QTouchEvent::TouchPoint &touchPoint, touchPoints) {
            int ind = touchPoint.id();
            switch (touchPoint.state()) {
            case Qt::TouchPointPressed:
                {
                    QPoint p = touchPoint.pos().toPoint();
                    runningGame->mouseEvent( MOUSE_DOWN, p.x(), p.y(), ind );
                    continue;
                }
            case Qt::TouchPointMoved:
                {
                    QPoint p = touchPoint.pos().toPoint();
                    runningGame->mouseEvent( MOUSE_MOVE, p.x(), p.y(), ind );
                    continue;
                }
            case Qt::TouchPointReleased:
                {
                    QPoint p = touchPoint.pos().toPoint();
                    runningGame->mouseEvent( MOUSE_UP, p.x(), p.y(), ind );
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
#ifdef MY_OS_MEEGO
    case QEvent::WindowStateChange :
    {
        QWindowStateChangeEvent *change = static_cast<QWindowStateChangeEvent *>(event);

        Qt::WindowStates current = windowState();
        Qt::WindowStates old = change->oldState();

        // did minimized flag change?
        if ((current ^ old) & Qt::WindowMinimized) {
            if (current & Qt::WindowMinimized) {
            }
            else {
                // In MeeGo, we need to re-create the EGL here.
                createEGLUtils();

            }
            //qDebug() << "WindowStateChange : " << m_paused << bool(m_updatesDisabled);
        }
        event->accept();
        break;
    }
#endif
    default:
        return QWidget::event(event);
    }
    return true;
}




void QtGameEngine::timerEvent(QTimerEvent *event) {
    doFrame();
}

void QtGameEngine::pause() {
    //qDebug() << "pausing game" << "stopSensors";
    m_aout->stopAudio();

    killTimer(timerid);
    timerid = 0;
    if (runningGame) {
        runningGame->notifyEvent( NE_PAUSED,0 );
        qDebug() << "Calling game::release";
        runningGame->release();
    }
}


void QtGameEngine::sendEvent(ENGINE_EVENT_TYPE type, int flags) {
    switch (type) {
        case HAPTIC_FEEDBACK:
#if !defined(Q_OS_WIN32) && !defined(Q_WS_X11)
         QFeedbackEffect::playThemeEffect(QFeedbackEffect::ThemeBasicButton);
#endif

        break;
    };
};

void QtGameEngine::resume() {

        // wait a little while to make sure profile is updated.
    #ifdef Q_OS_SYMBIAN
        User::After(500000);
    #endif

    profileSilent = isProfileSilent();
    m_aout->startAudio();


    if(timerid)
        return;

    if (runningGame) {
        if (profileSilent) runningGame->notifyEvent( NE_VOLUME_CHANGED, 0);
            else runningGame->notifyEvent( NE_VOLUME_CHANGED, currentVolumeSlot);
        qDebug() << "Preparing game.. ";
        if (runningGame->prepare() == false) {
            // Something went wrong when preparing resources
            //qDebug() << "FAILED";
        } else {
            //qDebug() << "SUCCESS";
        }
        runningGame->notifyEvent( NE_RESUMED,0 );
    }
    timerid = startTimer(0);
}

void* QtGameEngine::loadBinaryResource( const char *fileName, int &sizeTarget )
{
    // FIXME this is baaaad.  should map() the resource data
    qDebug() << "Loading binary resource : " << QString( fileName );
    // Load file from Qt resource
    QFile qfile(QString(":/") +QString( fileName ));

    sizeTarget = 0;
    if (qfile.open( QFile::ReadOnly) == false) return 0;
    sizeTarget = (int)qfile.size();
    if (sizeTarget<1) return 0;
    unsigned char *rval = new unsigned char[ sizeTarget ];
    if (qfile.read( (char*)rval, sizeTarget) != sizeTarget) {
        delete [] rval;
        return 0;
    }
    qDebug() << "success: " << sizeTarget << " bytes.";
    return rval;
}

void QtGameEngine::releaseBinaryResource( void *resource )
{
    if (resource) delete [] resource;
}


int QtGameEngine::loadGLTexture( const char *fn ) {
    qDebug("load '%s'", fn);
    return eglUtils->loadTexture(fn);
}

void QtGameEngine::releaseTexture(int textureHandle)
{
    eglUtils->releaseTexture(textureHandle);
}


    // GOOM EVENT HANDLING
    // http://wiki.forum.nokia.com/index.php/Graphics_memory_handling
#ifdef Q_OS_SYMBIAN
bool QtGameEngine::symbianEventFilter( const QSymbianEvent* symbianEvent ) {
  const TWsEvent *event = symbianEvent->windowServerEvent();

  if( !event ) {
    return false;
  }

  switch( event->Type() ) {
    case EEventUser: {

          TApaSystemEvent* eventData = reinterpret_cast<TApaSystemEvent*>(event->EventData());
          if ((*eventData) == EApaSystemEventShutdown)
          {
                eventData++;
                if ((*eventData) == KGoomMemoryLowEvent)
                {

                    return true;
                }
           }
        break;
    }
  }

  return false;
}
#endif

const char* QtGameEngine::adjustPath(const char *inputPath, char *outputPath, int outputPathLength, bool *isOk) {
    QString resultPath;
    if (strncmp(inputPath,"configdata:",11) == 0) {
        resultPath = (inputPath+11);
        resultPath = QDesktopServices::storageLocation(QDesktopServices::DataLocation) + QLatin1String("/") + resultPath;
        QString tmpPath = resultPath.left(resultPath.lastIndexOf('/'));
        QDir dir(tmpPath);
        if (!dir.exists()) {
            dir.mkpath(dir.path());
        }
    }
    else {
#if defined(Q_OS_UNIX) || defined(MY_OS_MEEGO)
#ifdef Q_OS_MAC
        if (!QDir::isAbsolutePath(path))
            resultPath = QCoreApplication::applicationDirPath()
                         + QLatin1String("/../Resources/") + inputPath;
#else
        const QString pathInInstallDir = QCoreApplication::applicationDirPath()
            + QLatin1String("/../") + inputPath;
        if (pathInInstallDir.contains(QLatin1String("opt"))
                && pathInInstallDir.contains(QLatin1String("bin"))) {
            resultPath = pathInInstallDir;
        }
#endif
        else {
            resultPath = inputPath;
        }
#else
    resultPath = inputPath;
#endif
    }
    QByteArray bytes = resultPath.toUtf8();
    if (bytes.length() >= outputPathLength) {
        qDebug() << "PongApp::adjustPath: Too long filename!";
        if (isOk) (*isOk) = false;
        strcpy(outputPath, "");
    }
    else {
        if (isOk) (*isOk) = true;
        strcpy(outputPath, bytes.constData());
    }
    return outputPath;
}

void QtGameEngine::setUpdateAndPauseState(bool updatesDisabled, bool pauseResumeDisabled)
{
    qDebug() << "QtGameEngine::setUpdateAndPauseState" << updatesDisabled << pauseResumeDisabled;
    if (updatesDisabled && pauseResumeDisabled) {
        killTimer(timerid);
        timerid = startTimer(300);
    }
    else if (m_updatesDisabled && m_pauseResumeDisabled) {
        killTimer(timerid);
        timerid = startTimer(0);
    }
    if (updatesDisabled) {
        m_updatesDisabled = true;
    }
    else {
        m_updatesDisabled = false;
    }
    if (pauseResumeDisabled) {
        m_pauseResumeDisabled = true;
    }
    else {
        m_pauseResumeDisabled = false;
    }
}

#ifdef IAP
MiniIAPClientApi *QtGameEngine::getMiniIAPClient()
{
    if (!m_miniIAPClient)
        m_miniIAPClient = new MiniIAPQtClient();
    return m_miniIAPClient;
}

#endif


#ifdef MY_OS_MEEGO
bool QtGameEngine::isSwipeEnabled()
{
    return m_swipeEnabled;
}
#endif // MY_OS_MEEGO


