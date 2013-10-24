/*
 * Copyright (c) 2011 Nokia Corporation.
 */


#include <stdlib.h>
#include <memory.h>
#include <stdio.h>
#include <math.h>
#include "PongApp.h"
#include "PongGame.h"


PongGame::PongGame(PongApp *app, bool singlepl) {
    singlePlayer = singlepl;
    pongApp = app;
    quitAnimation = -1.0f;  // not enabled when below zero
    runTime = 0.0f;
    currentLevel = 1;
    currentScore = 0;
    displayScore = 0;
    if (singlePlayer) {
        pads[0] = new PongObject( app,PO_PLAYERPAD );     // Human player
        pads[1] = new PongObject( app,PO_COMPUTERPAD );   // AI player
    } else {
        pads[0] = new PongObject( app,PO_PLAYERPAD );     // Human player
        pads[1] = new PongObject( app,PO_PLAYERPAD2 );    // Second human player.
    };
    ball = new PongObject( app, PO_BALL );             // Ball

    m_ballDefaultSpeed = 0.8f;      // unit / sec

    scoreIncreaseCounter = 0;
    gameState = PGS_ABOUTTOSTART;
    stateCounter = 0.0f;
    resetBall();
}


PongGame::~PongGame() {
    if (ball) delete ball;
    if (pads[0]) delete pads[0];
    if (pads[1]) delete pads[1];
}

void PongGame::levelCompleted() {
    if (gameState != PGS_RUNNING) return;
    pongApp->getGameEngine()->stopSensors();
    pongApp->playSoundEffect( pongApp->sampleLevelCompleted, 1.0f );
    stateCounter = 0.0f;

    if (singlePlayer) {
        gameState = PGS_LEVELFINISHED;

    } else {
        // game over, player 1 wins
        gameState = PGS_2PL_PLAYER1WINS;
    };
}

void PongGame::gameOver() {
    if (gameState != PGS_RUNNING) return;
    pongApp->getGameEngine()->stopSensors();
    pongApp->playSoundEffect( pongApp->sampleGameOver, 1.0f );
    stateCounter = 0.0f;
    if (singlePlayer) {
        gameState = PGS_GAMEOVER;

    } else {
        // game over, player 2 wins
        gameState = PGS_2PL_PLAYER2WINS;
    };

}

void PongGame::resetBall() {
    memset( ball->direction(), 0, sizeof( float ) * 3 );
    memset( ball->position(), 0, sizeof( float ) * 3 );
    ball->position()[0] = -0.1f + (rand() & 255)*0.2f / 255.0f;
}


