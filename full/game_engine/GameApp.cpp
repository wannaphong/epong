/*
 * Copyright (c) 2011 Nokia Corporation.
 */


#include "GameApp.h"


using namespace GF;


GameApp::GameApp( GameEngine *e ) {
    engine = e;
};


GameApp::~GameApp() {

};


bool GameApp::prepare() {

    return true;
};


void GameApp::release() {

};

void GameApp::resize( int w, int h ) {

};

void GameApp::update( const float frameTime ) {

};

void GameApp::render() {

};

void GameApp::readAudioStream(AUDIO_SAMPLE_TYPE *target, int sampleCount ) {
};
