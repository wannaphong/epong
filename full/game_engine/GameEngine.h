/*
 * Copyright (c) 2011 Nokia Corporation.
 */

/**
 *
 * Interface for platform dependent game-engine
 *
 *
 */


#ifndef __NOKIAGAME_ENGINE__
#define __NOKIAGAME_ENGINE__

#ifdef IAP
#include "../epong_iap_impl/miniiapclientapi.h"
#endif

namespace GF {

enum ENGINE_EVENT_TYPE {
    HAPTIC_FEEDBACK, // engine should privide haptic click
    VIRTUAL_KEYBOARD, // vkb show (flags=1) or hide(flags=0)
    EXIT // exit
};

class GameEngine {
public:
    GameEngine();
    virtual ~GameEngine();

    /**
     * Exit game engine.  Shuts down everything and returns to OS.
     */
    virtual void exit() {};
    
    /**
     * Start audio engine using given format.  Audio engine starts calling game's pullAudio method.
     */
    virtual bool startAudio( int frequency, int channels );

    /**
     * Stop audio engine.  
     */
    virtual void stopAudio();

    // TODO is this necessary, can't the sensors be always active when engine is enabled,
    // it does not use much power anyway?
    /**
     * Start sensors.
     */
    virtual bool startSensors();

    /**
     * Stop sensors.
     */
    virtual void stopSensors();

    /**
     * Display text-query dialog and return the user's input to requester
     * @param prompt optional prompt
     * @param target buffer to hold user input
     * @param targetLength max length of user input buffer
     * return false if no text was aquired.
     */
    virtual bool queryTextInput( const char *prompt, char *target, int targetLength ) { return false; }

        // Helpers
    // TODO rename to loadTexture
    virtual int loadGLTexture( const char *fileName );
    virtual void releaseTexture(int textureHandle);

    /**
     *
     * Load a file, read it to memory and return it's content and length.
     * The user is responsible for releasing the memory.
     *
     */
    virtual void* loadBinaryResource( const char *fileName, int &sizeTarget ) { return 0; }
    virtual void releaseBinaryResource( void *resource ) {}


    virtual void sendEvent(ENGINE_EVENT_TYPE type, int flags) = 0;

    virtual int openUrlInBrowser(const char *path8) { return 0; };

    // TODO should this be replaced with just setting proper working directory, like on Symbian?
    // InputPaths which start with "configdata:" are supposed to be adjusted to be 
    // in a system-wide configuration place for this application.
    virtual const char *adjustPath(const char *inputPath, char *outputPath, int outputPathLength, bool *isOk = 0);

    // TODO lose this, replace with sendEvent if this is really necessary
    virtual void setUpdateAndPauseState(bool updatesDisabled, bool pauseResumeDisabled);

#ifdef IAP
    // TODO IAP should probably be instantiated by PongApp, not passed from engine
    // gets (and if not existing, creates) "global" MiniIAPClient.
    virtual MiniIAPClientApi *getMiniIAPClient() = 0;
#endif

#ifdef MY_OS_MEEGO
    // TODO lose this, replace with GameApp:notifyEvent
    virtual bool isSwipeEnabled();
#endif
protected:

};




};

#endif
