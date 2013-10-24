/**
 * Copyright (c) 2011 Nokia Corporation and/or its subsidiary(-ies).
 * All rights reserved.
 *
 * For the applicable distribution terms see the license.txt -file, included in
 * the distribution.
 */

#ifndef MINIIAPCLIENTAPI_H
#define MINIIAPCLIENTAPI_H

#include "miniiapinterface.h"

class MiniIAPClientApi
{

public:
    MiniIAPClientApi();
    virtual ~MiniIAPClientApi();
    
    virtual void prepare();

    /*!
        \fn bool MiniIAPInterface::purchaseProduct(const char *productId)

        Requests that Ovi Store process a payment for an in-app purchase.
        The purchase is automatically transformed to a restoration.
        If an item is DRM-encrypted, the %IAPClient API can restore the item
        only if it was encrypted by Nokia.

        The following methods of receiver are called during execution:
        - purchaseCompleted() &mdash; when the Ovi Payment Experience (OPE) back-end
        has completed the purchase, this signal provides the purchase status and data
        - purchaseFlowFinished() &mdash; when the user has finished purchase flow.

        \param productId The ID of the product to be purchased.

        \return If the request fails immediately, returns false and the MiniIAPInterface receiver's
                methods are not called, otherwise, returns true.
     */
    virtual bool purchaseProduct(const char * productId);

    /*!
        \fn int MiniIAPInterface::isProductPurchased(const char *productId, const char *fileName = 0)

        \param productId The ID of the product.
        \param fileName The filename of a DRM protected file of the product.

        \return If the product is purchased, returns true, otherwise, returns false.
     */
    virtual bool isProductPurchased(const char *productId, const char *fileName = 0);

    MiniIAPInterface *miniIAPReceiver() const;
    void setMiniIAPReceiver(MiniIAPInterface *receiver);

protected:
    MiniIAPInterface *m_miniIAPReceiver; // Not owned
};

#endif // MINIIAPCLIENTAPI_H
