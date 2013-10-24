/*
 * Copyright (c) 2011 Nokia Corporation.
 */


#ifndef __MUSICPLAYER_H__
#define __MUSICPLAYER_H__

#include "AudioInterfaces.h"


#ifdef USE_OGG
#include "stb_vorbis.h"
//#define STB_VORBIS_HEADER_ONLY
//#include "stb_vorbis.c"
#define STB_BUFSIZE (256*1024)
#endif

#ifdef USE_MP3
#include "mpg123.h"
#endif

class MusicPlayer : public GF::IAudioSource
{
public:
    MusicPlayer();
    ~MusicPlayer();

    bool canBeDestroyed() { return false; }
    virtual int pullAudio(short *target, int sampleCount);
    virtual void play(const char *filename);
    virtual void play(const char *mem, int length);

protected:
#ifdef USE_OGG
    stb_vorbis *v;
    // temp buffer for ogg decoding
    char buf[STB_BUFSIZE];
#endif
#ifdef USE_MP3
	mpg123_handle *mh;

	int encoding;

	bool playing;
#endif

	int error;
	long rate;
	int channels;
};

#endif
