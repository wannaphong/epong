/*
 * Copyright (c) 2011 Nokia Corporation.
 */

/*
 ============================================================================
 Name		: HdmiOutput.cpp
 Description : Creates window for HDMI display when HDMI cable is plugged in
 ============================================================================
 */
#include "HdmiOutput.h"
_LIT(KWgName, "HdmiOutput");
const TUint32 KWgId = 12345678; // TODO: define unique id
const TUint32 KHdScreen = 1;

CHdmiOutput::CHdmiOutput()
	{
	// No implementation required
	}

CHdmiOutput::~CHdmiOutput()
	{
	delete iAccMonitor;
	iAccMonitor = NULL;
	DestroyWindow();
	}

CHdmiOutput* CHdmiOutput::NewLC()
	{
	CHdmiOutput* self = new (ELeave) CHdmiOutput();
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

CHdmiOutput* CHdmiOutput::NewL()
	{
	CHdmiOutput* self = CHdmiOutput::NewLC();
	CleanupStack::Pop(); // self;
	return self;
	}

void CHdmiOutput::ConstructL()
	{
	iAccMonitor = CAccMonitor::NewL();

	// invoke callback for all currently connected accessories
	RConnectedAccessories array;
	CleanupClosePushL(array);
	iAccMonitor->GetConnectedAccessoriesL(array);
	for (int i = 0; i < array.Count(); i++)
		{
		ConnectedL(array[i]);
		}
	CleanupStack::PopAndDestroy(&array);

	// register to listen for changes
	RAccMonCapabilityArray capabilityArray;
	CleanupClosePushL(capabilityArray);
	capabilityArray.Append(KAccMonHDMI);
	iAccMonitor->StartObservingL(this, capabilityArray);
	CleanupStack::PopAndDestroy(&capabilityArray);
	}

void CHdmiOutput::ConnectedL(CAccMonitorInfo *aAccessoryInfo)
	{
	if (aAccessoryInfo->Exists(KAccMonHDMI))
		{
		CreateWindow();
		if (iCallback)
			iCallback->OutputChanged(this);
		}
	}

void CHdmiOutput::CreateWindow()
	{

	if (iWindow)
		return;
	iWsSession.Connect();

	// create screen device
	iScreenDevice = new (ELeave) CWsScreenDevice(iWsSession);
	TInt err = iScreenDevice->Construct(KHdScreen);
	User::LeaveIfError(err);

	iWsSession.ComputeMode(RWsSession::EPriorityControlDisabled);

	// create window group
	iWindowGroup = new (ELeave) RWindowGroup(iWsSession);
	err = iWindowGroup->Construct(KWgId, iScreenDevice);
	User::LeaveIfError(err);
	err = iWindowGroup->SetName(KWgName);
	User::LeaveIfError(err);

	iWindowGroup->EnableReceiptOfFocus(EFalse);

	TSize size = iScreenDevice->SizeInPixels();

	iWindow = new (ELeave) RWindow(iWsSession);
	User::LeaveIfError(iWindow->Construct(*iWindowGroup, (TUint32) iWindow));

	iWindow->SetExtent(TPoint(0, 0), size);
	iWindow->SetOrdinalPosition(0, ECoeWinPriorityAlwaysAtFront + 1);
	iWindow->SetNonFading(ETrue);
	iWindow->SetVisible(ETrue);
	iWindow->Activate();

	iHdmiEnabled = ETrue;
	}

void CHdmiOutput::DisconnectedL(CAccMonitorInfo *aAccessoryInfo)
	{
	if (aAccessoryInfo->Exists(KAccMonHDMI))
		{
		iHdmiEnabled = EFalse;
		if (iCallback)
			iCallback->OutputChanged(this);
		DestroyWindow();
		}
	}

void CHdmiOutput::DestroyWindow()
	{
	if (!iWindow)
		return;

	iWindow->Close();
	delete iWindow;
	iWindow = NULL;

	iWindowGroup->Close();
	delete iWindowGroup;
	iWindowGroup = NULL;

	iWsSession.Close();
	}

void CHdmiOutput::AccMonitorObserverError(TInt aError)
	{
	}

TBool CHdmiOutput::IsHdmiConnected()
	{
	return iHdmiEnabled;
	}

RWindow *CHdmiOutput::HdmiWindow()
	{
	if (IsHdmiConnected())
		return iWindow;
	else
		return NULL;
	}

