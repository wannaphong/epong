/*
 * Copyright (c) 2011 Nokia Corporation.
 */


#ifndef __PONGCREDITS__
#define __PONGCREDITS__

class PongApp;


class PongCredits {
public:
    PongCredits( PongApp *app );
    ~PongCredits();

    bool update( const float frameTime );
    void render();


    inline void die() { dead = true; }

protected:
    PongApp *pongApp;
    float lifeTime;
    float fade;
    bool dead;
};


#endif