bool PongGame::update( const float frameTime,
                      bool *buttons, float *pointerX, float *pointerY ) {
    runTime += frameTime;

    if (quitAnimation>=0.0f) {
        quitAnimation += frameTime;
        if (quitAnimation>=1.0f) return false;  // die
    };

    scoreIncreaseCounter++;
    if (scoreIncreaseCounter>4) {

        displayScore = currentScore*3/4 + displayScore/4;
        if (displayScore<currentScore) displayScore++;
        scoreIncreaseCounter = 0;
    }



            // Button more right will be the one controlling Pad2.


    int leftButton = -1;
    int rightButton = -1;
    if (buttons[0]) {
        if (pointerX[0]<-0.2f) leftButton = 0;
        if (pointerX[0]>0.2f) rightButton = 0;
    };

    if (buttons[1]) {
        if (pointerX[1]<-0.2f && (pointerX[1]<pointerX[0] || leftButton==-1)) leftButton = 1;
        if (pointerX[1]>0.2f && (pointerX[1]>pointerX[0] || rightButton==-1)) rightButton = 1;
    };

    if (leftButton!=-1) {
        pads[0]->update( frameTime, ball, buttons[leftButton], pointerX[leftButton], pointerY[leftButton] );
    } else
        pads[0]->update( frameTime, ball );

    if (rightButton!=-1)
        pads[1]->update( frameTime, ball, buttons[rightButton], pointerX[rightButton], pointerY[rightButton] );
    else
        pads[1]->update( frameTime, ball );


    ball->update( frameTime, 0 );
    pads[0]->ballHitCheck( frameTime, ball );
    pads[1]->ballHitCheck( frameTime, ball );


    stateCounter += frameTime;

    switch (gameState) {
    case PGS_LEVELFINISHED:
        if (buttons[0] && !exbuttonDown) {
                // Give some scores for clearing the level
            currentScore += (currentLevel)*100;
            m_ballDefaultSpeed+=0.1f;
            currentLevel++;
            gameState = PGS_ABOUTTOSTART;
            stateCounter = 0.0f;
            exbuttonDown = true;
            return true;
        };

        break;

    case PGS_ABOUTTOSTART:
        if (buttons[0] && !exbuttonDown) {
            gameState = PGS_RUNNING;
            pongApp->getGameEngine()->startSensors();
            resetBall();
            stateCounter = 0.0f;
            exbuttonDown = true;
            return true;
        };
        break;

    case PGS_GAMEOVER:
    case PGS_2PL_PLAYER1WINS:
    case PGS_2PL_PLAYER2WINS:
        if (buttons[0] && !exbuttonDown && quitAnimation<0.0f) {
            /*
            gameState = PGS_ABOUTTOSTART;
            resetBall();
            displayScore = 0;
            currentScore = 0;
            currentLevel = 1;
            stateCounter = 0.0f;
            exbuttonDown = true;
            */
            quitAnimation = 0.0f;

            return true;
        };

        break;

    case PGS_RUNNING:
    {




        // if ball's horizontal speed is too slow
        // push ball away from horizontal centre
        if (fabsf(ball->direction()[0]) < 0.2f) {
            float to_x_center = fabsf(ball->position()[0]);
            float d = 1.0f - to_x_center;
            if (d>0.0f) {
                //d*=0.1f;
                if (ball->position()[0] < 0.0f)
                    ball->direction()[0] -= d *frameTime;
                else
                    ball->direction()[0] += d *frameTime;

            };
        }


        // Control the ball's speed
        float *ballDir = ball->direction();
        float ballSpeed = sqrtf( ballDir[0] * ballDir[0] + ballDir[1] * ballDir[1] );
        //if (ballSpeed>m_ballDefaultSpeed) {
            float newSpeed = ballSpeed + (m_ballDefaultSpeed - ballSpeed) * frameTime*2.0f;
            ballDir[0] = ballDir[0] / ballSpeed * newSpeed;
            ballDir[1] = ballDir[1] / ballSpeed * newSpeed;
        //};

        break;
    }

    }

    exbuttonDown = buttons[0];
    return true;
}


void PongGame::render() {
    float vis = runTime;
    if (vis>1.0f) vis = 1.0f;
    if (quitAnimation>=0.0f) vis-=quitAnimation;

    pads[0]->render(vis);
    pads[1]->render(vis);
    ball->render(vis);

    char testr[64];
    FontWriter *writer = pongApp->getFontWriter();

    float a = stateCounter;
    if (a>1.0f) a = 1.0f;

    writer->setColor( 1.0f,
                      1.0f,
                      1.0f,
                     vis);
    if (singlePlayer) {
        sprintf(testr, "level %.3d   score %.5d", currentLevel, displayScore );
    } else {
          sprintf(testr, "2player fight" );
    };

    writer->writeText(testr, -0.85f, pongApp->getTop()-0.05f, 0.04f );


    switch (gameState) {
    case PGS_RUNNING:
        break;

    case PGS_2PL_PLAYER1WINS:
        strcpy(testr, "player 1 wins" );
        writer->setColor( 1.0f, 1.0f, 1.0f, a );
        a/=12.0f;
        writer->writeText( testr, -writer->getTextWidth(testr,a)/2.0f, 0.0f, a );
        break;

    case PGS_2PL_PLAYER2WINS:
        strcpy(testr, "player 2 wins" );
        writer->setColor( 1.0f, 1.0f, 1.0f, a );
        a/=12.0f;
        writer->writeText( testr, -writer->getTextWidth(testr,a)/2.0f, 0.0f, a );
        break;

    case PGS_ABOUTTOSTART:
       if ( singlePlayer )
            sprintf(testr, "level %d start", currentLevel );
       else
           strcpy(testr, "fight!");
       writer->setColor( 1.0f, 1.0f, 1.0f, a );
       a/=12.0f;
       writer->writeText( testr, -writer->getTextWidth(testr,a)/2.0f, 0.0f, a );
       break;


    case PGS_LEVELFINISHED:
       sprintf(testr, "level complete" );
       writer->setColor( 1.0f, 1.0f, 1.0f, a );
       a/=12.0f;
       writer->writeText( testr, -writer->getTextWidth(testr,a)/2.0f, 0.0f, a );
       break;

    case PGS_GAMEOVER:
       strcpy(testr, "game over" );
       writer->setColor( 1.0f, 1.0f, 1.0f, a );
       a/=12.0f;
       writer->writeText( testr, -writer->getTextWidth(testr,a)/2.0f, 0.0f, a );
       break;
    }
}







