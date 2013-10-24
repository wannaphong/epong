/*
 * Copyright (c) 2011 Nokia Corporation.
 */

/**
 *
 * GF::GA General interfaces
 *
 */

#include <memory.h>
#include "AudioInterfaces.h"
//#include <QDebug>

using namespace GF;

/**
 * AudioSource
 * common functionality
 *
 */
AudioSourceItem::AudioSourceItem(IAudioSource *s) {
    m_currentAudioSource = s;
    m_next = 0;
};

AudioSourceItem::~AudioSourceItem() {
    delete m_currentAudioSource;
};

/**
 * AudioMixer
 *
 */
AudioMixer::AudioMixer() {
    m_sourceList = 0;
    m_mixingBuffer = 0;
    m_mixingBufferLength = 0;
    m_fixedGeneralVolume = 4096;
};


AudioMixer::~AudioMixer() {
    destroyList();
    if (m_mixingBuffer) {
        delete [] m_mixingBuffer;
        m_mixingBuffer = 0;
    };
};

void AudioMixer::destroyList() {

    AudioSourceItem *l = m_sourceList;
    while (l) {
        AudioSourceItem *n = l->m_next;
        delete l;
        l = n;
    };
    m_sourceList = 0;

};


IAudioSource* AudioMixer::addAudioSource( IAudioSource *source ) {
    AudioSourceItem *item = new AudioSourceItem(source);
    item->m_next = 0;
    if (m_sourceList) {
        AudioSourceItem *l = m_sourceList;
        while (l->m_next) l = l->m_next;
        l->m_next = item;
    } else m_sourceList = item;

    return source;

};


bool AudioMixer::removeAudioSource( IAudioSource *source ) {
    // TODO: Implement
    return true;
};

void AudioMixer::setGeneralVolume( float vol ) {
    m_fixedGeneralVolume = (4096.0f*vol);
};

int AudioMixer::pullAudio( AUDIO_SAMPLE_TYPE *target, int sampleCount ) {
    //qDebug() << "AudioMixer::pullAudio" << sampleCount;

    if (!m_sourceList) return 0;

    if (m_mixingBufferLength< sampleCount) {
        if (m_mixingBuffer) delete [] m_mixingBuffer;
        m_mixingBufferLength = sampleCount;
        m_mixingBuffer = new AUDIO_SAMPLE_TYPE[ m_mixingBufferLength ];
    };

    memset( target, 0, sizeof( AUDIO_SAMPLE_TYPE ) * sampleCount );

    AUDIO_SAMPLE_TYPE *t;
    AUDIO_SAMPLE_TYPE *t_target;
    AUDIO_SAMPLE_TYPE *s;

    AudioSourceItem *prev = 0;
    AudioSourceItem *l = m_sourceList;
    while (l) {
        AudioSourceItem *next = l->m_next;

            // process l
        int mixed = l->m_currentAudioSource->pullAudio( m_mixingBuffer, sampleCount );
        if (mixed>0) {
            // mix to main..
            t = target;
            t_target = t+mixed;
            s = m_mixingBuffer;
            while (t!=t_target) {
                // WARNING: Won't work when the sample type is unsigned int
                *t +=(((*s)*m_fixedGeneralVolume)>>12);
                t++;
                s++;
            };
        };


        // autodestroy
        if (l->m_currentAudioSource->canBeDestroyed() == true) {          // NOTE, IS UNDER TESTING,... MIGHT CAUSE UNPREDICTABLE CRASHING WITH SOME USE CASES!!!
            if (!prev)
                m_sourceList = next;
            else prev->m_next = next;
            delete l;
            l = 0;
        }
        else {
            // must only happen when l is NOT destroyed
            prev = l;
        }


        l = next;
    };
    return sampleCount;
};




