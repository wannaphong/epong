/*
 * Copyright (c) 2011 Nokia Corporation.
 */



#ifndef __PONGHIGHSCORES__
#define __PONGHIGHSCORES__

#include <stdio.h>

class PongApp;

#define NOF_HIGH_SCORES 7
#define SCORE_DISPLAY_WIDTH 20
#define SCORE_DISPLAY_HEIGHT NOF_HIGH_SCORES
#define SCORE_TAG_CHARACTERS 32

class PongHighScores {
public:
    PongHighScores( PongApp *app, unsigned int new_score_entry = 0 );
    ~PongHighScores();

    void die() { visible = false; }

    int update( const float frameTime );
    void render();


    bool enterCharacter( unsigned char character );

protected:
    void buildScreen();
    float angle;
    bool textInputEnabled;
    int newScorePos;        // if something else than -1, indicates which line is the new score made into the list
    int orderScores(int followPos);      // return the position of the followpos
    char scoreTags[ NOF_HIGH_SCORES ][SCORE_TAG_CHARACTERS];
    unsigned int scores[ NOF_HIGH_SCORES ];

    void createDisplayGrid();

    float fade;
    bool visible;

    char displayGrid[SCORE_DISPLAY_HEIGHT][SCORE_DISPLAY_WIDTH];
    unsigned char displayGridRandom[SCORE_DISPLAY_HEIGHT][SCORE_DISPLAY_WIDTH];
    PongApp *pongApp;
};

#endif
