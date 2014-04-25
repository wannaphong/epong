/*
 * Copyright (c) 2011-2014 Microsoft Mobile.
 */


#include "GameEngine.h"
#include <stdlib.h>
#include <math.h>
#include "PongApp.h"
#include <memory.h>
#include "PongObject.h"


PongObject::PongObject( PongApp *app, PONG_OBJECT_TYPE t) {
    type = t;

    aiAttacking = false;
    pongApp = app;
    curving = 0.0f;

    gravityAffectTime = -1.0f;
    pos[1] = 0.0f;
    pos[2] = 0.5f;

    color[0] = 1.0f;
    color[1] = 1.0f;
    color[2] = 1.0f;
    color[3] = 1.0f;

    memset( gravityVector, 0, sizeof( float ) *3 );

    // normal
    orientation[0] = 1.0f;
    orientation[1] = 0.0f;
    memset( dir, 0, sizeof( float ) *3 );

    switch (t) {
    case PO_COMPUTERPAD:
    case PO_PLAYERPAD2:
        color[1] =0.5f;
        color[2] =0.5f;
        size[0] = PAD_HOR_R;
        size[1] = PAD_VER_R;
        pos[0] = 0.9f;
        pos[1] = 0.0f;
        break;
    case PO_PLAYERPAD:
        color[0] =0.5f;
        color[1] = 0.5f;
        size[0] = PAD_HOR_R;
        size[1] = PAD_VER_R;
        pos[0] = -0.9;
        pos[1] = -0.0;
        break;
    case PO_BALL:
        size[0] = BALL_R;
        size[1] = BALL_R;
        pos[0] = 0.0f;
        dir[0] =0.3f;
        dir[1] = 0.7f;
        break;

    }
    travelled = 0.0f;
    memcpy( expos, pos, sizeof( float ) *3 );
}


PongObject::~PongObject() {

}


void PongObject::controlWithGravity( const float frameTime, float *gravity ) {
    float m = gravityAffectTime;
    if (m<0.0f) return;

    /*
    pos[0]+=gravity[1]*frameTime*m;
    pos[1]-=gravity[0]*frameTime*m;
    */
    gravityVector[0] = gravity[1];
    gravityVector[1] = -gravity[0];

}


