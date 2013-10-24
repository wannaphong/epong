/*
 * Copyright (c) 2011 Nokia Corporation.
 */

#include <math.h>
#include <memory.h>
#include <stdlib.h>
#include "PongHighScores.h"
#include "PongApp.h"


PongHighScores::PongHighScores( PongApp *app, unsigned int new_score_entry ) {
    // initialize new entry
    pongApp = app;
    createDisplayGrid();
    fade = 0;
    visible = true;
    angle = 0.0f;
    textInputEnabled = false;

    char fileOut[1024];

    // Load the highscore data
    FILE *file = fopen(pongApp->getGameEngine()->adjustPath("configdata:qtepong.dat", fileOut, 1024), "rb");
    //FILE *file = 0;
    if (file) {
        fread( scoreTags, sizeof(char),SCORE_TAG_CHARACTERS*NOF_HIGH_SCORES, file );
        fread( scores, sizeof (unsigned int), NOF_HIGH_SCORES, file );
        fclose(file);
    } else {
        for (int f=0; f<NOF_HIGH_SCORES; f++) {
            sprintf( scoreTags[f], "beatme%d", f+1 );
            scores[f] =  20+f*f*300;
        };
    };

    orderScores(-1);

    // For debug purposes,.. override highscores
     //new_score_entry = scores[0]+100;

    if (new_score_entry>0 && new_score_entry>scores[0]) {
        scores[0] = new_score_entry;
#ifndef USE_VKB
        app->getGameEngine()->queryTextInput("Enter Your Name", scoreTags[0], SCORE_TAG_CHARACTERS );
#else
        app->getEngine()->sendEvent( GF::VIRTUAL_KEYBOARD, 1);
        strcpy( scoreTags[0], "");
        textInputEnabled = true;
#endif
        // PREPROCESS QUERIED TEXT.
        for (int f=0; f<SCORE_TAG_CHARACTERS; f++) {
            // drop to low-case
            if (scoreTags[0][f]>='A' && scoreTags[0][f]<='Z') scoreTags[0][f] = scoreTags[0][f] - 'A'  + 'a';
        };

        newScorePos = 0;
    } else newScorePos = -1;        // DONT TRACK IT.

    newScorePos = orderScores(newScorePos);

    buildScreen();
}

void PongHighScores::buildScreen()
{
    char testr[64];
    // build the screen.
    for (int f=0; f<NOF_HIGH_SCORES; f++) {
        memset( displayGrid[f], '.', SCORE_DISPLAY_WIDTH );
        char *name = scoreTags[f];
        if (name[0]==0) name = "ERROR";
        memcpy( displayGrid[f], name, strlen(name));
        sprintf( testr, "%d", scores[f]);
        int l = strlen( testr );
        memcpy( displayGrid[f]+SCORE_DISPLAY_WIDTH-l,testr, l);
    }
}

int PongHighScores::orderScores(int followPos) {
    char switchTemp[64];

    unsigned int i;
    for (int f=0; f<NOF_HIGH_SCORES-1; f++)
        for (int g=f+1; g<NOF_HIGH_SCORES; g++) {
            if (scores[f]>scores[g]) {
                if (f == followPos) followPos = g;
                else if (g==followPos) followPos = f;
                i = scores[f];
                memcpy( switchTemp, scoreTags[f], SCORE_TAG_CHARACTERS );
                scores[f] = scores[g];
                memcpy( scoreTags[f], scoreTags[g], SCORE_TAG_CHARACTERS );
                scores[g] = i;
                memcpy( scoreTags[g], switchTemp, SCORE_TAG_CHARACTERS );
            };
        }
    return followPos;
}

PongHighScores::~PongHighScores() {
    char fileOut[1024];
    // save highscores
    FILE *file = fopen(pongApp->getGameEngine()->adjustPath("configdata:qtepong.dat", fileOut, 1024), "wb");
    if (file) {
        fwrite( scoreTags, sizeof(char),SCORE_TAG_CHARACTERS*NOF_HIGH_SCORES, file );
        fwrite( scores, sizeof (unsigned int), NOF_HIGH_SCORES, file );
        fclose(file);
    };
}

