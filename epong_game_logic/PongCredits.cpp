/*
 * Copyright (c) 2011-2014 Microsoft Mobile.
 */


#include <math.h>


const char *creditsText[] = {
#ifdef MY_OS_MEEGO
"epong 1.1", // For Ovi Store requirements, version number has to be here.
#else
"epong",
#endif
"",
"produced by",
"forum nokia",
"",
"producer",
"mika pesonen",
"",
"executive",
"producers",
"jarkko aura",
"tommi kenttamies",
"",
"programming",
"jarno heikkinen",
"tuomo hirvonen",
"",
"graphics",
"kari kantola",
"",
"music and sound",
"freesound.org",
"",
"scrum master",
"markus pelkonen",
"",
"feedback",
"kimmo tokkari",
"tomi paananen",
"",
"management",
"tapio laitinen",
"",
"testing",
"antti krats",
"tero paananen",
"",
"image codec",
"music codec",
"sean barrett",
"",
0};


#include "PongApp.h"
#include "PongCredits.h"

PongCredits::PongCredits( PongApp *app ) {
    dead = false;
    fade = 0.0f;
    lifeTime = 0.0f;
    pongApp = app;
}


PongCredits::~PongCredits() {

}


bool PongCredits::update( const float frameTime ) {
    lifeTime += frameTime;

    if (!dead) fade+=frameTime;
    if (fade>1.0f) fade = 1.0f;

    if (dead) fade-=frameTime;

    if (dead && lifeTime>1.0f && fade<0.025f) return false;
    return true;
}

#define CREDIT_TEXT_SIZE 0.08f
#define CREDIT_TEXT_YSPACING 0.20f


void PongCredits::render() {
    FontWriter *fw = pongApp->getFontWriter();

    int textLine = 0;
    while (1) {
        const char *line = creditsText[textLine];
        if (!line) break;

        float lypos = -pongApp->getTop() + lifeTime/6.0f - CREDIT_TEXT_YSPACING * textLine;
        float a = 1.0f - lypos*10.0f*lypos;
        if (a<0.0f) a = 0.0f;
        float xinc = -sinf( lypos*2.0f )*lypos*lypos*4.0f;
        a*=fade;
        fw->setColor(1.0f, 1.0f, 1.0f, a );
        fw->writeText( line, xinc-fw->getTextWidth(line, CREDIT_TEXT_SIZE)/2.0f,lypos , CREDIT_TEXT_SIZE );

        textLine++;
    };
}
