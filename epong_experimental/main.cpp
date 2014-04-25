/*
 * Copyright (c) 2011-2014 Microsoft Mobile.
 */

#include <QtGui/QApplication>
#include "qglgameengine.h"
#include <QDebug>

int main(int argc, char *argv[])
{
    qDebug("main");
    QApplication a(argc, argv);

    QGlGameEngine widget;

#ifndef Q_OS_WIN
    widget.showFullScreen();
#else
    widget.setMinimumSize(640,360);
    widget.show();
#endif

    widget.raise();

    int rval = a.exec();
    qDebug("main ret %d", rval);
    return rval;
}
