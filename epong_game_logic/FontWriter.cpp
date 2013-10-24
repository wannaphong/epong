/*
 * Copyright (c) 2011 Nokia Corporation.
 */


#include <memory.h>
#include "PongApp.h"
#include "FontWriter.h"


FontWriter::FontWriter( SpriteBatch *batch, unsigned int textID, int xdiv, int ydiv, char *characters ) {
    texture = textID;
    xdivision = xdiv;
    ydivision = ydiv;
    //pongApp = app;
    spriteBatch = batch;

    currentColor[0] = 1.0f;
    currentColor[1] = 1.0f;
    currentColor[2] = 1.0f;
    currentColor[3] = 1.0f;


    for (int f=0; f<128; f++) charMap[f].enabled = false;

    float xscale = 1.0f / (float)xdiv;
    float yscale = 1.0f / (float)ydiv;
    int index =0 ;
    while (*characters) {
        if (*characters>0 && *characters<=128) {
            charMap[ *characters ].enabled = true;
            int ypos = index / xdiv;
            int xpos = index - (ypos*xdiv);
            charMap[ *characters ].offsetScale[0] = xscale * (float)xpos;
            charMap[ *characters ].offsetScale[1] = yscale * (float)ypos;
            charMap[ *characters ].offsetScale[2] = xscale;
            charMap[ *characters ].offsetScale[3] = yscale;

        };
        characters++;
        index++;
    };
}


FontWriter::~FontWriter() {

}

float FontWriter::getTextWidth( const char *text, float scale ) {
    int tlen = strlen( text );
    return (float)(tlen-1) * scale * CHAR_EXTEND_MUL;
}

void FontWriter::setColor( float *c ) {
    memcpy( currentColor, c, sizeof( float ) * 4 );
}

void FontWriter::setColor( float r, float g, float b, float a ) {
    currentColor[0] = r;
    currentColor[1] = g;
    currentColor[2] = b;
    currentColor[3] = a;
}

void FontWriter::writeText( const char *text, float xpos, float ypos, float scale ) {

    SpriteDrawInfo sdi;
    sdi.setColor( currentColor );
    sdi.setScale( 2.0f*scale, 2.0f*scale );
    sdi.textureHandle = texture;

    SCharacter *ch;
    while (*text!=0) {

        if (*text>0 && *text<128) {
            ch = charMap + (*text);

            if (ch->enabled) {
                sdi.setSourceRect( ch->offsetScale );
                sdi.setTargetPos( xpos, ypos );
                spriteBatch->draw(&sdi);
            }
            xpos+=scale*CHAR_EXTEND_MUL;
        }
        text++;
    };
}