void PongObject::ballHitCheck( const float frameTime, PongObject *ball ) {

    float mat2d[2][2];
    mat2d[0][0] = orientation[0];
    mat2d[0][1] = orientation[1];
    mat2d[1][0] = orientation[1];
    mat2d[1][1] = -orientation[0];



    float transBallPos[2];
    float transBallDir[2];
    float transOwnDir[2];

    /*
    transBallPos[0] = (ball->pos[0]-pos[0])*mat2d[0][0] + (ball->pos[1]-pos[1])*mat2d[1][0];
    transBallPos[1] = (ball->pos[0]-pos[0])*mat2d[0][1] + (ball->pos[1]-pos[1])*mat2d[1][1];
    */
    transBallPos[0] = (ball->expos[0]-expos[0])*mat2d[0][0] + (ball->expos[1]-expos[1])*mat2d[1][0];
    transBallPos[1] = (ball->expos[0]-expos[0])*mat2d[0][1] + (ball->expos[1]-expos[1])*mat2d[1][1];

    // combined movement dir.
    float relative_dir[2];
    relative_dir[0] = (pos[0] - expos[0]) + (ball->expos[0] - ball->pos[0]);
    relative_dir[1] = (pos[1] - expos[1]) + (ball->expos[1] - ball->pos[1]);
    transBallDir[0] = relative_dir[0]*mat2d[0][0] + relative_dir[1]*mat2d[1][0];
    transBallDir[1] = relative_dir[0]*mat2d[0][1] + relative_dir[1]*mat2d[1][1];



    // intersect ray

    bool hit = false;
    if (transBallDir[0] * transBallPos[0] > 0.0f) {
        float travelamount = transBallPos[0] / transBallDir[0];
        if (travelamount>=0.0f && travelamount<=1.0f) {
            // hit!!!
            float y = transBallPos[1] + transBallDir[1] * travelamount;
            if (y>-PAD_VER_R && y<PAD_VER_R) hit = true;
        };
    };




    // balls position is inside pad
    if (transBallPos[0]>-PAD_HOR_R && transBallPos[0]<PAD_HOR_R &&
            transBallPos[1]>-PAD_VER_R && transBallPos[1]<PAD_VER_R ) {
        hit = true;
    }



    if (hit) {
        aiAttacking = false;
        //
        transBallPos[0] = (ball->pos[0]-pos[0])*mat2d[0][0] + (ball->pos[1]-pos[1])*mat2d[1][0];
        transBallPos[1] = (ball->pos[0]-pos[0])*mat2d[0][1] + (ball->pos[1]-pos[1])*mat2d[1][1];
        transBallDir[0] = ball->dir[0]*mat2d[0][0] + ball->dir[1]*mat2d[1][0];
        transBallDir[1] = ball->dir[0]*mat2d[0][1] + ball->dir[1]*mat2d[1][1];
        transOwnDir[0] = dir[0]*mat2d[0][0] + dir[1]*mat2d[1][0];
        transOwnDir[1] = dir[0]*mat2d[0][1] + dir[1]*mat2d[1][1];



        pongApp->getGameEngine()->sendEvent( GF::HAPTIC_FEEDBACK, 0 );  // haptic feedback-event.
        pongApp->playSoundEffect( pongApp->sampleBallPong, 1.0f, pos[0] );

        // NOTE, NOT IN 2 PLAYER MODE
        if (type == PO_PLAYERPAD) pongApp->getGame()->addScore( 3 );

        transBallPos[0] = PAD_HOR_R+0.025f;

        float ballSpeed = sqrtf( transBallDir[0] * transBallDir[0] + transBallDir[1] * transBallDir[1] );

        transBallDir[0] = 0.25f;
        transBallDir[1] = transBallPos[1] / PAD_VER_R;

        float length = sqrtf( transBallDir[0] * transBallDir[0] + transBallDir[1] * transBallDir[1] );

        transBallDir[0] = transBallDir[0]/length * ballSpeed;
        transBallDir[1] = transBallDir[1]/length * ballSpeed;

        // add our own "potential" energy to the ball
        transBallDir[0] += transOwnDir[0];
        transBallDir[1] += transOwnDir[1];

        // set curve for the ball
        ball->curving = -transOwnDir[1]*3.0f;



        ball->pos[0] = transBallPos[0]*mat2d[0][0] + transBallPos[1]*mat2d[0][1] + pos[0];
        ball->pos[1] = transBallPos[0]*mat2d[1][0] + transBallPos[1]*mat2d[1][1] + pos[1];

        // reset expos to prevent simultaneous hits.
        ball->expos[0] = ball->pos[0];
        ball->expos[1] = ball->pos[1];

        // Transform modified direction back into the game-space
        ball->dir[0] = transBallDir[0]*mat2d[0][0] + transBallDir[1]*mat2d[0][1];
        ball->dir[1] = transBallDir[0]*mat2d[1][0] + transBallDir[1]*mat2d[1][1];

        pongApp->getParticleEngine()->emitParticles( 1,
                                                    pongApp->getBallHitParticleType(),
                                                    ball->pos,
                                                    0.0f, 0, 0.0f );


        ParticleEngine *peng = pongApp->getParticleEngine();
        int amount = 3 + ballSpeed*30.0f;
        peng->emitParticles( amount, pongApp->getBallFlyParticleType(), ball->pos, 0.05f, 0, ballSpeed );
    };
}



void PongObject::runAI( const float frameTime, PongObject *ball ) {

    float ytarget = ball->pos[1];

    float xdis = ball->pos[0] - pos[0];
    bool movingTowards = false;
    if (xdis*ball->dir[0] < 0.0f) {         // ball is moving towards
        movingTowards = true;
        ytarget -= xdis * ball->dir[1];
        if (ytarget>1.0f) ytarget = 1.0f;
        if (ytarget<-1.0f) ytarget = -1.0f;
    };


    float ydelta = ytarget - pos[1];
    ydelta *= AI_PADMOVE_SPEED;

    float ymax = AI_PAD_MAXYMOVE*(1+pongApp->getGame()->getCurrentLevel())/8.0f;

    if (ydelta>ymax) ydelta = ymax;
    else if (ydelta<-ymax) ydelta = -ymax;
    pos[1] += ydelta*frameTime;

    float xdelta = ball->pos[0] - pos[0];

    // attack towards ball
    if (!aiAttacking) {
        if (movingTowards && ball->pos[0]>0.7 && fabsf( ydelta) < 0.2) aiAttacking = true;
    };
    //if (movingTowards && ball->pos[0]>0.7 && fabsf( ydelta ) < 0.6f ) {      // attack limit
    if (aiAttacking) {
        pos[0] += xdelta * AI_PADMOVE_SPEED * frameTime * 2.0f;
        if ( fabsf( xdelta) > 0.4f) aiAttacking = false;
    } else {
        pos[0] += (0.9-pos[0]) * frameTime * AI_PADMOVE_SPEED;
    }
}

