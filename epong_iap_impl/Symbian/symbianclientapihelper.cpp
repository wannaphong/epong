/**
 * Copyright (c) 2011 Nokia Corporation and/or its subsidiary(-ies).
 * All rights reserved.
 *
 * For the applicable distribution terms see the license.txt -file, included in
 * the distribution.
 */
#include "symbianclientapihelper.h"

#include <e32debug.h>
#include <aknappui.h>
//#include "drmfile.h"
//#include <mw/vwsdef.h> 
#include <e32def.h>

#include "../miniiapsymbianclient.h"

SymbianClientApiHelper::SymbianClientApiHelper() :  
	MIAPSymbianClientApiCallbackInterface(),
	m_IAPClient(NULL),
	m_requestId(-1),
	isPurchaseRunning(EFalse),
	m_miniIAPSymbianClient(NULL)
{
	m_productIdAsText[0] = '\0';
}

SymbianClientApiHelper::~SymbianClientApiHelper()
{
	if (m_IAPClient) delete m_IAPClient;
}

bool SymbianClientApiHelper::purchaseProduct(const char * productId) 
{
	RDebug::Printf("SymbianClientApiHelper::purchaseProduct");
	if (!m_miniIAPSymbianClient) return false;
	if (isPurchaseRunning) return false;
	if (!m_IAPClient) return false;

	isPurchaseRunning = ETrue;
    bool result = false;
    TText8 *productIdAsText = (TText8 *)productId;
    TPtr8 ptrTo(m_productIdAsText, KMaxProductIdLength);
    TPtrC8 ptrFrom(productIdAsText);
    if (ptrFrom.Size() >= KMaxProductIdLength) {
    	RDebug::Printf("SymbianClientApiHelper::purchaseProduct: too long");
    	return false;
    }
    ptrTo.Zero();
    TBuf<KMaxProductIdLength> productIdAsTBuf16;
    RDebug::Printf("pointerSize: %d %s",ptrTo.Size(), productId);
    RDebug::RawPrint(productIdAsTBuf16);
    productIdAsTBuf16.Copy(ptrFrom);
    ptrTo.Copy(ptrFrom);
    ptrTo.Append('\0');
    m_requestId = m_IAPClient->PurchaseProduct(productIdAsTBuf16, CIAPSymbianClientApi::EForcedAutomaticRestoration);
    // @TODO: fix this to "(m_requestId >= 0)" comparison when IAP is working ok!
    if (m_requestId >= 0) {
    	RDebug::Printf("SymbianClientApiHelper::purchaseProduct: started OK, m_requestId == %d",m_requestId);
    	result = true;
    }
    else {
    	RDebug::Printf("SymbianClientApiHelper::purchaseProduct: not started OK, m_requestId == %d",m_requestId);
    	clearPurchasing();
    	result = false;
    }
    return result;
}


void SymbianClientApiHelper::PurchaseCompleted(TInt aRequestId, const TDesC& aStatus,
    const TDesC& aPurchaseTicket) 
{
	RDebug::Printf("SymbianClientApiHelper::PurchaseCompleted, m_miniIAPSymbianClient");
    if (!m_miniIAPSymbianClient) return;
	RDebug::Printf("SymbianClientApiHelper::PurchaseCompleted, %d =? %d ",m_requestId,aRequestId);
    // @TODO: fix this to "(m_requestId != aRequestId)" comparison when IAP is working ok!
    if (m_requestId != aRequestId && m_requestId != 0) return;
    
    TText8 statusAsText[KMaxStatusLength];
    TPtr8 statusPointer(statusAsText, KMaxStatusLength);
    statusPointer.Copy(aStatus);
    statusPointer.Append('\0'); // add \0 to end, is needed?
    (void)aPurchaseTicket;

    RDebug::Printf("SymbianClientApiHelper::PurchaseCompleted, %d", statusPointer.Length());
    RDebug::RawPrint(statusPointer);
    
    m_miniIAPSymbianClient->PurchaseCompleted(m_productIdAsText, statusAsText);
}

void SymbianClientApiHelper::RestorationCompleted(TInt aRequestId, const TDesC& aStatus,
    const TDesC& aPurchaseTicket)
{
	RDebug::Printf("MiniIAPSymbianClient::RestorationCompleted");
    if (!m_miniIAPSymbianClient) return;
    // @TODO: fix this to "(m_requestId != aRequestId)" comparison when IAP is working ok!
    if (m_requestId != aRequestId && m_requestId != 0) return;
    
    // <copied>
    TText8 statusAsText[KMaxStatusLength];
    TPtr8 statusPointer(statusAsText, KMaxStatusLength);
    statusPointer.Copy(aStatus);
    statusPointer.Append('\0'); // add \0 to end, is needed?
    (void)aPurchaseTicket;
    // </copied>
    RDebug::Printf("SymbianClientApiHelper::RestorationCompleted, %d", statusPointer.Length());
    RDebug::RawPrint(statusPointer);
    
    m_miniIAPSymbianClient->PurchaseCompleted(m_productIdAsText, statusAsText);
}


void SymbianClientApiHelper::PurchaseFlowFinished(TInt aRequestId)
{
	RDebug::Printf("SymbianClientApiHelper::PurchaseFlowFinished");
    if (!m_miniIAPSymbianClient) return;
    // @TODO: fix this to "(m_requestId != aRequestId)" comparison when IAP is working ok!
    if (m_requestId != aRequestId && m_requestId != 0) return;    
    
    m_miniIAPSymbianClient->PurchaseFlowFinished(m_productIdAsText);
    clearPurchasing();
}

void SymbianClientApiHelper::prepare(MiniIAPSymbianClient *aMiniIAPSymbianClient)
{
	RDebug::Printf("SymbianClientApiHelper::prepare");
	m_miniIAPSymbianClient = aMiniIAPSymbianClient;
	
    if (!m_IAPClient) {
    	TVwsViewId viewId;
    	((CAknAppUi*)CEikonEnv::Static()->EikAppUi())->GetActiveViewId(viewId);
        m_IAPClient = new CIAPSymbianClientApi(this, viewId);
    }
}

void SymbianClientApiHelper::clearPurchasing()
{
    m_requestId = -1;
    isPurchaseRunning = EFalse;
}

void SymbianClientApiHelper::ProductDataReceived(TInt aRequestId, const TDesC& aStatus,
    MIAPSymbianClientApiCallbackInterface::RProductDataHash aProductData) {
	RDebug::Printf("MiniIAPSymbianClient::PurchaseFlowFinished");

}

void SymbianClientApiHelper::RestorableProductsReceived(TInt aRequestId, const TDesC& aStatus,
    MIAPSymbianClientApiCallbackInterface::RProductDataList aItems) {
	RDebug::Printf("MiniIAPSymbianClient::PurchaseFlowFinished");

}

void SymbianClientApiHelper::UserAndDeviceDataReceived(TInt aRequestId, const TDesC& aStatus,
    MIAPSymbianClientApiCallbackInterface::RUserAndDeviceDataHash aData) {
	RDebug::Printf("MiniIAPSymbianClient::PurchaseFlowFinished");
}

