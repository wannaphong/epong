/*
 * Copyright (c) 2011-2014 Microsoft Mobile.
 */


#include <stdlib.h>
#include <memory.h>
#include "PongApp.h"
#include "PongBackground.h"

const char *themeTextures[THEME_COUNT][3]={
    { "background_space2.png", "background_space1.png", "planet_big.png"},
//    { "orange_bg_mountain.png", "orange_bg_space.png", "orange_planet_512x512.png"},
//   { "gray_bg_space.png", NULL, "gray_planet1_512x512.png"},
};


PongBackground::PongBackground( int effectIndex, GF::GameEngine *engine, PongApp *app ) {
    visible = true;
    fade = 0.0f;
    pongApp = app;
    m_engine = engine;

    for (int f=0; f<BG_SPRITECOUNT; f++)
        initSprite( &sprites[f], true );
}

void PongBackground::prepare() {
    spriteBatch = pongApp->getSpriteBatch();
    for(int i=0;i<THEME_COUNT;i++)
    {
        themeTextureBackground[i] = m_engine->loadGLTexture(themeTextures[i][0]);
        themeTextureLayer[i] = m_engine->loadGLTexture(themeTextures[i][1]);
        themeTexturePlanet[i] = m_engine->loadGLTexture(themeTextures[i][2]);
#warning planet mipmaps are currently disabled
#if 0
#ifndef USE_GLES11
        glGenerateMipmap(GL_TEXTURE_2D);  // generate mipmaps for planet sprites
#endif
#if defined(USE_GLES11) || defined(USE_GLES20)
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
#endif
#endif
    }
}

PongBackground::~PongBackground() {
    release();
}

void PongBackground::release() {
    for(int i=0;i<THEME_COUNT;i++)
    {
        m_engine->releaseTexture(themeTextureBackground[i]);
        m_engine->releaseTexture(themeTextureLayer[i]);
        m_engine->releaseTexture(themeTexturePlanet[i]);
    }
}

void PongBackground::initSprite( BgSprite *sprite, bool anywhere ) {
    sprite->y = -1.0f + (rand() & 255) / 128.0f;
    if (anywhere==false)
        sprite->x = 1.5f + (rand() & 255 ) / 512.0f;
    else
        sprite->x = -1.0f + (rand() & 255) / 128.0f;
    sprite->speed = ((8+(rand()&255))/2000.0f);

    if ((rand() & 255 ) < 128)
        sprite->size = (0.1f + ((rand()&255)/255.0f))/4.0f;
    else
        sprite->size = (0.2f + ((rand()&255)/255.0f))/2.0f;

    sprite->distance = 6.0f + (rand()&255)/255.0f * 6.0f;
}

bool PongBackground::update( const float frameTime ) {
    if (visible) {
        fade+=frameTime;
        if (fade>1.0f) fade = 1.0f;
    } else {
        fade -= frameTime;
        if (fade<0.0f) return false;
    };

    for (int f=0; f<BG_SPRITECOUNT; f++) {
        sprites[f].x -= sprites[f].speed*frameTime;
        if (sprites[f].x < -1.5f) initSprite( &sprites[f], false );
    };

    int theme = 0;
    if(pongApp->getGame()) theme = pongApp->getGame()->getCurrentLevel()%THEME_COUNT;

    texture1 = themeTextureBackground[theme];
    texture2 = themeTextureLayer[theme];
    sprite1tex = themeTexturePlanet[theme];

    return true;
}


void PongBackground::render(float camx, float camy) {
    SpriteDrawInfo sdi;
    sdi.textureHandle = texture1;
    sdi.setTargetPos( camx/14.0f, camy/14.0f );
    sdi.setScale(3.2f,1.6f);
    spriteBatch->draw( &sdi );


    sdi.textureHandle = sprite1tex;
    for (int f=0; f<BG_SPRITECOUNT; f++) {
        sdi.setScale( 2.0f*sprites[f].size, 2.0f*sprites[f].size );
        sdi.setTargetPos( sprites[f].x + camx/sprites[f].distance,
                          sprites[f].y + camy/sprites[f].distance );

        spriteBatch->draw( &sdi );
    }



    sdi.textureHandle = texture2;
    sdi.setScale(4.0f, 2.0f );
    sdi.setTargetPos(camx/4, camy/4);
    spriteBatch->draw(&sdi);
}
