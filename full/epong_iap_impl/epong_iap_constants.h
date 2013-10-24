#ifndef IAP_CONSTANTS_H
#define IAP_CONSTANTS_H

namespace EPongIAP {
    static const char *IAP_ITEM_ID("642613");
    static const char *IAP_FILE("2player.dat");
#ifdef IAP_DEBUG
    static const char *IAP_FAILED_ID("642614");
    static int IAP_REQUEST = 0;
#endif
}

#endif // IAP_CONSTANTS_H