bool PongHighScores::enterCharacter( unsigned char character )
{
#ifndef USE_VKB
    return true;
#endif
    if (!textInputEnabled) {
        pongApp->getEngine()->sendEvent( GF::VIRTUAL_KEYBOARD, 0);
        return false;
    }
    if (newScorePos<0) return false;        // no new score requires a name.
    int curlength = strlen( scoreTags[ newScorePos ] );
    if (curlength<SCORE_TAG_CHARACTERS-1) {
        if (character == 13 || character == 10) {
            textInputEnabled = false;
            pongApp->getEngine()->sendEvent( GF::VIRTUAL_KEYBOARD, 0);
            return false;
        }
        else if(character==8 || character==127 && curlength>0) // backspace or delete
        {
            scoreTags[newScorePos][ curlength-1 ] = 0;
            buildScreen();
        }
        else            // Enter new character into the string and rebuild the screen.
        {
            scoreTags[newScorePos][ curlength ] = character;
            scoreTags[newScorePos][ curlength+1 ] = 0;
            buildScreen();
        }
    }
}


void PongHighScores::createDisplayGrid() {
    for (int y=0; y<SCORE_DISPLAY_HEIGHT; y++) {
        for (int x=0; x<SCORE_DISPLAY_WIDTH; x++) {
            displayGrid[y][x] = 'a' + x;
            displayGridRandom[y][x] = (rand()&255);
        }
    }

}


int PongHighScores::update( const float frameTime ) {
    if (visible)
        fade += frameTime;
    else {
        if (textInputEnabled) {
            pongApp->getEngine()->sendEvent( GF::VIRTUAL_KEYBOARD, 0);
            textInputEnabled = false;
        }
        fade-=frameTime;
    }

    angle += frameTime;

    if (fade>2.0f) fade = 2.0f;
    if (!visible && fade<=0.0f) {
        return 0;
    }
    return 1;
}


void PongHighScores::render() {
    FontWriter *w = pongApp->getFontWriter();
    char te[2];
    te[1] =0;


    float lsize = 0.08;

    float v = fade * ( 0.499f*cosf( angle*14.0f ) + 0.5f);


    for (int y=0; y<SCORE_DISPLAY_HEIGHT; y++) {
        if (y==newScorePos) {
            te[0]='#';
            w->setColor( v, v, 1.0f, v );
            w->writeText( te,
                         -((SCORE_DISPLAY_WIDTH+1)*lsize/2.0f),
                         -((SCORE_DISPLAY_HEIGHT-1)*lsize/2.0f) +(float)y*lsize,
                         (lsize)/2.0f );

            w->writeText( te,
                         +((SCORE_DISPLAY_WIDTH+1)*lsize/2.0f),
                         -((SCORE_DISPLAY_HEIGHT-1)*lsize/2.0f) +(float)y*lsize,
                         (lsize)/2.0f );

        };

        for (int x=0; x<SCORE_DISPLAY_WIDTH; x++) {
            te[0] = displayGrid[y][x];
            v = (float)displayGridRandom[y][x] / 255.0f;
            v = (fade-v)*2.0f;
            v-=(rand()&255)/1024.0f;
            if (v>1.0) v = 1.0f;

            if (v<0.0f) v = 0.0f;



            w->setColor( v, v, 1.0f, v );
            w->writeText( te,
                         -((SCORE_DISPLAY_WIDTH-1)*lsize/2.0f) +  (float)x * lsize,
                         -((SCORE_DISPLAY_HEIGHT-1)*lsize/2.0f) +(float)y*lsize,
                         (lsize)/2.0f );
        }
    }


        // Draw string under edit
    if (textInputEnabled) {
        v = fade * ( 0.499f*cosf( angle*14.0f ) + 0.5f);
        w->writeText(scoreTags[newScorePos],-((SCORE_DISPLAY_WIDTH-1)*lsize/2.0f), pongApp->getTop()-lsize/2.0f, lsize/2.0f );
        // cursor
        int l = strlen( scoreTags[ newScorePos ]);
        w->setColor( v, v, 1.0f, v );
        te[0]='#';
        w->writeText( te,
                      -((SCORE_DISPLAY_WIDTH+1)*lsize/2.0f) + lsize*(l+1)*CHAR_EXTEND_MUL/2.0f + lsize/2.0f,
                        pongApp->getTop()-lsize/2.0f,(lsize)/2.0f );

    }
}
