/*
 * Copyright (c) 2011 Nokia Corporation.
 */



#ifndef __PONGBACKGROUND__
#define __PONGBACKGROUND__

class PongApp;

#include "GameEngine.h"

#define THEME_COUNT 1
#define BG_SPRITECOUNT 4

class PongBackground {
public:
    PongBackground( int effectIndex, GF::GameEngine *engine, PongApp *app );
    ~PongBackground();

        // return true if still wants to be alive.
    bool update( const float frameTime );
    void render( float camx, float camy );

    void prepare();
    void release();
protected:
    PongApp *pongApp;
    SpriteBatch *spriteBatch;

    int themeTextureBackground[THEME_COUNT];
    int themeTextureLayer[THEME_COUNT];
    int themeTexturePlanet[THEME_COUNT];

    int texture1;
    int texture2;
    int sprite1tex;

    float fade;
    bool visible;


    struct BgSprite {
        float x;
        float y;
        float size;
        float speed;
        float distance;
    };

    void initSprite( BgSprite *sprite, bool anywhere );
    BgSprite sprites[ BG_SPRITECOUNT ];
private:
    GF::GameEngine *m_engine;

};


#endif
