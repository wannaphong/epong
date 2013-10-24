/*
 * Copyright (c) 2011 Nokia Corporation.
 */



/**
 *
 * GE::GA AudioBuffer functionality
 * tuomo.hirvonen@digia.com
 *
 */

#include <stdio.h>
#include <memory.h>
#include <math.h>
#include "AudioBuffer.h"

using namespace GF;


#pragma pack(1)
    // size = 44?
struct SWavHeader {
    char chunkID[4];
    unsigned int chunkSize;
    char format[4];

    unsigned char subchunk1id[4];
    unsigned int subchunk1size;
    unsigned short audioFormat;
    unsigned short nofChannels;
    unsigned int sampleRate;
    unsigned int byteRate;

    unsigned short blockAlign;
    unsigned short bitsPerSample;

    unsigned char subchunk2id[4];
    unsigned int subchunk2size;
};
#pragma pack()


AudioBuffer::AudioBuffer() {
    m_data = 0;
    m_dataLength = 0;
    m_sampleFunction = 0;
};


AudioBuffer::~AudioBuffer() {
    reallocate(0);
}

void AudioBuffer::reallocate( int length ) {
    if (m_data) delete [] ((char*)m_data);
    m_dataLength = length;
    if (m_dataLength>0) {
        m_data = new char[ m_dataLength ];
    } else m_data = 0;
};


int readWrapper( void *target, const int length, const void *sbuf, const int sbuflen, int &pos )
{
    memcpy( target, ((const char*)sbuf)+pos, length );
    pos+=length;
    return length;
}

AudioBuffer *AudioBuffer::loadWav( const void *d, unsigned int dlength )
{
    // read the header.
    SWavHeader header;
    int p=0;
    readWrapper( &header.chunkID, 44, d,dlength, p);


    // Sanity check
    if (header.chunkID[0]!='R' || header.chunkID[1]!='I' || header.chunkID[2]!='F' || header.chunkID[3]!='F') return 0;	//  incorrect header
    if (header.format[0]!='W' || header.format[1]!='A' || header.format[2]!='V' || header.format[3]!='E') return 0;	//  incorrect header
    if (header.subchunk1id[0]!='f' || header.subchunk1id[1]!='m' || header.subchunk1id[2]!='t' || header.subchunk1id[3]!=' ') return 0;	//  incorrect header
    if (header.subchunk2id[0]!='d' || header.subchunk2id[1]!='a' || header.subchunk2id[2]!='t' || header.subchunk2id[3]!='a') return 0;	//  incorrect header



    // the data follows.
    if (header.subchunk2size<1) return 0;


    AudioBuffer *rval = new AudioBuffer;
    rval->m_nofChannels = header.nofChannels;
    rval->m_bitsPerSample = header.bitsPerSample;
    rval->m_samplesPerSec = header.sampleRate;
    rval->m_signedData = 0;		// where to know this?
    rval->reallocate( header.subchunk2size );

    //fread( rval->m_data, 1, header.subchunk2size, wavFile );
    readWrapper( rval->m_data, header.subchunk2size, d,dlength, p);


    // choose a good sampling function.
    rval->m_sampleFunction = 0;
    if (rval->m_nofChannels==1) {
        if (rval->m_bitsPerSample == 8) rval->m_sampleFunction = sampleFunction8bitMono;
        if (rval->m_bitsPerSample == 16) rval->m_sampleFunction = sampleFunction16bitMono;
    } else {
        if (rval->m_bitsPerSample == 8) rval->m_sampleFunction = sampleFunction8bitStereo;
        if (rval->m_bitsPerSample == 16) rval->m_sampleFunction = sampleFunction16bitStereo;
    }

    return rval;
}

AudioBuffer* AudioBuffer::loadWav( const char *fileName) {
    FILE *f = fopen( fileName, "rb");
    if (f==0) return 0;
    AudioBuffer *rval = loadWav(f);
    fclose(f);
    return rval;
};

