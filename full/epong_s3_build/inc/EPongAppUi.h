/*
 * Copyright (c) 2011 Nokia Corporation.
 */

/*
 ============================================================================
 Name		: EPongAppUi.h
 Author	  : 
 Description : Declares UI class for application.
 ============================================================================
 */

#ifndef __EPongAPPUI_h__
#define __EPongAPPUI_h__

// INCLUDES
#include <aknappui.h>

// FORWARD DECLARATIONS
class EpocGameEngine;

// CLASS DECLARATION
/**
 * CEPongAppUi application UI class.
 * Interacts with the user through the UI and request message processing
 * from the handler class
 */
class CEPongAppUi : public CAknAppUi
	{
public:
	// Constructors and destructor

	/**
	 * ConstructL.
	 * 2nd phase constructor.
	 */
	void ConstructL();

	/**
	 * CEPongAppUi.
	 * C++ default constructor. This needs to be public due to
	 * the way the framework constructs the AppUi
	 */
	CEPongAppUi();

	/**
	 * ~CEPongAppUi.
	 * Virtual Destructor.
	 */
	virtual ~CEPongAppUi();

private:
	// Functions from base classes

	/**
	 * From CEikAppUi, HandleCommandL.
	 * Takes care of command handling.
	 * @param aCommand Command to be handled.
	 */
	virtual void HandleCommandL(TInt aCommand);

	/**
	 *  HandleStatusPaneSizeChange.
	 *  Called by the framework when the application status pane
	 *  size is changed.
	 */
	virtual void HandleStatusPaneSizeChange();
	
	virtual void HandleWsEventL(const TWsEvent &aEvent, CCoeControl *aDestination);

private:
	// Data

	/**
	 * The application view
	 * Owned by CEPongAppUi
	 */
	EpocGameEngine* iAppView;

	};

#endif // __EPongAPPUI_h__
// End of File
