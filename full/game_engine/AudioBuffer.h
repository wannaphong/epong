/*
 * Copyright (c) 2011 Nokia Corporation.
 */


/**
 *
 * GE::GA AudioBuffer functionality
 * tuomo.hirvonen@digia.com
 *
 */

#ifndef __GE_IGA_AUDIOBUFFER__
#define __GE_IGA_AUDIOBUFFER__

#include <stdio.h>
#include "AudioInterfaces.h"


namespace GF {

    class AudioBufferPlayInstance;
    class AudioBuffer;         // forward declaration
    typedef AUDIO_SAMPLE_TYPE(*SAMPLE_FUNCTION_TYPE)(AudioBuffer *abuffer, int pos, int channel);

    class AudioBuffer {			// container for a sound
    public:
        AudioBuffer();
        virtual ~AudioBuffer();

        static AudioBuffer *loadWav( const void *d, unsigned int dlength );
        static AudioBuffer* loadWav( FILE *wavFile );          // support for stdio
        static AudioBuffer* loadWav( const char *fname );
        void reallocate( int length );


        inline void* getRawData() { return m_data; }
        inline int getDataLength() { return m_dataLength; }

        inline int getBytesPerSample() { return (m_bitsPerSample>>3); }
        inline int getBitsPerSample() { return m_bitsPerSample; }
        inline int getSamplesPerSec() { return m_samplesPerSec; }
        inline short getNofChannels() { return m_nofChannels; }
        inline SAMPLE_FUNCTION_TYPE getSampleFunction() { return m_sampleFunction; }


        // static implementations of sample functions
        static AUDIO_SAMPLE_TYPE sampleFunction8bitMono( AudioBuffer *abuffer, int pos, int channel );
        static AUDIO_SAMPLE_TYPE sampleFunction16bitMono( AudioBuffer *abuffer, int pos, int channel );
        static AUDIO_SAMPLE_TYPE sampleFunction8bitStereo( AudioBuffer *abuffer, int pos, int channel );
        static AUDIO_SAMPLE_TYPE sampleFunction16bitStereo( AudioBuffer *abuffer, int pos, int channel );

        AudioBufferPlayInstance *playWithMixer( GF::AudioMixer &mixer );

    protected:
        SAMPLE_FUNCTION_TYPE m_sampleFunction;
        short m_nofChannels;
        void *m_data;
        int m_dataLength;				// in bytes
        short m_bitsPerSample;
        bool m_signedData;
        int m_samplesPerSec;
    };



    class AudioBufferPlayInstance : public IAudioSource {
    public:
        AudioBufferPlayInstance();
        AudioBufferPlayInstance( AudioBuffer *start_playing );
        virtual ~AudioBufferPlayInstance();
        void playBuffer( AudioBuffer *startPlaying, float volume, float fixedSpeed, int loopTimes = 0 );			// looptimes -1 = loop forever

        void setSpeed( float speed );
        void setLeftVolume( float lvol );
        void setRightVolume( float rvol );


        inline void setLoopTimes( int ltimes ) { m_loopTimes = ltimes; }
        void stop();



        int pullAudio( AUDIO_SAMPLE_TYPE *target, int sampleCount );
        bool canBeDestroyed();

        bool isPlaying() { if (m_buffer) return true; else return false; }
        inline bool isFinished() { return m_finished; }
        inline bool destroyWhenFinished() { return m_destroyWhenFinished; }
        inline void setDestroyWhenFinished( bool set ) { m_destroyWhenFinished = set; }

    protected:
        int mixBlock( AudioBuffer *buffer, AUDIO_SAMPLE_TYPE *target, int samplesToMix );
        bool m_finished;
        bool m_destroyWhenFinished;
        int m_fixedPos;
        int m_fixedInc;

        int m_fixedLeftVolume;
        int m_fixedRightVolume;
        int m_fixedCenter;
        int m_loopTimes;
        AudioBuffer *m_buffer;
    };

};



#endif