AudioBuffer* AudioBuffer::loadWav( FILE *wavFile ) {
    fseek(wavFile, 0L, SEEK_END);
    int fileLength = ftell(wavFile);
    fseek(wavFile, 0L, SEEK_SET);

    char *fileData = new char[ fileLength ];
    fread( fileData, fileLength, 1, wavFile );
    AudioBuffer *rval = loadWav( fileData, fileLength );
    delete [] fileData;
    return rval;


/*
      // DIRECT FILE READING.. COMMENTED FOR NOW.
    // read the header.
    SWavHeader header;
    fread( header.chunkID, 4, 1, wavFile );
    if (header.chunkID[0]!='R' || header.chunkID[1]!='I' || header.chunkID[2]!='F' || header.chunkID[3]!='F') return 0;	//  incorrect header

    fread( &header.chunkSize, 4, 1, wavFile );
    fread( header.format, 4, 1, wavFile );
    if (header.format[0]!='W' || header.format[1]!='A' || header.format[2]!='V' || header.format[3]!='E') return 0;	//  incorrect header

    fread( header.subchunk1id, 4, 1, wavFile );
    if (header.subchunk1id[0]!='f' || header.subchunk1id[1]!='m' || header.subchunk1id[2]!='t' || header.subchunk1id[3]!=' ') return 0;	//  incorrect header

    fread( &header.subchunk1size, 4, 1, wavFile );
    fread( &header.audioFormat, 2, 1, wavFile );
    fread( &header.nofChannels, 2, 1, wavFile );
    fread( &header.sampleRate, 4, 1, wavFile );
    fread( &header.byteRate, 4, 1, wavFile );

    fread( &header.blockAlign, 2, 1, wavFile );
    fread( &header.bitsPerSample, 2, 1, wavFile );

    fread( header.subchunk2id, 4, 1, wavFile );
    if (header.subchunk2id[0]!='d' || header.subchunk2id[1]!='a' || header.subchunk2id[2]!='t' || header.subchunk2id[3]!='a') return 0;	//  incorrect header
    fread( &header.subchunk2size, 4, 1, wavFile );


    // the data follows.
    if (header.subchunk2size<1) return 0;

    AudioBuffer *rval = new AudioBuffer;
    rval->m_nofChannels = header.nofChannels;
    rval->m_bitsPerSample = header.bitsPerSample;
    rval->m_samplesPerSec = header.sampleRate;
    rval->m_signedData = 0;		// where to know this?
    rval->reallocate( header.subchunk2size );

    fread( rval->m_data, 1, header.subchunk2size, wavFile );



    // choose a good sampling function.
    rval->m_sampleFunction = 0;
    if (rval->m_nofChannels==1) {
        if (rval->m_bitsPerSample == 8) rval->m_sampleFunction = sampleFunction8bitMono;
        if (rval->m_bitsPerSample == 16) rval->m_sampleFunction = sampleFunction16bitMono;
    } else {
        if (rval->m_bitsPerSample == 8) rval->m_sampleFunction = sampleFunction8bitStereo;
        if (rval->m_bitsPerSample == 16) rval->m_sampleFunction = sampleFunction16bitStereo;
    }

    return rval;
    */

};



AUDIO_SAMPLE_TYPE AudioBuffer::sampleFunction8bitMono( AudioBuffer *abuffer, int pos, int channel ) {
    return (AUDIO_SAMPLE_TYPE)(((unsigned char*)(abuffer->m_data))[pos]-128)<<8;
};

AUDIO_SAMPLE_TYPE AudioBuffer::sampleFunction16bitMono( AudioBuffer *abuffer, int pos, int channel ) {
    return (AUDIO_SAMPLE_TYPE)(((short*)(abuffer->m_data))[pos]);
};

AUDIO_SAMPLE_TYPE AudioBuffer::sampleFunction8bitStereo( AudioBuffer *abuffer, int pos, int channel ) {
    return ((AUDIO_SAMPLE_TYPE)(((char*)(abuffer->m_data))[pos*abuffer->m_nofChannels + channel])<<8);
};


AUDIO_SAMPLE_TYPE AudioBuffer::sampleFunction16bitStereo( AudioBuffer *abuffer, int pos, int channel ) {
    return (AUDIO_SAMPLE_TYPE)(((short*)(abuffer->m_data))[pos*abuffer->m_nofChannels + channel]);
};

AudioBufferPlayInstance *AudioBuffer::playWithMixer( AudioMixer &mixer ) {
    AudioBufferPlayInstance *i = (AudioBufferPlayInstance*)mixer.addAudioSource( new AudioBufferPlayInstance( this ));
    return i;
};


AudioBufferPlayInstance::AudioBufferPlayInstance() {
    m_fixedPos = 0;
    m_fixedInc = 0;
    m_buffer = 0;
    m_fixedLeftVolume = 4096;
    m_fixedRightVolume = 4096;
    m_destroyWhenFinished = true;
    m_finished = false;
};

AudioBufferPlayInstance::AudioBufferPlayInstance( AudioBuffer *startPlaying ) {
    m_fixedPos = 0;
    m_fixedInc = 0;
    m_fixedLeftVolume = 4096;
    m_fixedRightVolume = 4096;
    m_destroyWhenFinished = true;
    m_finished = false;
    playBuffer( startPlaying, 1.0f, 1.0f );
};

void AudioBufferPlayInstance::playBuffer( AudioBuffer *startPlaying, float volume, float speed, int loopTimes ) {
    m_buffer = startPlaying;
    m_fixedLeftVolume = (int)(4096.0f*volume);
    m_fixedRightVolume = m_fixedLeftVolume;
    m_fixedPos = 0;
    //m_fixedInc = ( startPlaying->getSamplesPerSec() * (int)(4096.0f*speed)) / AUDIO_FREQUENCY;
    setSpeed( speed );
    m_loopTimes = loopTimes;

};

AudioBufferPlayInstance::~AudioBufferPlayInstance() {

};


void AudioBufferPlayInstance::stop() {
    m_buffer = 0;
    m_finished = true;
};

