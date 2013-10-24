/*
 * Copyright (c) 2011 Nokia Corporation.
 */

#include <stdlib.h>
#include <math.h>
#include <memory.h>
#include <stdlib.h>
#include "PongApp.h"

#include "SpriteBatch.h"


void PongApp::readAudioStream(AUDIO_SAMPLE_TYPE *target, int sampleCount) {
    amixer->pullAudio(target, sampleCount);
}

GF::GameApp *GF::createGameApplication( GF::GameEngine *engine ) {
    return new PongApp( engine );
}


void PongApp::menuItemRender( void *data, GameMenu *menu, MenuItem *i ) {
    PongApp *app = (PongApp*)data;
    float scale = MENU_ITEM_HEIGHT/2.0f;
    int itemCount = menu->getItemCount();
    float ystart = -0.0 + scale*0.65f*itemCount;
    float ystep = MENU_ITEM_HEIGHT;


    float phase = menu->getLifeTime()*4.0f - (float)i->index;
    if (phase > 1.0f) phase = 1.0f;
    if (phase<0.0f) phase = 0.0f;

    FontWriter *w = app->getFontWriter();

    float a = phase*i->alpha;
    if (a>1.0f) a = 1.0f;
    if (i->selected==false) a -= menu->getDestroyAnimation();
        else a = a*(0.5f + cosf( menu->getDestroyAnimation() * 20.0f )*0.49f );


    if (i->keysEnabled) {
        if (i->keyfocus)
        {
            w->setColor( 1.0f, 1.0f, 1.0f, a );
            scale += 0.02f;
        } else w->setColor( 0.7f, 0.7f, 0.7f, a );
    } else w->setColor( 1.0f, 1.0f, 1.0f, a );

    w->writeText( i->label, -w->getTextWidth(i->label, scale)/2.0f+(1.0f-phase)*2.0f, ystart - ystep * (float)i->index, scale*phase );
}


GF::AudioBuffer *PongApp::loadWavFromResource( const char *name )
{
    //qDebug() << "Loading wav from resource " << QString(name);
    int bin_res_size;
    void *bin_res = engine->loadBinaryResource( name, bin_res_size );
    if (bin_res && bin_res_size>0) {
        GF::AudioBuffer *rval = GF::AudioBuffer::loadWav( bin_res, bin_res_size );
        engine->releaseBinaryResource( bin_res );
        return rval;
    } else {
        return 0;
    }
}


PongApp::PongApp( GF::GameEngine *engine ) : GF::GameApp( engine ) {
    // read settings
    sensorGravity[0] = 0.0f;
    sensorGravity[1] = 0.0f;
    sensorGravity[2] = 0.0f;

    allowMainMenuCreation = true;
    currentMenu = 0;
    fontTexture = 0;
    volumeTexture = 0;
    topBarTexture = 0;
    playerPadTexture = 0;
    computerPadTexture = 0;
    ballTexture = 0;
    currentGame = 0;
    background1 = 0;
    highScores = 0;
    credits = 0;
#ifdef MY_OS_MEEGO
    meegoSwipeTexture=0;
#endif
    buttonDown[0] = false;
    pointerX[0] = 0.0f;
    pointerY[0] = 0.0f;

    buttonDown[1] = false;
    pointerX[1] = 0.0f;
    pointerY[1] = 0.0f;

    fontWriter = 0;
    pengine = 0;

    currentMenu = 0;
    musicBinaryResource = 0;
    musicBinaryResourceLength = 0;

    volumeIndicator = 0;

    amixer = new GF::AudioMixer();
    amixer->setGeneralVolume( 0.4f );
    MusicPlayer *musicPlayer = (MusicPlayer*)amixer->addAudioSource( new MusicPlayer() );

    char fullPath[1024];

#ifdef EPONG_DEBUG
    sampleLatencyTest = loadWavFromResource( "shorttick.wav" );
#else

#ifdef USE_OGG
    //musicPlayer->play(engine->adjustPath("34141__ERH__swell_pad.ogg", fullPath, 1024));
    musicBinaryResource =  engine->loadBinaryResource( "34141__ERH__swell_pad.ogg", musicBinaryResourceLength );
    if (musicBinaryResource) {
        musicPlayer->play( (const char*)musicBinaryResource, musicBinaryResourceLength );
    }
#endif
#ifdef USE_MP3	
    musicPlayer->play(engine->adjustPath("34141__ERH__swell_pad.mp3", fullPath, 1024));
#endif

#endif
    sampleBallPong = loadWavFromResource( "ballpong.wav" );
    sampleMenuTick = loadWavFromResource("menuclick.wav");
    sampleGameOver = loadWavFromResource("gameover.wav");
    sampleLevelCompleted = loadWavFromResource("level_completed.wav");

#ifdef IAP
    engine->getMiniIAPClient()->setMiniIAPReceiver(this);
    m_multiplayerPurchased = -1;
#endif
    aeffectChannels = new GF::AudioBufferPlayInstance*[ AUDIO_EFFECT_CHANNELS ];
    for (int c=0; c<AUDIO_EFFECT_CHANNELS; c++) {
        aeffectChannels[c] = new GF::AudioBufferPlayInstance();
        aeffectChannels[c]->setDestroyWhenFinished( false );
        amixer->addAudioSource( aeffectChannels[c] );
    }
    background1 = new PongBackground(0, engine, this );

    engine->startAudio(AUDIO_FREQUENCY, 2);
}