#define PADPHYSICS_POTENTIAL_ENERGY_FRACTION 15.0f
#define PADPHYSICS_POTENTIAL_ENERGY_INCREASE 8.0f



void PongObject::update( const float frameTime, PongObject *ball,
                        bool buttonDown, float pointerX, float pointerY) {

    float gameLevelYLimit = ( pongApp->getTop() );

    memcpy( expos, pos, sizeof( float  ) * 3 );



    float ftemp;
    float c[3];
    switch (type) {
    case PO_BALL:
    {
        c[0] = dir[0] * frameTime;
        c[1] = dir[1] * frameTime;
        c[2] = dir[2] * frameTime;


        pos[0] += c[0];
        pos[1] += c[1];
        pos[2] += c[2];
        //dir[1] += frameTime;

        if (pos[1] < -gameLevelYLimit) {
            pos[1] = -gameLevelYLimit;
            dir[1] *= -1.0f;
        };

        if (pos[1] > gameLevelYLimit) {
            pos[1] = gameLevelYLimit;
            dir[1] *= -1.0f;
        };

        if (pos[0]<-1.2f) {
            // game over?
            //pos[0] = -1.0f;
            //dir[0] *= -1.0f;
            pongApp->getGame()->gameOver();
        };

        if (pos[0]>1.2f) {
            // game over?
            //pos[0] = 1.0f;
            //dir[0] *= -1.0f;
            pongApp->getGame()->levelCompleted();
        };

        // curve ball
        dir[0] += dir[1]*curving * frameTime;
        dir[1] += -dir[0]*curving * frameTime;

        // Decrease curving
        curving -= curving * frameTime;


        // rotate towards direction
        orientation[0] += (dir[0] - orientation[0]) * 30.0f * frameTime;
        orientation[1] += (dir[1] - orientation[1]) * 30.0f * frameTime;


        float ballSpeed = sqrtf( dir[0]*dir[0] + dir[1]*dir[1] + dir[2]*dir[2] );

        if (ballSpeed>pongApp->getGame()->getBallDefaultSpeed()*1.6f) {
            size[0] = (ballSpeed - pongApp->getGame()->getBallDefaultSpeed())*0.5f;
            size[0] *= size[0]*0.05f;
            size[0] += BALL_R;

            //
        } else size[0] = BALL_R;


        ParticleEngine *peng = pongApp->getParticleEngine();
        // emit line of particles from expos to pos
        c[0] = pos[0] - expos[0];
        c[1] = pos[1] - expos[1];
        c[2] = pos[2] - expos[2];
        ftemp = sqrtf( c[0]*c[0] + c[1]*c[1] + c[2]*c[2] );
        travelled += ftemp*125.0f;
        int amount = travelled;
        if (amount>0) {
            travelled -= (float)amount;
            c[0] /= (float)amount;
            c[1] /= (float)amount;
            c[2] /= (float)amount;
            float ppos[3];
            for (int f=0; f<amount; f++) {
                ppos[0] = expos[0] + c[0] * (float)f;
                ppos[1] = expos[1] + c[1] * (float)f;
                ppos[2] = expos[2] + c[2] * (float)f;
                peng->emitParticles(1, pongApp->getBallFlyParticleType(), ppos, 0.02f, 0, 0.085f );
            };

        }
        break;
    }


    case PO_COMPUTERPAD:
        runAI(frameTime, ball);
        break;



    case PO_PLAYERPAD:
    case PO_PLAYERPAD2:
    {
        if (buttonDown) {
            gravityAffectTime = -1.0f;          // reset time
            if (type==PO_PLAYERPAD2)
                pos[0] = pointerX - 0.15;
                //if (pos[0]<0.2f) pos[0] = 0.2f;
                //if (pos[0]>0.95f) pos[0] =0.95f;
            else {
                pos[0] = pointerX + 0.15f;
                //if (pos[0]>-0.2f) pos[0] = -0.2f;
                //if (pos[0]<-0.95f) pos[0] =-0.95f;
            }

            pos[1] = pointerY;
            //pos[1] += (pointerY - pos[1]) * 30.0f * frameTime;

            // +0.15 is for preventing the pad to be directly under the finger
            //pos[0] += ((pointerX+0.15f) - pos[0]) * 30.0f * frameTime;

            if (pos[1] < -gameLevelYLimit) {
                pos[1] = -gameLevelYLimit;
            };

            if (pos[1] > gameLevelYLimit) {
                pos[1] = gameLevelYLimit;
            };

        };

        // control with gravity if set
        pos[0] += gravityVector[0]*frameTime;
        pos[1] += gravityVector[1]*frameTime;
        gravityVector[0] = 0.0f;
        gravityVector[1] = 0.0f;
        gravityVector[2] = 0.0f;


        if (pos[0]<-1.0f) pos[0] = -1.0f;
        float ylimit = pongApp->getTop()-PAD_VER_R;
        if (pos[1]<-ylimit) pos[1] = -ylimit;
        if (pos[1]>ylimit) pos[1] = ylimit;

        gravityAffectTime+=frameTime*6.0f;
        if (gravityAffectTime>2.0f) gravityAffectTime = 2.0f;
        break;
    }
    }


    // Track the potential energy
    if (type==PO_PLAYERPAD || type == PO_COMPUTERPAD || type==PO_PLAYERPAD2) {
        orientation[0] = (ball->pos[0] - pos[0] );
        orientation[1] = (ball->pos[1] - pos[1] );
        if (pos[0]<0.0f) orientation[0] += 1.0f; else orientation[0]-=1.0f;


        // Decrease current energy
        dir[0] -= dir[0] * frameTime * PADPHYSICS_POTENTIAL_ENERGY_FRACTION;
        dir[1] -= dir[1] * frameTime * PADPHYSICS_POTENTIAL_ENERGY_FRACTION;
        dir[2] -= dir[2] * frameTime * PADPHYSICS_POTENTIAL_ENERGY_FRACTION;
        // Increase it according our current actual movement
        dir[0] += (pos[0]-expos[0]) * PADPHYSICS_POTENTIAL_ENERGY_INCREASE;
        dir[1] += (pos[1]-expos[1]) * PADPHYSICS_POTENTIAL_ENERGY_INCREASE;
        dir[2] += (pos[2]-expos[2]) * PADPHYSICS_POTENTIAL_ENERGY_INCREASE;


    };

    ftemp = sqrtf( orientation[0] * orientation[0] + orientation[1] * orientation[1] );
    if (ftemp!=0.0f) {
        orientation[0] = orientation[0] / ftemp;
        orientation[1] = orientation[1] / ftemp;
    } else {
        orientation[0] = 1.0f;
        orientation[1] = 0.0f;
    }

}


