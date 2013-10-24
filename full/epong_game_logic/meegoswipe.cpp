#include "meegoswipe.h"

#include "PongApp.h"
#include "SpriteBatch.h"
#include "PongCredits.h"
#ifdef MY_OS_MEEGO
MeeGoSwipe::MeeGoSwipe( PongApp *app ) {
    visible = false;
    pongApp = app;
}


MeeGoSwipe::~MeeGoSwipe() {

}


bool MeeGoSwipe::update( const float frameTime ) {
    if (!visible) return false;
    return true;
}

#define CREDIT_TEXT_SIZE 0.08f
#define CREDIT_TEXT_YSPACING 0.20f


void MeeGoSwipe::render() {
}

#endif
