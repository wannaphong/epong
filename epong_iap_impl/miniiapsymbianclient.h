/**
 * Copyright (c) 2011 Nokia Corporation and/or its subsidiary(-ies).
 * All rights reserved.
 *
 * For the applicable distribution terms see the license.txt -file, included in
 * the distribution.
 */

#ifndef MINIIAPSYMBIANCLIENT_H
#define MINIIAPSYMBIANCLIENT_H

#include <iapsymbianclientapi.h>
#include <iapsymbianclientapiinterface.h>
#include <e32def.h>
//#include <e32cmn.h>
#include "miniiapclientapi.h"
#include "Symbian/drmfile_native_symbian.h"
#include <e32cmn.h>

const TInt KMaxProductIdLength=0x20; // 32
const TInt KMaxStatusLength=0x20; // 32

class SymbianClientApiHelper;

class MiniIAPSymbianClient : public MiniIAPClientApi
{
public:
	MiniIAPSymbianClient();
	~MiniIAPSymbianClient();
    
    // From MiniIAPClientApi 
    virtual void prepare();
    virtual bool purchaseProduct(const char * productId);
    virtual bool isProductPurchased(const char *productId, const char *fileName = 0); 

    void PurchaseCompleted(const char *productId, const char *status);
    void PurchaseFlowFinished(const char *productId);

private:
    SymbianClientApiHelper *m_IAPHelper;
    DRMFileNativeSymbian drmFileHelper;
}; 

#endif // MINIIAPSYMBIANCLIENT_H
