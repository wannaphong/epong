/*
 * Copyright (c) 2011 Nokia Corporation.
 */

/*
 ============================================================================
 Name		: EPongApplication.h
 Author	  : 
 Description : Declares main application class.
 ============================================================================
 */

#ifndef __EPongAPPLICATION_H__
#define __EPongAPPLICATION_H__

// INCLUDES
#include <aknapp.h>
#include "EPong.hrh"

// UID for the application;
// this should correspond to the uid defined in the mmp file
const TUid KUidEPongApp =
	{
	_UID3
	};

// CLASS DECLARATION

/**
 * CEPongApplication application class.
 * Provides factory to create concrete document object.
 * An instance of CEPongApplication is the application part of the
 * AVKON application framework for the EPong example application.
 */
class CEPongApplication : public CAknApplication
	{
public:
	// Functions from base classes

	/**
	 * From CApaApplication, AppDllUid.
	 * @return Application's UID (KUidEPongApp).
	 */
	TUid AppDllUid() const;

protected:
	// Functions from base classes

	/**
	 * From CApaApplication, CreateDocumentL.
	 * Creates CEPongDocument document object. The returned
	 * pointer in not owned by the CEPongApplication object.
	 * @return A pointer to the created document object.
	 */
	CApaDocument* CreateDocumentL();
	};

#endif // __EPongAPPLICATION_H__
// End of File
