/**
 * Copyright (c) 2011-2014 Microsoft Mobile.
 *
 * Part of the Qt GameEnabler.
 */

#include "pushaudioout.h"
#include "trace.h" // For debug macros

#if defined(QTGAMEENABLER_USE_VOLUME_HACK) && defined(Q_OS_SYMBIAN)
    #include <sounddevice.h>
#endif

// Constants
const int GEThreadSleepTime(1); // Milliseconds

using namespace GE;


/*!
  \class PushAudioOut
  \brief An object deploying QAudioOutput for sending the pre-mixed/processed
         audio data into an actual audio device.
*/


/*!
  Constructor.
*/
PushAudioOut::PushAudioOut(AudioSource *source, QObject *parent /* = 0 */)
    : QThread(parent),
      m_audioOutput(0),
      m_outTarget(0),
      m_sendBuffer(0),
      m_sendBufferSize(0),
      m_samplesMixed(0),
      m_threadState(NotRunning),
      m_source(source)
{
    DEBUG_INFO(this);

    QAudioFormat format;
    format.setFrequency(AUDIO_FREQUENCY);
    format.setChannels(AUDIO_CHANNELS);
    format.setSampleSize(AUDIO_SAMPLE_BITS);
    format.setCodec(GEDefaultAudioCodec);
    switch (GEByteOrder) {
    case BigEndian:
        format.setByteOrder(QAudioFormat::BigEndian);
        break;
    case LittleEndian:
        format.setByteOrder(QAudioFormat::LittleEndian);
        break;
    default:
        DEBUG_INFO("INVALID BYTE ORDER");
    }
    switch (GESampleType) {
    case Unknown:
        format.setSampleType(QAudioFormat::Unknown);
        break;
    case SignedInt:
        format.setSampleType(QAudioFormat::SignedInt);
        break;
    case UnSignedInt:
        format.setSampleType(QAudioFormat::UnSignedInt);
        break;
    case Float:
        format.setSampleType(QAudioFormat::Float);
        break;
    default:
        DEBUG_INFO("INVALID SAMPLE TYPE");
    }
    QAudioDeviceInfo info(QAudioDeviceInfo::defaultOutputDevice());

    if (!info.isFormatSupported(format))
        format = info.nearestFormat(format);

    m_audioOutput = new QAudioOutput(info, format);

#if defined(Q_WS_MAEMO_5) || defined(MEEGO_EDITION_HARMATTAN)
    m_sendBufferSize = 4096 * 4;
#else
    m_audioOutput->setBufferSize(4096 * 4);
#endif

#if defined(Q_WS_MAEMO_5) || defined(MEEGO_EDITION_HARMATTAN)
    m_audioOutput->setBufferSize(4096 * 16);
    m_sendBufferSize = 4096 * 8;
#else
    m_audioOutput->setBufferSize(4096 * 4);
    m_sendBufferSize = 4096 * 2;
#endif

    DEBUG_INFO("Buffer size: " << m_audioOutput->bufferSize());
    m_sendBuffer = new AUDIO_SAMPLE_TYPE[m_sendBufferSize];

    m_needsTick = false;
#if defined(QTGAMEENABLER_USE_VOLUME_HACK) && defined(Q_OS_SYMBIAN)
    DEBUG_INFO("WARNING: Using the volume hack!");

    // This really ugly hack is used as the last resort. This allows us to
    // adjust the application volume in Symbian. The CMMFDevSound object lies
    // deep inside the QAudioOutput in Symbian implementation and it has the
    // needed functions. So, we dig the required object accessing it directly
    // from memory.
    unsigned int *pointer_to_abstract_audio =
            (unsigned int*)((unsigned char*)m_audioOutput + 8);

    unsigned int *dev_sound_wrapper =
            (unsigned int*)(*pointer_to_abstract_audio) + 13;

    unsigned int *temp = ((unsigned int*)(*dev_sound_wrapper) + 6);

    CMMFDevSound *devSound = (CMMFDevSound*)(*temp);
    devSound->SetVolume(devSound->MaxVolume() * 6 / 10);
#endif

}


/*!
  Destructor.
*/
PushAudioOut::~PushAudioOut()
{
    if (m_threadState == DoRun) {
        // Set the thread to exit run().
        m_threadState = DoExit;
    }

    if (QThread::isRunning() == false) {
        m_threadState = NotRunning;
    }

    while (m_threadState != NotRunning) {
        // Wait until the thread is finished.
        msleep(50);
    }

    m_audioOutput->stop();

    delete m_audioOutput;
    delete [] m_sendBuffer;
}

/*!
  TODO: Document what this method actually does and why it is needed.

  Call this method manually only if you are not using a thread (with Symbian).

  Note: When using Qt GameEnabler, the GameWindow instance owning this AudioOut
  instance will handle calling this method and you should not try to call this
  explicitly.
*/
void PushAudioOut::tick()
{
    if (!m_outTarget) return;

    // QAudioOutput does not use period size on Symbian, and periodSize
    // returns always the bufferSize
#ifdef Q_OS_SYMBIAN
    // Fill data to the buffer as much as there is free space available.
    int samplesToWrite(m_audioOutput->bytesFree() / sizeof(AUDIO_SAMPLE_TYPE));
    if (samplesToWrite <= 0)
        return;
#else // !Q_OS_SYMBIAN
    // Use multiples of period size
    int samplesToWrite(m_audioOutput->bytesFree() / m_audioOutput->periodSize());
    if (!samplesToWrite)
        return;
    samplesToWrite *= m_audioOutput->periodSize() / sizeof(AUDIO_SAMPLE_TYPE);
#endif // Q_OS_SYMBIAN

    if (samplesToWrite > m_sendBufferSize)
        samplesToWrite = m_sendBufferSize;

    int mixedSamples = m_source->pullAudio(m_sendBuffer, samplesToWrite);
    m_outTarget->write((char*)m_sendBuffer,
        mixedSamples * sizeof(AUDIO_SAMPLE_TYPE));
}


/*!
  From QThread.

  Used only in threaded solutions.
*/
void PushAudioOut::run()
{
    DEBUG_INFO("Starting thread.");
    m_threadState = DoRun;

    while (m_threadState == DoRun) {
        tick();
        msleep(GEThreadSleepTime);
    }

    DEBUG_INFO("Exiting thread.");
    m_threadState = NotRunning;
}

bool PushAudioOut::startAudio()
{
    m_outTarget = m_audioOutput->start();
#ifndef Q_OS_SYMBIAN
    start();
#else
    m_needsTick = true;
#endif
}

void PushAudioOut::stopAudio()
{
    m_needsTick = false;
#ifndef Q_OS_SYMBIAN
    if (m_threadState == DoRun) {
        // Set the thread to exit run().
        m_threadState = DoExit;
    }

    if (QThread::isRunning() == false) {
        m_threadState = NotRunning;
    }

    while (m_threadState != NotRunning) {
        // Wait until the thread is finished.
        msleep(50);
    }
#endif
    m_audioOutput->stop();
    m_outTarget = 0;
}