void PongApp::notifyEvent( GF::NOTIFY_EVENT_TYPE type, int flags ) {
    switch (type) {
    case GF::NE_PAUSED:
        if (currentGame && currentGame->getGameState() == PGS_RUNNING && !currentMenu)
            currentMenu = createPauseMenu();
        break;
    case GF::NE_VOLUME_CHANGED:
        amixer->setGeneralVolume( (float)flags/100.0f*0.7f );
        if (flags<1)
            volumeIndicator = 4;
        else volumeIndicator = (flags-1) * 4 / 100;
        break;
    };
}

GameMenu* PongApp::createMainMenu() {
    // create menu
    GameMenu *m = new GameMenu( menuItemRender, this );
    m->addMenuItem( "start", "start_game" );
#ifdef IAP
    if (m_multiplayerPurchased == -1) {
        // "Delayed initialisation" of m_multiplayerPurchased
        if (engine->getMiniIAPClient()->isProductPurchased(EPongIAP::IAP_ITEM_ID, EPongIAP::IAP_FILE)) {
            m_multiplayerPurchased = 3;
        }
        else {
            m_multiplayerPurchased = 0;
        }
    }
    if (m_multiplayerPurchased == 3) {
        m->addMenuItem( "start 2 player", "start_mp_game" );
    }
    else {
        m->addMenuItem( "buy 2 player game", "buy_mp_game" );
    }
#else
    m->addMenuItem( "start 2 player", "start_mp_game" );
#endif
    m->addMenuItem( "highscores", "show_scores" );
#ifdef TRIAL_VERSION
    m->addMenuItem( "buy now", "buy_now" );
#endif
    m->addMenuItem( "credits", "show_credits" );
    //MenuItem *i = currentMenu->addMenuItem( "settings", "goto_settings" );
    m->addMenuItem( "exit", "exit_query" );
    return m;
};

GameMenu* PongApp::createExitQuery() {
   GameMenu *m = new GameMenu( menuItemRender, this );
   m->addMenuItem( "yes", "exit" );
   m->addMenuItem( "no", "void" );
   return m;
};

GameMenu* PongApp::createPauseMenu() {
   engine->stopSensors();
   GameMenu *m = new GameMenu( menuItemRender, this );
   m->addMenuItem( "resume", "void" );
   m->addMenuItem( "end game", "cancel_game" );
   return m;
};

PongApp::~PongApp() {
    engine->stopAudio();

    if (amixer) delete amixer;

    if (background1) delete background1;
    if (currentMenu) delete currentMenu;
    if (currentGame) delete currentGame;
    if (highScores) delete highScores;
    if (credits) delete credits;

    if (sampleBallPong) delete sampleBallPong;
    if (sampleMenuTick) delete sampleMenuTick;
    if (sampleGameOver) delete sampleGameOver;
    if (sampleLevelCompleted) delete sampleLevelCompleted;

    // Release musicBinaryResource
    if (musicBinaryResource) {
        engine->releaseBinaryResource( musicBinaryResource );
        musicBinaryResource = 0;
        musicBinaryResourceLength = 0;
    }

    if (aeffectChannels) delete [] aeffectChannels;

#ifdef EPONG_DEBUG
    if (sampleLatencyTest) delete sampleLatencyTest;
#endif
}

