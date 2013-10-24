/**
 * Copyright (c) 2011 Nokia Corporation.
 *
 * Part of the Qt GameEnabler.
 */

#ifndef GEINTERFACES_H
#define GEINTERFACES_H



namespace GE {

#define AUDIO_FREQUENCY 22050
#define AUDIO_SAMPLE_TYPE short
#define AUDIO_SAMPLE_BITS 16
#define AUDIO_CHANNELS 2

// Constants
const float GEMaxAudioVolumeValue(4096.0f);


class AudioSource
{

public:
    explicit AudioSource();
    virtual ~AudioSource();

public:
    virtual bool canBeDestroyed();
    // sampleCount is the count of short, should be always divisible by AUDIO_CHANNELS
    // For example, when sampleCount is 20,
    //  - 40 bytes (20 shorts) are written
    //  - 10 voice samples are provided for each channel
    virtual int pullAudio(AUDIO_SAMPLE_TYPE *target, int sampleCount ) = 0;
};

} // namespace GE

#endif // GEINTERFACES_H
