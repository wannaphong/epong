#ifndef MEEGOSWIPE_H
#define MEEGOSWIPE_H
#ifdef MY_OS_MEEGO

class PongApp;
class SpriteDrawInfo;

class MeeGoSwipe {
public:
    MeeGoSwipe( PongApp *app );
    ~MeeGoSwipe();

    bool update( const float frameTime );
    void render();


    inline void die() { visible = false; }

protected:
    PongApp *pongApp;
    bool visible;
    SpriteDrawInfo sdi;
};


#endif
#endif // MEEGOSWIPE_H
