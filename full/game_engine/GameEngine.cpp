/*
 * Copyright (c) 2011 Nokia Corporation.
 */


#include "GameEngine.h"
#include <memory.h>
#include <string.h>

using namespace GF;


GameEngine::GameEngine() {

};



GameEngine::~GameEngine() {

};



bool GameEngine::startAudio( int frequenzy, int channels ) {

    return false;
}


void GameEngine::stopAudio() {

}

bool GameEngine::startSensors() {
    return false;
}

void GameEngine::stopSensors() {
}

int GameEngine::loadGLTexture( const char *fileName ) {
    return 0;
}

void GameEngine::releaseTexture(int textureHandle) {
}


const char* GameEngine::adjustPath(const char *inputPath, char *outputPath, int outputPathLength, bool *isOk) {
    if (strlen(inputPath) > outputPathLength) {
        if (isOk) (*isOk) = false;
        strcpy(outputPath, "");
    }
    else {
        if (isOk) (*isOk) = true;
        strcpy(outputPath, inputPath);
    }
    return outputPath;
}

void GameEngine::setUpdateAndPauseState(bool updatesDisabled, bool pauseResumeDisabled) {
}


#ifdef MY_OS_MEEGO
bool GameEngine::isSwipeEnabled() {
    return false;
}
#endif
