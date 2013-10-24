/*
 * Copyright (c) 2011 Nokia Corporation.
 */


#ifndef __PONGGAME__
#define __PONGGAME__

#include "PongObject.h"

enum PONGGAMESTATE { PGS_RUNNING, PGS_GAMEOVER, PGS_LEVELFINISHED, PGS_ABOUTTOSTART, PGS_2PL_PLAYER1WINS, PGS_2PL_PLAYER2WINS };


class PongGame {
public:
    PongGame(PongApp *app, bool singlepl );
    ~PongGame();

    bool update( const float frameTime,
                bool *buttons, float *pointerX, float *pointerY  );
    void render();
    void addScore( int add ) { currentScore += add; }
    void levelCompleted();
    void gameOver();

    float getBallDefaultSpeed() { return m_ballDefaultSpeed; }

    int getScore() { return currentScore; }
    inline PONGGAMESTATE getGameState() { return gameState; }

    inline int getCurrentLevel() { return currentLevel; }
    inline bool isSinglePlayer() { return singlePlayer; }


protected:
    bool singlePlayer;
    void resetBall();
    char stateText[256];
    PONGGAMESTATE gameState;
    float stateCounter;

    float runTime;
    bool exbuttonDown;
    float quitAnimation;

    PongApp *pongApp;
    PongObject *pads[2];
    PongObject *ball;

    int scoreIncreaseCounter;
    int displayScore;
    int currentScore;
    int currentLevel;
    float m_ballDefaultSpeed;
};

#endif
