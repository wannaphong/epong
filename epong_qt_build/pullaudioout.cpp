/**
 * Copyright (c) 2011-2014 Microsoft Mobile.
 *
 * Part of the Qt GameEnabler.
 */

#include "pullaudioout.h"
#include "trace.h" // For debug macros
#include <qglobal.h>
//#include <QDebug>
#if defined(QTGAMEENABLER_USE_VOLUME_HACK) && defined(Q_OS_SYMBIAN)
    #include <sounddevice.h>
#endif

using namespace GE;

/**
 *
 * Pull mode solution
 *
 */


PullAudioOut::PullAudioOut(AudioSource *source, QObject *parent)
    : QIODevice(parent),
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

    m_sendBufferSize = 4096;
    m_audioOutput = new QAudioOutput(info, format);

#if defined(QTGAMEENABLER_USE_VOLUME_HACK) && defined(Q_OS_SYMBIAN)
    DEBUG_INFO("WARNING: Using the volume hack!");

    // This really ugly hack is used as the last resort. This allows us to
    // adjust the application volume in Symbian. The CMMFDevSound object lies
    // deep inside the QAudioOutput in Symbian implementation and it has the
    // needed functions. So, we get the needed object accessing it directly
    // from memory.
    unsigned int *pointer_to_abstract_audio =
            (unsigned int*)((unsigned char*)m_audioOutput + 8);

    unsigned int *dev_sound_wrapper =
            (unsigned int*)(*pointer_to_abstract_audio) + 13;

    unsigned int *temp = ((unsigned int*)(*dev_sound_wrapper) + 6);

    CMMFDevSound *devSound = (CMMFDevSound*)(*temp);
    devSound->SetVolume(devSound->MaxVolume() * 6 / 10);
#endif

    open(QIODevice::ReadOnly | QIODevice::Unbuffered);
    connect(m_audioOutput, SIGNAL(stateChanged(QAudio::State)),
        SLOT(audioStateChanged(QAudio::State)));
}

void PullAudioOut::audioStateChanged(QAudio::State state)
{
    Q_UNUSED(state);
    DEBUG_INFO("AudioStateChanged:" << state << " error:" << m_audioOutput->error());

#ifdef MY_OS_MEEGO
    // Quick fix for bug https://bugreports.qt.nokia.com/browse/QTMOBILITY-764
    if (m_audioOutput->error() == QAudio::UnderrunError)
        m_audioOutput->start(this);
#endif

}

PullAudioOut::~PullAudioOut()
{
    m_audioOutput->stop();

    delete m_audioOutput;
    m_audioOutput = 0;

    close();
}

qint64 PullAudioOut::bytesAvailable() const
{
    return (qint64)m_sendBufferSize * sizeof(AUDIO_SAMPLE_TYPE);
}

qint64 PullAudioOut::writeData(const char *data, qint64 len)
{
    Q_UNUSED(data);
    Q_UNUSED(len);
    return 0;
}

qint64 PullAudioOut::readData(char *data, qint64 maxlen)
{

    DEBUG_INFO("pullaudio: %d", maxlen );
    int sampleCount = maxlen / 2;  // How many samples we mix maximumly.
    //sampleCount /= 4;              // The partialbuffer upgrade part.

    if (sampleCount > m_sendBufferSize)
        sampleCount = m_sendBufferSize;

    // QAudioOutput does not use period size on Symbian, and periodSize
    // returns always the bufferSize.
#ifndef Q_OS_SYMBIAN
    // Use multiples of period size
    sampleCount = sampleCount * sizeof(AUDIO_SAMPLE_TYPE) / m_audioOutput->periodSize();
    if (sampleCount)
        sampleCount *= m_audioOutput->periodSize();
    else
        sampleCount = qMin(maxlen, (qint64)m_audioOutput->periodSize());
    sampleCount /= sizeof(AUDIO_SAMPLE_TYPE);
#endif // !Q_OS_SYMBIAN

    memset(data, 0, sampleCount * sizeof(AUDIO_SAMPLE_TYPE));
    int mixedSamples = m_source->pullAudio((AUDIO_SAMPLE_TYPE*)data,
        sampleCount);

    if (mixedSamples < sampleCount)
        mixedSamples = sampleCount;

    return (qint64)mixedSamples * sizeof(AUDIO_SAMPLE_TYPE);
}

bool PullAudioOut::startAudio()
{
    m_audioOutput->start(this);
}

void PullAudioOut::stopAudio()
{
    m_audioOutput->stop();
}

