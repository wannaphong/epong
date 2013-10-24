#include "miniiapsymbianclient.h"
#include <iapsymbianclientapi.h>
#include <e32debug.h>
#include <aknappui.h>
//#include "drmfile.h"
//#include <mw/vwsdef.h> 
#include <e32def.h>
#include <f32file.h>
#include "Symbian/symbianclientapihelper.h"


MiniIAPSymbianClient::MiniIAPSymbianClient() :
    MiniIAPClientApi(),
    drmFileHelper()
{ 
    m_IAPHelper = 0;
	RDebug::Printf("MiniIAPSymbianClient::MiniIAPSymbianClient");
}

MiniIAPSymbianClient::~MiniIAPSymbianClient()
{
	if (m_IAPHelper) delete m_IAPHelper;
	m_IAPHelper = 0;
}

bool MiniIAPSymbianClient::isProductPurchased(const char *productId, const char *fileName)
{
	TBuf<KMaxFileName> processname = 
    		RProcess().FileName();
	processname.Delete(processname.Length()-4,4);
	// from "C:\sys\bin\EPong_0xE003AF4F.exe"
	//  to  "C:\sys\bin\EPong_0xE003AF4F"

	//  ultimately to  "C:\private\E003AF4F\drm\data\resourceid_642613\2player.dat"
	TText8 *productIdAsText = (TText8 *)productId;
	TPtrC8 productIdPtr(productIdAsText);
	TText8 *fileNameAsText = (TText8 *)fileName;
	TPtrC8 fileNamePtr(fileNameAsText);
	TPtrC16 drive = processname.Left(1);
	TPtrC16 uid = processname.Right(8);
	TBuf<KMaxProductIdLength> tempBuf;
	TBuf<KMaxFileName> path;
	path.Append(drive);
	_LIT(KPrivate,":\\private\\");
	path.Append(KPrivate);
	path.Append(uid);
	_LIT(KDRMDir,"\\drm\\data\\resourceid_");
	path.Append(KDRMDir);
	tempBuf.Zero(); tempBuf.Copy(productIdPtr);
	path.Append(tempBuf);
	_LIT(KSeparator,"\\");
	path.Append(KSeparator);
	tempBuf.Zero(); tempBuf.Copy(fileNamePtr);
	path.Append(tempBuf);
	RDebug::Printf("isProductPurchased ??");
	RDebug::RawPrint(path);
	int error = drmFileHelper.open(path);
	bool result = true;
	if (error && drmFileHelper.isDRMError(error)) {
		RDebug::Printf("IS NOT PURCHASED");
		result = false;
	}
	else {
		RDebug::Printf("IS PURCHASED");
	}
	return result;
}

bool MiniIAPSymbianClient::purchaseProduct(const char *productId)
{
    RDebug::Printf("MiniIAPSymbianClient::purchaseProduct started");
    prepare();
    if (!m_miniIAPReceiver) {
        RDebug::Printf("MiniIAPSymbianClient::purchaseProduct, but NO m_miniIAPReceiver");
    }
    return m_IAPHelper->purchaseProduct(productId);
}

void MiniIAPSymbianClient::PurchaseCompleted(const char *productId, const char *status) 
{
	RDebug::Printf("MiniIAPSymbianClient::PurchaseCompleted %d", m_miniIAPReceiver);
    if (m_miniIAPReceiver == NULL) return;
	RDebug::Printf("MiniIAPSymbianClient::PurchaseCompleted");
    m_miniIAPReceiver->purchaseCompleted(productId, status);
}

void MiniIAPSymbianClient::PurchaseFlowFinished(const char *productId)
{
	RDebug::Printf("MiniIAPSymbianClient::PurchaseFlowFinished %d", m_miniIAPReceiver);
    if (m_miniIAPReceiver == NULL) return;
    m_miniIAPReceiver->purchaseFlowFinished(productId);
}

void MiniIAPSymbianClient::prepare()
{
	RDebug::Printf("MiniIAPSymbianClient::prepare");
    if (!m_miniIAPReceiver) {
        RDebug::Printf("MiniIAPSymbianClient::prepare, #6, but NO m_miniIAPReceiver");
    }
    if (!m_IAPHelper) {
    	
    	m_IAPHelper = new SymbianClientApiHelper();
        if (!m_miniIAPReceiver) {
            RDebug::Printf("MiniIAPSymbianClient::prepare, #8, but NO m_miniIAPReceiver");
        }
    	m_IAPHelper->prepare(this);
    }
    if (!m_miniIAPReceiver) {
        RDebug::Printf("MiniIAPSymbianClient::prepare, #7, but NO m_miniIAPReceiver");
    }
}

