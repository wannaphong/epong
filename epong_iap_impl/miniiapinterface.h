/**
 * Copyright (c) 2011 Nokia Corporation and/or its subsidiary(-ies).
 * All rights reserved.
 *
 * For the applicable distribution terms see the license.txt -file, included in
 * the distribution.
 */

#ifndef MINIIAPINTERFACE_H
#define MINIIAPINTERFACE_H

class MiniIAPInterface
{
public:
    MiniIAPInterface();

    /*!
     \fn void MiniIAPClientApi::PurchaseCompleted(const char *productId, const char *status)

     This callback function is called in the following situations:
     - When the %IAPClient gets information from the OPE back end indicating that
     the purchase has been completed.
     - In error situations along with purchaseFlowFinished().

        \param productId The ID of the product.
        \param status The status of the purchase flow. The status values are following:
        - OK: The purchase was successful.
        - Cancel: The user cancelled the transaction.
        - RestorableProduct: The purchase failed but the product is restorable.
        - Other error codes: The purchase failed. Your application should handle these error codes just as a failed purchase.

     */
    virtual void purchaseCompleted(const char *productId, const char *status);

    /*!
     \fn void MiniIAPInterface::PurchaseFlowFinished(const char *productId)

        This callback function is called when IAP shows no more UI after purchase, and the client
        is in control of the whole UI. It is emitted only after purchase flow has ended and
        the last IAP dialog has disappeared from the screen.
        - When the user finishes the purchase flow by cancelling the purchase.
        - When the user closes the Purchase Successful dialog.
        - In error situations.

        \param productId The ID of the product.

     */
    virtual void purchaseFlowFinished(const char *productId);

};

#endif // MINIIAPINTERFACE_H
