/*
 * Copyright (c) 2011 Nokia Corporation.
 */

/**
 *
 * GE::GA General interfaces
 * tuomo.hirvonen@digia.com
 *
 */

#ifndef __GE_IGA_INTERFACES__
#define __GE_IGA_INTERFACES__

#include "audiosourceif.h"

namespace GF {

    typedef GE::AudioSource IAudioSource;

    class AudioSourceItem {
    public:
        AudioSourceItem(IAudioSource *s); // s will be owned
        ~AudioSourceItem();
        IAudioSource *m_currentAudioSource; // owned
        AudioSourceItem *m_next;       // for listing, do not touch if you don't know what you are doing.
    };


    class AudioMixer : public IAudioSource {
    public:
        AudioMixer();
        virtual ~AudioMixer();
        void destroyList();						// destroy all the sources in the list


        IAudioSource* addAudioSource( IAudioSource *s );		// add new audio source to the list
        bool removeAudioSource( IAudioSource *s );                      // remove an audio source from the list
        int pullAudio( AUDIO_SAMPLE_TYPE *target, int sampleCount );
        void setGeneralVolume( float vol );


    protected:
        int m_fixedGeneralVolume;
        AUDIO_SAMPLE_TYPE *m_mixingBuffer;
        // Length in AUDIO_SAMPLE_TYPE units (not in bytes)
        int m_mixingBufferLength;
        AudioSourceItem *m_sourceList;
    };

};


#endif
