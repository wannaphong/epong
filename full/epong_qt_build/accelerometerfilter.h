/*
 * Copyright (c) 2011 Nokia Corporation.
 */

/*
 * Copyright (c) 2011 Nokia Corporation.
 * Class the interpret the accelerometer data
 *
 */

#ifndef ACCELEROMETERFILTER_H
#define ACCELEROMETERFILTER_H

#include <QObject>
//! [0]
#if defined(Q_OS_SYMBIAN) || defined(MY_OS_MEEGO)
#include <QAccelerometerFilter>
#include <QtCore/QVariant>
QTM_USE_NAMESPACE
#endif

#ifdef EPONG_DEBUG
#include <QTime>
#endif


class AccelerometerFilter
    : public QObject
#if defined(Q_OS_SYMBIAN) || defined(MY_OS_MEEGO)
      ,public QAccelerometerFilter
#endif
{
    Q_OBJECT


public:
    AccelerometerFilter();
#if defined(Q_OS_SYMBIAN) || defined(MY_OS_MEEGO)
    bool filter(QAccelerometerReading *reading);
#endif

    double readings[3];
#ifdef EPONG_DEBUG
private:
    unsigned int m_lastSensorsPerSecondTime;
    int m_frameCount;
#endif


};
//! [0]

#endif // ACCELEROMETERFILTER_H
