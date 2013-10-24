/**
 * Copyright (c) 2011 Nokia Corporation and/or its subsidiary(-ies).
 * All rights reserved.
 *
 * For the applicable distribution terms see the license.txt -file, included in
 * the distribution.
 */

#ifndef SYMBIAN_CLIENT_API_HELPER_H
#define SYMBIAN_CLIENT_API_HELPER_H

#include <e32debug.h>
#include <aknappui.h>
//#include "drmfile.h"
//#include <mw/vwsdef.h> 
#include <e32def.h>
#include <iapsymbianclientapiinterface.h>
#include <iapsymbianclientapi.h>
#include "..\miniiapsymbianclient.h"


class SymbianClientApiHelper : public MIAPSymbianClientApiCallbackInterface 
{
public:
	explicit SymbianClientApiHelper();
	~SymbianClientApiHelper();
    // From MIAPSymbianClientApiCallbackInterface
    virtual void PurchaseCompleted(TInt aRequestId, const TDesC& aStatus,
        const TDesC& aPurchaseTicket);
    virtual void PurchaseFlowFinished(TInt aRequestId);
    virtual void RestorationCompleted(TInt aRequestId, const TDesC& aStatus,
        const TDesC& aPurchaseTicket);
    
    virtual void ProductDataReceived(TInt aRequestId, const TDesC& aStatus,
        MIAPSymbianClientApiCallbackInterface::RProductDataHash aProductData);
    virtual void RestorableProductsReceived(TInt aRequestId, const TDesC& aStatus,
        MIAPSymbianClientApiCallbackInterface::RProductDataList aItems);
    virtual void UserAndDeviceDataReceived(TInt aRequestId, const TDesC& aStatus,
        MIAPSymbianClientApiCallbackInterface::RUserAndDeviceDataHash aData);
    bool purchaseProduct(const char * productId);
    bool isProductPurchased(const char *productId, const char *fileName = 0); 
    void prepare(MiniIAPSymbianClient *aMiniIAPSymbianClient);
private:
    void clearPurchasing();
    CIAPSymbianClientApi *m_IAPClient; // owned
    TInt m_requestId;
    TBool isPurchaseRunning;
    TText8 m_productIdAsText[KMaxProductIdLength];
    MiniIAPSymbianClient *m_miniIAPSymbianClient; // not owned

};

#endif // SYMBIAN_CLIENT_API_HELPER_H
