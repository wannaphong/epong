/*
 * Copyright (c) 2011 Nokia Corporation.
 */


#include <string.h>
#include "GameMenu.h"


GameMenu::GameMenu( GAMEMENU_RENDERFUNCTIONTYPE rfunc, void *dataToRFunc, char *menuTitle ) {
    itemList = 0;
    nextReturnAction = 0;
    destroyAnimation = 0.0f;
    renderFunction = rfunc;
    dataToRenderFunction = dataToRFunc;
    title = menuTitle;
    lifeTime = 0.0f;
    keyControlIndex = -1;
}


GameMenu::~GameMenu() {
    releaseItems();
}

void GameMenu::keyControlUp() {
    int ic = getItemCount();
    if (ic<1) return;
    if (keyControlIndex==-1)
        keyControlIndex = 0;
    else {
        if (keyControlIndex>0) keyControlIndex--;
    };
}


void GameMenu::keyControlDown() {
    int ic = getItemCount();
    if (ic<1) return;
    if (keyControlIndex==-1)
        keyControlIndex = ic-1;
    else {
        if (keyControlIndex<ic-1) keyControlIndex++;
    };
}


void GameMenu::keyControlSelect() {
    int ic = getItemCount();
    if (ic<1) return;

    if (keyControlIndex>=0) {
        click( keyControlIndex );
    } else keyControlIndex = 0;
}


void GameMenu::keyControlEnd() {
    keyControlIndex = -1;
}


MenuItem* GameMenu::addMenuItem( const char *label, const char *action, MENU_ITEM_TYPE type ) {
    MenuItem *ni = new MenuItem;
    ni->subMenu = 0;
    ni->type =type;
    ni->alpha = 1.0f;
    int labelLength = strlen( label );
    ni->selected = false;
    ni->label = new char[ labelLength+1 ];
    strcpy( ni->label, label );

    if (action) {
        int actionLength = strlen( action );
        ni->action = new char[ actionLength + 1 ];
        strcpy( ni->action, action );
    } else ni->action = 0;

    ni->next =0;

    if (itemList) {
        MenuItem *l = itemList;
        while (l->next) l = l->next;
        l->next = ni;
    } else itemList = ni;


    return ni;
};

int GameMenu::getItemCount() {
    int rval = 0;
    MenuItem *l = itemList;
    while (l) { rval++; l = l->next; }
    return rval;
}

void GameMenu::releaseItems() {
    MenuItem *l = itemList;
    while (l) {
        MenuItem *n = l->next;
        if (l->subMenu) delete l->subMenu;
        if (l->label) delete [] l->label;
        if (l->action) delete [] l->action;
        delete l;
        l = n;
    }
    itemList = 0;
}

void GameMenu::render() {

    int ind = 0;
    MenuItem *l = itemList;
    while (l) {
        if (keyControlIndex>-1) l->keysEnabled = true; else l->keysEnabled = false;
        if (ind==keyControlIndex)
            l->keyfocus = true;
        else
            l->keyfocus = false;

        (renderFunction)( dataToRenderFunction, this, l );
        l = l->next;
        ind++;
    }
}

MenuItem* GameMenu::getItemAt( int index ) {
    MenuItem *l = itemList;
    while (l) {
        if (index==0) return l;
        index--;
        l = l->next;
    }
    return 0;
}

bool GameMenu::click( int index ) {
    if (nextReturnAction) return false;
    if (index<0 || index>getItemCount()) return false;
    MenuItem *i = getItemAt(index);
    if (i) {

        i->selected = true;
        nextReturnAction = i->action;
        return true;
    }
    return false;
}

const char *GameMenu::run( float frameTime ) {
    lifeTime+=frameTime;
    int ind = 0;
    MenuItem *l = itemList;
    while (l) {
        l->index = ind;
        ind++;
        l = l->next;
    };



    if (nextReturnAction) {
            destroyAnimation += frameTime;
        if (destroyAnimation>1.0f) {
            char *rval = nextReturnAction;
            nextReturnAction = 0;
            return rval;
          //return nextReturnAction;
        }

    }
    return 0;
}