void PongApp::playSoundEffect( GF::AudioBuffer *buffer, float volume, float leftright ) {
    int c = 0;
    while (c<AUDIO_EFFECT_CHANNELS && aeffectChannels[c]->isPlaying() == true) c++;
    if (c>=AUDIO_EFFECT_CHANNELS) return; // no free channel

    if (leftright<-1.0f) leftright = -1.0f;
    if (leftright>1.0f) leftright = 1.0f;
        // play with channel c

    aeffectChannels[c]->playBuffer( buffer, volume, 1.0f );
    aeffectChannels[c]->setLeftVolume( 0.5f - leftright*0.5f);
    aeffectChannels[c]->setRightVolume( 0.5 + leftright*0.5f);
}


bool PongApp::prepare() {
    fontTexture = engine->loadGLTexture("font.png");
    topBarTexture = engine->loadGLTexture("top_bar.png");
    volumeTexture = engine->loadGLTexture("control_button.png");
    playerPadTexture = engine->loadGLTexture("stick_blue_64x256.png");
    computerPadTexture = engine->loadGLTexture("stick_red_64x256.png");
    ballTexture = engine->loadGLTexture("ball_64x64.png");
    ballShineTexture = engine->loadGLTexture("ball_shine.png");
#ifdef MY_OS_MEEGO
    meegoSwipeTexture = engine->loadGLTexture("meego_swipe.png");
#endif


    pengine = new ParticleEngine( MAX_PARTICLES );

    // Particle types
    ballFlyParticleType = new ParticleType( ballShineTexture );
    ballFlyParticleType->m_lifeTime = 2000;
    ballFlyParticleType->m_lifeTimeRandom = 9000;
    ballFlyParticleType->m_size = 6000;
    ballFlyParticleType->m_sizeRandom = 10000;
    ballFlyParticleType->m_sizeIncRandom = -20000;
    ballFlyParticleType->m_additiveParticle = true;


    ballHitParticleType = new ParticleType( ballShineTexture );
    ballHitParticleType->m_lifeTime = 4000;
    ballHitParticleType->m_lifeTimeRandom = 0;
    ballHitParticleType->m_size = 1600;
    ballHitParticleType->m_sizeInc = 16000*150;
    ballHitParticleType->m_sizeIncInc = -2000;
    ballHitParticleType->m_additiveParticle = true;
    ballHitParticleType->setVisibility(0.0f, 0.3f, 0.6f );

    spriteBatch = SpriteBatch::createInstance(currentWidth, currentHeight);

    fontWriter = new FontWriter( spriteBatch, fontTexture, 8,6, "abcdefghijklmnopqrstuvwxyz0123456789,.!?()/&%#\"=" );

    if (background1) background1->prepare();

    if (currentGame && currentGame->getGameState() == PGS_RUNNING) {
        engine->startSensors();
    }
    return true;
}


void PongApp::release() {

    if (topBarTexture) engine->releaseTexture(topBarTexture);
    if (volumeTexture) engine->releaseTexture(volumeTexture);
    if (fontTexture) engine->releaseTexture(fontTexture);
    if (computerPadTexture) engine->releaseTexture(computerPadTexture);
    if (playerPadTexture) engine->releaseTexture(playerPadTexture);
    if (ballTexture) engine->releaseTexture(ballTexture);
    if (ballShineTexture) engine->releaseTexture(ballShineTexture);
#ifdef MY_OS_MEEGO
    if (meegoSwipeTexture) engine->releaseTexture(meegoSwipeTexture);
    meegoSwipeTexture=0;
#endif

    topBarTexture=0;
    volumeTexture=0;
    computerPadTexture = 0;
    playerPadTexture = 0;
    ballTexture = 0;
    ballShineTexture = 0;
    fontTexture = 0;

    if (pengine) delete pengine;
    if (ballFlyParticleType) delete ballFlyParticleType;
    if (ballHitParticleType) delete ballHitParticleType;
    ballFlyParticleType = 0;
    ballHitParticleType = 0;
    pengine = 0;

    if (fontWriter) delete fontWriter;
    fontWriter = 0;

    if (background1) background1->release();
    engine->stopSensors();

    if (spriteBatch) delete spriteBatch;
    spriteBatch = 0;
}

