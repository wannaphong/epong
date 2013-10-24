/*
 * Copyright (c) 2011 Nokia Corporation.
 */

/**
 *
 * Interface for OpenGL game.
 * Does not contain any bindings to any api's
 *
 * Will be runned separately by platform dependent engine.
 *
 */

#ifndef __NOKIAGAMEAPP__
#define __NOKIAGAMEAPP__

#include "GameEngine.h"
#include "AudioInterfaces.h"


namespace GF {

enum MOUSE_EVENT_TYPE { MOUSE_DOWN, MOUSE_UP, MOUSE_MOVE };
enum KEY_EVENT_TYPE { KEY_DOWN, KEY_UP, KEY_CHARACTER };
enum NOTIFY_EVENT_TYPE { NE_ORIENTATION_CHANGED, NE_PAUSED, NE_RESUMED, NE_VOLUME_CHANGED };
enum SENSOR_EVENT_TYPE { ACCELEROMETER,  };

enum BUTTONS { LEFT, RIGHT, UP, DOWN, FIRE, SOFTKEY1, SOFTKEY2 };

class GameApp;

/** implement this function to create your game */
GF::GameApp *createGameApplication( GF::GameEngine *engine );


class GameApp {
public:
    GameApp( GameEngine *engine );
    virtual ~GameApp();

	/** Update game state according to given delta frame time in seconds (e.g. 1/60.0)
	 */
    virtual void update( const float deltaFrameTime );
    
    /** Render screen using OpenGL
     */
    virtual void render();
    
    /** Renders sampleCount amount of audio samples into given buffer.  
     * Calling starts when startAudio() is requested by the application.
     * Here, sampleCount == 2 means 1 sample for 2 channels.
     */
    virtual void readAudioStream(AUDIO_SAMPLE_TYPE * target, int sampleCount );

	/** Screen resize event, called at least once in application life cycle.  In case OS
	 * level application orientation is enabled, this is called for every orientation change. */
    virtual void resize( int w, int h );

    /** Allocate application resources. Invoked by the engine before the update loop starts */
    virtual bool prepare();
    
    /** Release application resources. Invoked by the engine when update loop ends, 
     * either because of application is exiting or application lost its focus (e.g. incoming 
     * phone call) */
    virtual void release();

	/** Handling mouse event */
    virtual void mouseEvent( MOUSE_EVENT_TYPE type, int x, int y, int button ) {};
    
    /** Handling key event */
    virtual void keyEvent( KEY_EVENT_TYPE type, int code ) {};
    
    /** Handling system event */
    virtual void notifyEvent( NOTIFY_EVENT_TYPE type, int flags ) {};

    /** Handling sensor event */
    virtual void sensorEvent( SENSOR_EVENT_TYPE type, int x, int y, int z) {};

    GameEngine *getGameEngine() { return engine; }

protected:
    GameEngine *engine;
};


}

#endif
