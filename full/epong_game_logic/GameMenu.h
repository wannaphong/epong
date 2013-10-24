/*
 * Copyright (c) 2011 Nokia Corporation.
 */


#ifndef __GAMEMENU__
#define __GAMEMEMU__


enum MENU_ITEM_TYPE { MI_TRIGGER };

class GameMenu;

struct MenuItem {
    MENU_ITEM_TYPE type;
    float alpha;
    int index;
    bool selected;
    char *label;
    char *action;
    bool keysEnabled;
    bool keyfocus;
    GameMenu *subMenu;
    MenuItem *next;
};


typedef void (*GAMEMENU_RENDERFUNCTIONTYPE)(void *data, GameMenu *menu, MenuItem *i );


class GameMenu {
public:
    GameMenu( GAMEMENU_RENDERFUNCTIONTYPE renderCallBack,
              void *dataToCallbacks,
             char *menuTitle = 0 );
    virtual ~GameMenu();


    const char *run( float frameTime );     // return command string
    void render();
    // returns whether the click was valid
    bool click( int index );

    MenuItem* addMenuItem( const char *label, const char *action, MENU_ITEM_TYPE type = MI_TRIGGER );
    MenuItem* getItemAt( int index );
    void releaseItems();
    int getItemCount();
    float getLifeTime() { return lifeTime; }
    float getDestroyAnimation() { return destroyAnimation; }

    bool alreadyClicked() { if (nextReturnAction) return true; else return false; }


    void keyControlUp();
    void keyControlDown();
    void keyControlSelect();
    void keyControlEnd();


protected:
    int keyControlIndex;
    char *nextReturnAction;
    float destroyAnimation;
    float lifeTime;
    char *title;
    GAMEMENU_RENDERFUNCTIONTYPE renderFunction;
    void *dataToRenderFunction;

    MenuItem *itemList;

};

#endif