void PongApp::resize( int w, int h ) {
    float mproj[16] = {
        1.0f, 0, 0, 0,
        0, (float)w / (float)h, 0, 0,
        0, 0, 0, 0,
        0, 0, 0, 1,
    };
    memcpy( matrixProjection, mproj, sizeof( float ) * 16 );
    currentWidth = w;
    currentHeight = h;
    spriteBatch->setTargetSize(w, h);

#if defined(USE_GLES11) || defined(USE_GLES20)
    glViewport(0,0, currentWidth, currentHeight );
#endif
}


void PongApp::update( float realframeTime ) {

    static float frameTime = 1/60.0f;

    frameTime = frameTime*.75 + .25*realframeTime;

    if (!currentMenu && !currentGame && !highScores && !credits && allowMainMenuCreation==true) {
        currentMenu = createMainMenu();
    };

    if (credits) {
        if (credits->update( frameTime ) == 0) {
            delete credits;
            credits = 0;
        };
    };

    if (highScores) {
        if (highScores->update( frameTime ) == 0) {
            delete highScores;
            highScores = 0;
        };
    }

    if (background1) {
        if (background1->update( frameTime ) == false) {
            delete background1;
            background1 = 0;
        };
    };
    if (currentMenu)  {
        const char *command = currentMenu->run( frameTime );
        if (command) {
            GameMenu *newMenu = 0;
                // command has been triggered from the menu
            if (strcmp( command, "exit_query") == 0) {
                //newMenu = createExitQuery();
                engine->exit();
            } else if (strcmp( command, "start_game") == 0) {
                restartGame(true);
#ifdef IAP
            } else if (strncmp( command, "buy_mp_game", 11) == 0) {
                if ((strcmp(command, "buy_mp_game") == 0) && (m_multiplayerPurchased == 0 || m_multiplayerPurchased == 4)) {
                    engine->setUpdateAndPauseState(false, true);
                    engine->getMiniIAPClient()->prepare();
                    engine->setUpdateAndPauseState(false, true);
                    m_multiplayerPurchased = 1;
                    newMenu = createWaitingIAPMenu();
                }
                else if (strcmp(command, "buy_mp_game_waiting") == 0) {
                    if (m_multiplayerPurchased == 1) {
                        m_multiplayerPurchased = 2;
                        engine->setUpdateAndPauseState(true, true);
                        if (engine->getMiniIAPClient()->purchaseProduct(EPongIAP::IAP_ITEM_ID)) {
                            engine->setUpdateAndPauseState(true, true);

                            newMenu = createWaitingIAPMenu();
                        }
                        else {
                            engine->setUpdateAndPauseState(false, false);
                        }
                    }
                    else if (m_multiplayerPurchased == 2) {
                        newMenu = createWaitingIAPMenu();
                    }
                }
                else if (strcmp(command,"buy_mp_game_cancel") == 0) {
                    m_multiplayerPurchased = 4;
                    engine->setUpdateAndPauseState(false, false);
                    // case: "buy_mp_game_ok" -> nothing
                    // case: "buy_mp_game_bypass" -> nothing
                }
                //qDebug() << "buy_mp_game..., command" << command << (newMenu == 0? "no new menu":"new menu!");
#endif
            } else if (strcmp( command, "start_mp_game") == 0) {
                restartGame(false);
            } else if (strcmp( command, "show_scores") == 0) {
                if (highScores) delete highScores;
                highScores = new PongHighScores(this);
            } else if (strcmp( command, "buy_now") == 0)  {
                engine->openUrlInBrowser( "http://store.ovi.com" );
            } else if (strcmp( command, "cancel_game") == 0) {
                if (currentGame) {
                    delete currentGame;
                    currentGame = 0;
                }
            } else if (strcmp( command, "show_credits") == 0) {
                if (credits) delete credits;
                credits = new PongCredits(this);
            };

            delete currentMenu;
            if (!newMenu && currentGame && currentGame->getGameState() == PGS_RUNNING) {
                // return to game
                engine->startSensors();
            }
            currentMenu = newMenu;
        };
    }


    if (currentGame && !currentMenu) {
        if (!currentGame->update( frameTime, buttonDown, pointerX, pointerY)) {
            if (currentGame->isSinglePlayer()) {
                allowMainMenuCreation =false;
                int score = currentGame->getScore();
                delete currentGame;
                currentGame = 0;
                if (highScores) delete highScores;
                highScores = new PongHighScores( this, score );
                allowMainMenuCreation = true;
            } else {
                delete currentGame;
                currentGame = 0;
            }
        }
    }

    if (pengine) {
        pengine->setGravity( sensorGravity );
        pengine->run( frameTime );
    };
}


