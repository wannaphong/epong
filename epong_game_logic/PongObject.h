/*
 * Copyright (c) 2011 Nokia Corporation.
 */

#ifndef __PONGPAD__
#define __PONGPAD__


class PongApp;


#define PAD_HOR_R 0.03f
#define PAD_VER_R 0.2f
#define BALL_R 0.04f

#define AI_PAD_MAXYMOVE 1.0f
#define AI_PADMOVE_SPEED 20.0f


enum PONG_OBJECT_TYPE { PO_BALL, PO_COMPUTERPAD, PO_PLAYERPAD, PO_PLAYERPAD2 };


class PongObject {
public:
    PongObject( PongApp *app, PONG_OBJECT_TYPE t );
    ~PongObject();

    void update( const float frameTime, PongObject *ball,
                bool buttonDown=false, float pointerX=0.0f, float pointerY=0.0f );
    void ballHitCheck( const float frameTime, PongObject *ball );
    void render(float visibility);
    void controlWithGravity( const float frameTime, float *gravity );

    inline float *position() { return pos; };
    inline float *direction() { return dir; }
    inline float *getColor() { return color; }

protected:
    PongApp *pongApp;


    void runAI( const float frameTime, PongObject *ball );

    PONG_OBJECT_TYPE type;

    float gravityAffectTime;
    float color[4];
    float size[2];

    float orientation[2];

    float travelled;
    float expos[3];
    float pos[3];
    float dir[3];
    float gravityVector[3];
    float curving;

    bool aiAttacking;

};


#endif
