/*
 * Copyright (c) 2011 Nokia Corporation.
 */

#ifndef __PONG_GAMEAPP__
#define __PONG_GAMEAPP__

#include "FontWriter.h"
#include "ParticleEngine.h"
#include "GameApp.h"
#include "PongGame.h"
#include "PongBackground.h"
#include "PongHighScores.h"
#include "PongCredits.h"
#include "GameMenu.h"
#include "AudioInterfaces.h"
#include "AudioBuffer.h"
#include "MusicPlayer.h"
#include "../spritebatchqt/src_general/SpriteBatch.h"

#ifdef IAP
#include "../epong_iap_impl/miniiapinterface.h"
#include "../epong_iap_impl/miniiapclientapi.h"
#include "../epong_iap_impl/epong_iap_constants.h"
#endif

#define MENU_ITEM_HEIGHT 0.15f
#define MAX_PARTICLES 2000          // maximum amount of simultaneous particles alive.
#define AUDIO_EFFECT_CHANNELS 4     // How many audio channels are used.


class PongApp : public GF::GameApp
#ifdef IAP
 , public MiniIAPInterface
#endif
{
public:
    PongApp( GF::GameEngine *engine );
    ~PongApp();
    void update( float frameTime );
    void render();

    bool prepare();
    void release();

    void resize( int w, int h );

    virtual void readAudioStream( AUDIO_SAMPLE_TYPE *target, int sampleCount );

    void mouseEvent( GF::MOUSE_EVENT_TYPE type, int x, int y, int button );
    void keyEvent( GF::KEY_EVENT_TYPE type, int code );
    void sensorEvent(GF::SENSOR_EVENT_TYPE type, int x, int y, int z);


    float getTop() { return (float)currentHeight/(float)currentWidth; }


    ParticleEngine *getParticleEngine() { return pengine; }
    GF::GameEngine *getEngine() { return engine; }

    ParticleType *getBallFlyParticleType() { return ballFlyParticleType; }
    ParticleType *getBallHitParticleType() { return ballHitParticleType; }

    FontWriter *getFontWriter() { return fontWriter; }
    PongGame *getGame() { return currentGame; }

    void notifyEvent( GF::NOTIFY_EVENT_TYPE type, int flags );


    // textures
    int getPlayerPadTexture() { return playerPadTexture; }
    int getComputerPadTexture() { return computerPadTexture; }
    int getBallTexture() { return ballTexture; }

    SpriteBatch *getSpriteBatch() { return spriteBatch; }


        // Audio buffers
    GF::AudioBuffer *sampleBallPong;
    GF::AudioBuffer *sampleMenuTick;
    GF::AudioBuffer *sampleGameOver;
    GF::AudioBuffer *sampleLevelCompleted;
#ifdef EPONG_DEBUG
    GF::AudioBuffer *sampleLatencyTest;
#endif
        // Selects a free channels and starts playing buffer with it
    void playSoundEffect( GF::AudioBuffer *buffer, float volume, float leftright = 0.0f);

#ifdef IAP
    virtual void purchaseCompleted(const char *productId, const char *status);
    virtual void purchaseFlowFinished(const char *productId);
#endif
protected:
    SpriteBatch *spriteBatch;
    float sensorGravity[3];

        // Raw/encoded music loaded into the memory for playback.
    void *musicBinaryResource;
    int musicBinaryResourceLength;

    GameMenu* createMainMenu();
    GameMenu* createPauseMenu();
    GameMenu* createExitQuery();
#ifdef IAP
    GameMenu* createWaitingIAPMenu();
#endif
    static void menuItemRender( void *data, GameMenu *menu, MenuItem *ni );

    bool allowMainMenuCreation;
    int volumeIndicator;    // 0 - mute, 4=

    PongHighScores *highScores;
    PongCredits *credits;
    PongBackground *background1;

        // Font
    FontWriter *fontWriter;
    GameMenu *currentMenu;

        // Particles
    ParticleEngine *pengine;
    ParticleType *ballFlyParticleType;
    ParticleType *ballHitParticleType;


    float pointerX[2];
    float pointerY[2];
    bool buttonDown[2];
    int currentWidth;
    int currentHeight;

    void restartGame(bool singlePlayer);

    PongGame *currentGame;
    int fontTexture;

        // Textures
    int computerPadTexture;
    int playerPadTexture;
    int ballTexture;
    int ballShineTexture;
    int volumeTexture;   //all control buttons,
    int topBarTexture;
#ifdef MY_OS_MEEGO
    int meegoSwipeTexture;
#endif

        // Our projectionmatrix set at resize
    float matrixProjection[16];

        // Audio effect playing
    GF::AudioMixer *amixer;
        // Mixer channels for audio-effects
    GF::AudioBufferPlayInstance **aeffectChannels;
#ifdef IAP
    int m_multiplayerPurchased; // 0 == not started, 1 == phase 1 in started (preparing IAP client), 2 == started purchasing, 3 == purchase OK, 4 == purchase failed.

#endif

    GF::AudioBuffer *loadWavFromResource( const char *name );
};


#endif