#define TOP_BAR_HEIGHT 0.06f


void PongApp::render() {
    spriteBatch->begin(SpriteBatch::eALPHA, SpriteBatch::eCUSTOMPROJECTION, matrixProjection );


    static float ftest = 0.0f;
    ftest+=0.01f;
    float camx = cosf( ftest );
    float camy = sinf( ftest );

    if (background1) {
        background1->render(camx, camy);
    }


    if (credits) credits->render();
    if (highScores) highScores->render();
    if (currentGame) currentGame->render();
    if (currentMenu) currentMenu->render();



        // top bar
    SpriteDrawInfo sdi;
    sdi.setScale(2.0f,2.0f*TOP_BAR_HEIGHT);
    sdi.setTargetPos(0, getTop()-TOP_BAR_HEIGHT);
    sdi.setSourceRect(0,0,1,0.97f);
    sdi.textureHandle = topBarTexture;
    spriteBatch->draw(&sdi);

    // volume button and exit button
    sdi.setScale(0.2, 0.1f);
    sdi.textureHandle = volumeTexture;
#ifndef MY_OS_MEEGO
    // volume button specifics
    sdi.setTargetPos( 0.7, getTop() - 0.05f );
    sdi.setSourceRect( (float)volumeIndicator/6.0f, 0.0f, 1.0f/6.0f, 1.0f );
    spriteBatch->draw( &sdi );
#endif


    // Exit button
    if (currentGame) {
        sdi.setTargetPos( 0.9f, getTop() - 0.05f );
        sdi.setSourceRect( (float)5.0f/6.0f, 0.0f, 1.0f/6.0f, 1.0f );
        spriteBatch->draw( &sdi );
    }
#ifdef MY_OS_MEEGO
    if (getGameEngine()->isSwipeEnabled()) {
        sdi.setScale(0.5f, 0.25f);
        sdi.setTargetPos(-0.75f, -0.40f);
        sdi.setSourceRect(0,0,1,1);
        sdi.textureHandle = meegoSwipeTexture;
        spriteBatch->draw(&sdi);
    }
#endif
    spriteBatch->end();

    if (pengine) {
            // Render the two types separetally to maximize the average batchsize.
        spriteBatch->begin(SpriteBatch::eADDITIVE, SpriteBatch::eCUSTOMPROJECTION, matrixProjection );
        pengine->render( spriteBatch, ballFlyParticleType );
        pengine->render( spriteBatch, ballHitParticleType );
        spriteBatch->end();
    }

#ifdef EPONG_DEBUG
    spriteBatch->begin(SpriteBatch::eALPHA, SpriteBatch::eCUSTOMPROJECTION, matrixProjection );
    sdi.setScale(0.2, 0.1f);
    sdi.textureHandle = volumeTexture;
    sdi.setTargetPos( pointerX[0], pointerY[0]);
    sdi.setSourceRect( (float)5.0f/6.0f, 0.0f, 1.0f/6.0f, 1.0f );
    spriteBatch->draw( &sdi );
    spriteBatch->end();
#endif
};


void PongApp::keyEvent( GF::KEY_EVENT_TYPE type, int code ) {
    if (highScores && type==GF::KEY_CHARACTER) {
        if (highScores->enterCharacter( -code )) return;        // will return false when string is completed.
    }
    if (!currentMenu) return;

    if (type!=GF::KEY_DOWN) return;

    GF::BUTTONS but = (GF::BUTTONS)code;

    switch (but) {
        case GF::UP: currentMenu->keyControlUp(); break;
        case GF::DOWN: currentMenu->keyControlDown(); break;
        case GF::FIRE: currentMenu->keyControlSelect(); break;
    }
}


