/**
 * Copyright (c) 2011 Nokia Corporation.
 *
 * Part of the Qt GameEnabler.
 */

#ifndef GEAUDIOOUT_H
#define GEAUDIOOUT_H

#include <QAudioFormat>
#include "audiosourceif.h"

namespace GE {

enum GESampleTypeEnum { Unknown, SignedInt, UnSignedInt, Float };
enum GEEndianEnum { BigEndian, LittleEndian };


const char GEDefaultAudioCodec[] = "audio/pcm";
const GEEndianEnum GEByteOrder(GE::LittleEndian);
const GESampleTypeEnum GESampleType(GE::SignedInt);

class AudioOut
{
public:
    AudioOut() {}
    virtual ~AudioOut() {}

public:
    virtual bool needsManualTick() const { return false; }
    virtual void tick() {}
    virtual bool startAudio() { return false; }
    virtual void stopAudio() {}
};

} // namespace GE

#endif // GEAUDIOOUT_H
