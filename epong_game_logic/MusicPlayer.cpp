/*
 * Copyright (c) 2011-2014 Microsoft Mobile.
 */

#include <stdlib.h>
#include <memory.h>
#include <stdio.h>
#include <math.h>
#include <assert.h>

#include "MusicPlayer.h"

/**
 * Simple music player which plays either MP3 (mpg123) or OGG (stb_vorbis)
 */

MusicPlayer::MusicPlayer()
	{
//	playing = false;
	rate = AUDIO_FREQUENCY;
	channels = 2;
	error = 0;

#ifdef USE_MP3
	error = mpg123_init();
	mh = mpg123_new(NULL, &error);
#endif
#ifdef USE_OGG
	v=0;
#endif
	}

MusicPlayer::~MusicPlayer()
	{
#ifdef USE_OGG
	stb_vorbis_close(v);
#endif

#ifdef USE_MP3
	mpg123_close(mh);
	mpg123_delete(mh);
	mpg123_exit();
#endif
	}

void MusicPlayer::play(const char *filename)
	{
#ifdef USE_OGG
	stb_vorbis_close(v);
        stb_vorbis_alloc t;
	t.alloc_buffer=this->buf;
	t.alloc_buffer_length_in_bytes=STB_BUFSIZE;
	// stb_vorbis has char* pointer even though it is not really writing to buffer
	v = stb_vorbis_open_filename((char *)filename, &error, &t);
#endif

#ifdef USE_MP3
	mpg123_close(mh);
	error = mpg123_open(mh, filename);

	if (!error)
		{
		error = mpg123_getformat(mh, &rate, &channels, &encoding);

		rate = AUDIO_FREQUENCY;
		channels = 2;

		mpg123_format_none(mh);
		mpg123_format(mh, rate, channels, encoding);

		}
#endif
	}


void MusicPlayer::play(const char *mem, int length )
        {
#ifdef USE_OGG
        stb_vorbis_close(v);
        stb_vorbis_alloc t;
        t.alloc_buffer=this->buf;
        t.alloc_buffer_length_in_bytes=STB_BUFSIZE;
        // stb_vorbis has char* pointer even though it is not really writing to buffer
        v = stb_vorbis_open_memory( (unsigned char*)mem, length, &error, &t);
#endif

#ifdef USE_MP3
#warning MusicPlayer::play(const char *mem, int length ) not implemented
#endif
        }


int MusicPlayer::pullAudio(AUDIO_SAMPLE_TYPE *target, int sampleCount)
	{
	memset(target, 0, sampleCount * sizeof(AUDIO_SAMPLE_TYPE));
#ifdef USE_OGG
	if(v && error==0)
		{
		int samples = stb_vorbis_get_samples_short_interleaved(v, channels, target, sampleCount);
		if(samples==0)
			{
			stb_vorbis_seek_start(v);
			stb_vorbis_get_samples_short_interleaved(v, channels, target, sampleCount);
			// error=-1;
			}
		}
#endif

#ifdef USE_MP3
	if (!error)
		{
		size_t done;
		error = mpg123_read(mh, (unsigned char *) target, sampleCount
				* sizeof(AUDIO_SAMPLE_TYPE), &done);
		// read will return MPG123_DONE eventually...

		if (error == MPG123_DONE)
			{

			mpg123_seek(mh, 0, SEEK_SET);
			error = mpg123_read(mh, (unsigned char *) target, sampleCount
					* sizeof(AUDIO_SAMPLE_TYPE), &done);
			}

		// ignore format change
		if (error == MPG123_NEW_FORMAT)
			error = MPG123_OK;

		}
#endif
	return sampleCount;
	}