void PongApp::sensorEvent(GF::SENSOR_EVENT_TYPE type, int x, int y, int z) {
    if (type == GF::ACCELEROMETER) {
        if (x==0 && y==0 && z == 0) return;
#ifndef Q_OS_WIN32
        float fx,fy,fz;
        fx = (float)y;
        fy = (float)-x;
        fz = (float)z;
        float invlen = 1.0f/sqrtf( fx*fx +fy*fy +fz*fz );
        fx *= invlen;
        fy *= invlen;
        fz *= invlen;
        sensorGravity[0] = fx;
        sensorGravity[1] = fy;
        sensorGravity[2] = fz;
#endif
    };
}


void PongApp::mouseEvent( GF::MOUSE_EVENT_TYPE type, int x, int y, int button ) {
    if (button>1 || button<0) {
        return;          // ignore.. cannot fit
    }

    float worldx = (float)x / (float)currentWidth*2.0f - 1.0f;
    float worldy = (float)y / (float)currentHeight*2.0f - 1.0f;
    worldy *= -((float)currentHeight/(float)currentWidth);

#ifdef EPONG_DEBUG
    if (type==GF::MOUSE_DOWN && y < 24)
    {
        playSoundEffect( sampleLatencyTest, 1.0f );
    }
#endif

    if (currentMenu) currentMenu->keyControlEnd();


    if (highScores) {
        if (type==GF::MOUSE_DOWN) {
            highScores->die();
        }
        return;
    };

    if (credits) {
        if (type==GF::MOUSE_DOWN) {
            credits->die();
        }
        return;
    };

    if (currentMenu) {
        if (currentMenu->alreadyClicked() == false && type==GF::MOUSE_DOWN) {
            int itemCount = currentMenu->getItemCount();
            // similar as in menuItemRender
            float scale = MENU_ITEM_HEIGHT/2.0f;
            float ystart = scale*0.65f*itemCount + scale;
            float ystep = MENU_ITEM_HEIGHT;



            if (worldy<ystart) {
                int clickIndex = -(worldy-ystart) / ystep;
                if (currentMenu->click( clickIndex ))
                    playSoundEffect( sampleMenuTick, 1.0f );
            }

        };
        return;
    }


    switch (type) {
        case GF::MOUSE_DOWN:
            if (worldy>getTop()-0.1f) {
                if (worldx > 0.7f) {
                    if (worldx > 0.8f) {
                        if (currentGame && currentGame->getGameState()==PGS_RUNNING && !currentMenu) {
                            // create pause menu
                            currentMenu = createPauseMenu();
                        }
                    } else {

                    };

                };

                return;
            };
            buttonDown[button] = true;
        break;
        case GF::MOUSE_UP:
            buttonDown[button] = false;
    };



    pointerX[button] = worldx;
    pointerY[button] = worldy;


}

void PongApp::restartGame(bool singlePlayer ) {
    engine->startSensors();
    if (currentGame) delete currentGame;
    currentGame = new PongGame( this, singlePlayer );

}

#ifdef IAP
void PongApp::purchaseCompleted(const char *productId, const char *status) {
    if (strcmp(status, "OK") == 0) {
        //qDebug() << "PongApp::purchaseCompleted !!!";
        m_multiplayerPurchased = 3;
    }
    else {
        m_multiplayerPurchased = 4;
        // Nothing
    }
    //qDebug() << "PongApp::purchaseCompleted" << productId << status;
}

void PongApp::purchaseFlowFinished(const char *productId) {
    engine->setUpdateAndPauseState(false, false);
    currentMenu->click(0);
    playSoundEffect( sampleMenuTick, 1.0f );

    (void*)productId;
    //qDebug() << "purchaseFlowFinished" << productId;
}

GameMenu * PongApp::createWaitingIAPMenu()
{
    static int my_counter = 0;
    // create menu
    GameMenu *m = new GameMenu( menuItemRender, this );
    if (m_multiplayerPurchased == 2) {
        my_counter ++;
        m->addMenuItem( "waiting iap", "buy_mp_game_waiting" );
        if (my_counter > 2) {
            //qDebug() << "BYPASS";
            m->addMenuItem( "exit iap purchase", "buy_mp_game_bypass" );
            my_counter = 0;
        }
    }
    else if (m_multiplayerPurchased == 1){
        m->addMenuItem( "start purchase", "buy_mp_game_waiting" );
        m->addMenuItem( "cancel", "buy_mp_game_cancel" );
    }
    else {
        m->addMenuItem( "click to finish iap try", "buy_mp_game_bypass" );
    }
    return m;
}

#endif
