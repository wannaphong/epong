/*
 * Copyright (c) 2011-2014 Microsoft Mobile.
 */

#include <math.h>
#include "accelerometerfilter.h"

#ifdef EPONG_DEBUG
#include <QTime>
#include <QDebug>
#endif
AccelerometerFilter::AccelerometerFilter() {
    readings[0] = 0.0f;
    readings[1] = 0.0f;
    readings[2] = 0.0f;
#ifdef EPONG_DEBUG
    m_frameCount = 0;
#endif
}

//! [0]
#if defined(Q_OS_SYMBIAN) || defined(MY_OS_MEEGO)
bool AccelerometerFilter::filter(QAccelerometerReading *reading)
{
#ifdef EPONG_DEBUG
    QTime now = QTime::currentTime();
    m_frameCount ++;
    unsigned int nowInt = (now.hour()*3600+now.minute()*60 + now.second())*1000 + now.msec();
    unsigned int delta = nowInt - m_lastSensorsPerSecondTime;
    if (delta > 1000) {
        float m_fps = m_frameCount / ((float)(delta) * 0.001f);
        m_lastSensorsPerSecondTime=nowInt;
        m_frameCount=0;
        QString str;
        str.sprintf("%.2f sensor readings per second",m_fps);
        qDebug() << str;
    }
#endif
    readings[0] = reading->x();
    readings[1] = reading->y();
    readings[2] = reading->z();
    // Don't store the reading in the sensor.
    return false;
}
#endif
//! [0]