void PongObject::render(float visibility)
{
    SpriteDrawInfo sdi;

    // Use manualtransform with pongobject: Discard angle and scale attributes and use manual 2x2 matrix instead.
    sdi.manualTransform(
                (orientation[0]/(0.00001f+visibility))*size[0]*2.0f,
                (orientation[1]/(0.00001f+visibility))*size[0]*2.0f,
                -orientation[1]*size[1]*2.0f,
                orientation[0]*size[1]*2.0f );


    switch (type) {
    case PO_PLAYERPAD2:
    case PO_COMPUTERPAD:
        sdi.textureHandle = pongApp->getComputerPadTexture(); break;
    case PO_PLAYERPAD: sdi.textureHandle = pongApp->getPlayerPadTexture(); break;
    case PO_BALL: sdi.textureHandle = pongApp->getBallTexture(); break;
    };

    sdi.setColor( color );

        // Draw ghost pad
    if (type!=PO_BALL) {
        sdi.setTargetPos(pos[0]+dir[0]*0.25f, pos[1]+dir[1]*0.25f);
        sdi.a = visibility * 0.4f;
        pongApp->getSpriteBatch()->draw(&sdi);
    }

    sdi.setTargetPos(pos[0], pos[1]);
    sdi.a = visibility;
    pongApp->getSpriteBatch()->draw( &sdi );
}

