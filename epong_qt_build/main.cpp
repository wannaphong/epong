/*
 * Copyright (c) 2011-2014 Microsoft Mobile.
 */

#include <QtGui/QApplication>
#include "QtGameEngine.h"

#ifdef Q_OS_SYMBIAN
    #include <eikenv.h>
    #include <eikappui.h>
    #include <aknenv.h>
    #include <aknappui.h>
    QTM_USE_NAMESPACE
#endif
#include <QDesktopServices>

#ifdef MY_OS_MEEGO
#include <QMeeGoRuntime>
#endif
#include <QDebug>
#include <QFile>

#ifdef IAP
// somehow, IAP will not work when any debug messages are printed, 
// if there is no pause between instantation and usage of IAPClient.
void myMessageHandler(QtMsgType type, const char *msg)
{
    QString txt;
    switch (type) {
    case QtDebugMsg:
        txt = QString("Debug: %1").arg(msg);
        break;
    case QtWarningMsg:
        txt = QString("Warning: %1").arg(msg);
        break;
    case QtCriticalMsg:
        txt = QString("Critical: %1").arg(msg);
        break;
    case QtFatalMsg:
        txt = QString("Fatal: %1").arg(msg);
        abort();
    }
    QFile outFile("c:/data/EPong.log");
    outFile.open(QIODevice::WriteOnly | QIODevice::Append);
    QTextStream ts(&outFile);
    ts << txt << endl;
}
#endif

int main(int argc, char *argv[])
{
#ifdef MY_OS_MEEGO
    QApplication::setGraphicsSystem("runtime");
//    QMeeGoRuntime::switchToMeeGo();
//    QMeeGoRuntime::setSwitchPolicy(QMeeGoGraphicsSystemHelper::ManualSwitch);
#endif
    QApplication a(argc, argv);
#ifdef IAP
// somehow, IAP will not work when any debug messages are printed, 
// if there is no pause between instantation and usage of IAPClient.
//    qInstallMsgHandler(myMessageHandler);
#endif

    // Lock orientation to landscape
#ifdef Q_OS_SYMBIAN
    CAknAppUi* appUi = dynamic_cast<CAknAppUi*>(CEikonEnv::Static()->AppUi());
    appUi->SetOrientationL(CAknAppUi::EAppUiOrientationLandscape);
#endif

    GF::QtGameEngine *qtge = new GF::QtGameEngine();

    qtge->create();
#if defined(Q_OS_SYMBIAN) || defined(MY_OS_MEEGO)
        qtge->showMaximized();
#else
        qtge->show();
#endif



    qtge->setWindowState(Qt::WindowNoState);
    qApp->installEventFilter(qtge);


#ifdef Q_OS_WIN32
    //qtge.showMaximized();
    qtge->resize(640*2,360*2);
#else
    qtge->showFullScreen();
#endif

    int rval = a.exec();
    qtge->destroy();
    delete qtge;
    return rval;
}
