/*
 * Copyright (c) 2011 Nokia Corporation.
 */


#include <QtCore/QDebug>
#include <QtCore/qstring.h>
#include <QAudioOutput>
#include <QAudioFormat>

#include "GEAudioOut.h"

#ifdef USE_VOLUMEHACK_WARNING_MIGHT_CRASH_WITH_DIFFERENT_SDK
#ifdef Q_OS_SYMBIAN
    #include <SoundDevice.h>
#endif
#endif

using namespace GF;

const int CHANNELS = 2;
const QString CODEC = "audio/pcm";
const QAudioFormat::Endian BYTEORDER = QAudioFormat::LittleEndian;
const QAudioFormat::SampleType SAMTYPE = QAudioFormat::SignedInt;



AudioOut::AudioOut(QObject *parent, GF::IAudioSource *source)
    : QThread(parent)
{
    m_source = source;
    m_usingThread = false;
    QAudioFormat format;
    format.setFrequency(AUDIO_FREQUENCY);
    format.setChannels(CHANNELS);
    format.setSampleSize(AUDIO_SAMPLE_BITS);
    format.setCodec(CODEC);
    format.setByteOrder(BYTEORDER);
    format.setSampleType(SAMTYPE);

    QAudioDeviceInfo info(QAudioDeviceInfo::defaultOutputDevice());
    if (!info.isFormatSupported(format)) {
        //qDebug() << "DEBUG: Audio format WAS NOT supported";
        format = info.nearestFormat(format);
    }

    m_audioOutput = new QAudioOutput(info, format);

#if defined(Q_WS_MAEMO_5) || defined(MY_OS_MEEGO)
    m_sendBufferSize = 4096*4;
#else
    m_audioOutput->setBufferSize(4096*4);
#endif


    m_outTarget = m_audioOutput->start();
    //m_audioOutput->setBufferSize( 262144  );
#if defined(Q_WS_MAEMO_5) || defined(MY_OS_MEEGO)
    m_audioOutput->setBufferSize(4096*16);
    m_sendBufferSize = 4096*8;
#else
    m_audioOutput->setBufferSize(4096*4);
    m_sendBufferSize = 4096*2;
#endif

    m_sendBuffer = new AUDIO_SAMPLE_TYPE[m_sendBufferSize];
    m_samplesMixed = 0;
    m_runstate = 0;


#ifndef Q_OS_SYMBIAN
    m_usingThread = true;
    start();
#else
   m_audioOutput->setNotifyInterval(5);
   connect(m_audioOutput, SIGNAL(notify()), this, SLOT(audioNotify()));

#ifdef USE_VOLUMEHACK_WARNING_MIGHT_CRASH_WITH_DIFFERENT_SDK
   qDebug() << "Using voluemehack... WARNING, this might cause application to crash if SDK is different than the hack is designed for. Undef USE_VOLUMEHACK_WARNING_MIGHT_CRASH_WITH_DIFFERENT_SDK to disable hack..";
   /*
        WARNING!!!!!!

   Really ugly hack is used as a last resort. This allows us to adjust the
   application volume in Symbian. The CMMFDevSound object lies deep
   inside the QAudioOutput in Symbian implementation and it has the needed
   functions. So we get the needed object accessing directly from memory.
   */
   unsigned int *pointer_to_abstract_audio =
        (unsigned int*)((unsigned char*)m_audioOutput + 8);
   unsigned int *dev_sound_wrapper =
        (unsigned int*)(*pointer_to_abstract_audio) + 13;
   unsigned int *temp = ((unsigned int*)(*dev_sound_wrapper) + 6);
   CMMFDevSound *dev_sound = (CMMFDevSound*)(*temp);
   dev_sound->SetVolume(dev_sound->MaxVolume());
#endif
#endif

}


AudioOut::~AudioOut()
{
    if (m_runstate == 0)
        m_runstate = 1;

    if (QThread::isRunning() == false)
        m_runstate = 2;
    else
        wait(5000);
    m_outTarget->close();
    m_audioOutput->stop();

    delete m_audioOutput;
    delete [] m_sendBuffer;
}


void AudioOut::audioNotify()
{
    tick();
}

void AudioOut::tick()
{
    // fill data to buffer as much as free space is available..
    int samplesToWrite = m_audioOutput->bytesFree() /
                         (CHANNELS*AUDIO_SAMPLE_BITS/8);


    samplesToWrite *= 2;

    if (samplesToWrite <= 0)
        return;

    if (samplesToWrite > m_sendBufferSize)
        samplesToWrite = m_sendBufferSize;

    int mixedSamples = m_source->pullAudio(m_sendBuffer, samplesToWrite);

    int wrote = m_outTarget->write((char*)m_sendBuffer, mixedSamples * 2);
}


void AudioOut::run()
{
    //qDebug() << "Starting AudioUpdate thread.";
    if (!m_source) {
        m_runstate = 2;
        return;
    }

    int sleepTime = 1;

    while (m_runstate == 0) {
        tick();
        msleep(sleepTime);
    }
    //qDebug() << "Exiting AudioUpdate thread.";
    m_runstate = 2;
}
