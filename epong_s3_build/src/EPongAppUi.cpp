/*
 * Copyright (c) 2011-2014 Microsoft Mobile.
 */

/*
 ============================================================================
 Name		: EPongAppUi.cpp
 Description : CEPongAppUi implementation
 ============================================================================
 */

// INCLUDE FILES
#include <avkon.hrh>
#include <e32debug.h>

#include "EPong.hrh"
#include "EPong.pan"
#include "EPongApplication.h"
#include "EPongAppUi.h"
#include "EpocGameEngine.h"

#include <aknquerydialog.h>
#include <aknnotewrappers.h>
#include <aknmessagequerydialog.h>
#include <avkon.rsg>

// Goom Events through Window Server
const int KGoomMemoryLowEvent = 0x10282DBF;
const int KGoomMemoryGoodEvent = 0x20026790;

// ============================ MEMBER FUNCTIONS ===============================


// -----------------------------------------------------------------------------
// CEPongAppUi::ConstructL()
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CEPongAppUi::ConstructL()
	{
	// Initialise app UI with standard value.
	BaseConstructL(EAppOrientationLandscape | CAknAppUi::EAknEnableSkin);
	// Create view object
	iAppView = EpocGameEngine::NewL(ClientRect());
	// @TODO TVwsViewId viewId;
	//if(!GetActiveViewId(viewId))
	iAppView->SetMopParent(this);
	AddToStackL(iAppView);
	}
// -----------------------------------------------------------------------------
// CEPongAppUi::CEPongAppUi()
// C++ default constructor can NOT contain any code, that might leave.
// -----------------------------------------------------------------------------
//
CEPongAppUi::CEPongAppUi()
	{
	// No implementation required
	}

// -----------------------------------------------------------------------------
// CEPongAppUi::~CEPongAppUi()
// Destructor.
// -----------------------------------------------------------------------------
//
CEPongAppUi::~CEPongAppUi()
	{
	if (iAppView)
		{
		delete iAppView;
		iAppView = NULL;
		}
	}

// -----------------------------------------------------------------------------
// CEPongAppUi::HandleCommandL()
// Takes care of command handling.
// -----------------------------------------------------------------------------
//
void CEPongAppUi::HandleCommandL(TInt aCommand)
	{
	switch (aCommand)
		{
		case EEikCmdExit:
		case EAknSoftkeyExit:
			Exit();
			break;

		}
	}
// -----------------------------------------------------------------------------
//  Called by the framework when the application status pane
//  size is changed.  Passes the new client rectangle to the
//  AppView
// -----------------------------------------------------------------------------
//
void CEPongAppUi::HandleStatusPaneSizeChange()
	{
	if (iAppView)
		iAppView->SetRect(ClientRect());
	}

void CEPongAppUi::HandleWsEventL(const TWsEvent &aEvent, CCoeControl *aDestination)
	{
	if (aEvent.Type() == EEventUser)
		{
		TApaSystemEvent* eventData = reinterpret_cast<TApaSystemEvent*> (aEvent.EventData());
		if ((*eventData) == EApaSystemEventShutdown)
			{
			eventData++;
			if ((*eventData) == KGoomMemoryLowEvent)
				{
				return; // skip default handling
				}
			}
		}

	if (aEvent.Type() == EEventFocusGained)
		{
		if (iAppView)
			iAppView->startEngine();
		}
	else if (aEvent.Type() == EEventFocusLost)
		{
		if (iAppView)
			iAppView->stopEngine(ETrue);
		}

	CAknAppUi::HandleWsEventL(aEvent, aDestination);
	}

// End of File