void AudioBufferPlayInstance::setSpeed( float speed ) {
    if (!m_buffer) return;
    m_fixedInc = (int)( ((float)m_buffer->getSamplesPerSec() * 4096.0f*speed) / (float)AUDIO_FREQUENCY );
};

void AudioBufferPlayInstance::setLeftVolume( float vol ) {
    m_fixedLeftVolume = (int)(4096.0f*vol);
};

void AudioBufferPlayInstance::setRightVolume( float vol ) {
    m_fixedRightVolume = (int)(4096.0f*vol);
};

bool AudioBufferPlayInstance::canBeDestroyed() {
    if (m_finished==true &&
        m_destroyWhenFinished==true) return true; else return false;
};



// doesnt do any bound-checking Must be checked before called.
int AudioBufferPlayInstance::mixBlock( AudioBuffer *buffer, AUDIO_SAMPLE_TYPE *target, int samplesToMix ) {
    //qDebug() << "AudioBufferPlayInstance::mixBlock" << samplesToMix;
    SAMPLE_FUNCTION_TYPE sampleFunction = buffer->getSampleFunction();
    if (!sampleFunction) return 0; // unsupported sampletype
    AUDIO_SAMPLE_TYPE *t_target = target+samplesToMix*AUDIO_CHANNELS;
    int sourcepos;
    //int tempCounter = 0;

    if (buffer->getNofChannels() == 2) {         // stereo
        while (target!=t_target) {
            sourcepos = m_fixedPos>>12;
            //target[0] = (((((sampleFunction)( buffer, sourcepos, 0) * (4096-(m_fixedPos&4095)) + (sampleFunction)( m_buffer, sourcepos+1, 0) * (m_fixedPos&4095) ) >> 12) * m_fixedLeftVolume) >> 12);
            //target[1] = (((((sampleFunction)( buffer, sourcepos, 1) * (4096-(m_fixedPos&4095)) + (sampleFunction)( m_buffer, sourcepos+1, 1) * (m_fixedPos&4095) ) >> 12) * m_fixedRightVolume) >> 12);
                // no resampling here
            target[0] = (((sampleFunction)( buffer, sourcepos, 0) * m_fixedLeftVolume) >> 12);
            target[1] = (((sampleFunction)( buffer, sourcepos, 1) * m_fixedRightVolume) >> 12);

            m_fixedPos+=m_fixedInc;
            target+=2;
            //tempCounter++;
        };
    } else {                                      // mono
        int temp;
        while (target!=t_target) {
            sourcepos = m_fixedPos>>12;
            //temp = (((sampleFunction)( m_buffer, sourcepos, 0 ) * (4096-(m_fixedPos&4095)) + (sampleFunction)( m_buffer, sourcepos+1, 0 ) * (m_fixedPos&4095) ) >> 12);
            temp = (sampleFunction)( buffer, sourcepos, 0);
            target[0] = ((temp*m_fixedLeftVolume)>>12);
            target[1] = ((temp*m_fixedRightVolume)>>12);
            m_fixedPos+=m_fixedInc;
            target+=2;
            //tempCounter++;
        };

    };

    return samplesToMix;
};



int AudioBufferPlayInstance::pullAudio( AUDIO_SAMPLE_TYPE *target, int sampleCount ) {
    //qDebug() << "AudioBufferPlayInstance::pullAudio" << sampleCount;
    if (!m_buffer) return 0;			// no sample associated to mix..

    AudioBuffer *buffer = m_buffer;     // use this for now

    int channelLength = ((buffer->getDataLength()) / (buffer->getNofChannels()*buffer->getBytesPerSample()))-2;

    int samplesToWritePerChannel = sampleCount/AUDIO_CHANNELS;
    int amount;
    int totalMixed = 0;


    while (samplesToWritePerChannel>0) {
        int samplesLeft = channelLength - (m_fixedPos>>12);
        int maxMixAmount = (int)(((long long int)(samplesLeft)<<12) / m_fixedInc );         // This is how much we can mix at least
        //int maxMixAmount = (int)((float)samplesLeft / ((float)m_fixedInc/4096.0f));
        //if (maxMixAmount<1) maxMixAmount = 1;           // NOTE, THIS MIGHT CAUSE PROBLEMS. NEEDS CHECKING
        if (maxMixAmount>samplesToWritePerChannel) {
            maxMixAmount=samplesToWritePerChannel;
        }

        if (maxMixAmount>0) {
            amount=mixBlock(buffer, target+totalMixed*2, maxMixAmount );
            if (amount==0)
            {
                break;                       // an error occured
            }
            totalMixed+=amount;
        } else {
            amount = 0;
            m_fixedPos = channelLength<<12;
        }



        // sample is ended,.. check the looping variables and see what to do.
        if ((m_fixedPos>>12)>=channelLength) {
            m_fixedPos -= (channelLength<<12);
            if (m_loopTimes>0) m_loopTimes--;
            if (m_loopTimes==0) {
                stop();
                return totalMixed;
            }
        }

        samplesToWritePerChannel-=amount;
        if (samplesToWritePerChannel<1) break;
    };
    return  totalMixed*2;
};
