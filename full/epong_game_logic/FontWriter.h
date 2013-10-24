/*
 * Copyright (c) 2011 Nokia Corporation.
 */

/**
 * Font writer for fixed-width static fonts.
 *
 *
 */

#ifndef __FONTWRITER__
#define __FONTWRITER__

#include "../spritebatchqt/src_general/SpriteBatch.h"


#define CHAR_EXTEND_MUL 1.5f

class PongApp;

class FontWriter {
public:
    FontWriter( SpriteBatch *batch, unsigned int textureID, int xdiv, int ydiv, char *characters );
    ~FontWriter();

    void setColor( float *color );
    void setColor( float r, float g, float b, float a );
    void writeText( const char *text, float xpos, float ypos, float scale );
    float getTextWidth( const char *text, float scale );


protected:
    SpriteBatch *spriteBatch;

    float currentColor[4];

    struct SCharacter {
        bool enabled;
        float offsetScale[4];
    };

    SCharacter charMap[128];

    unsigned int texture;
    int xdivision;
    int ydivision;


};




#endif
