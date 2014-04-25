/*
 * Copyright (c) 2011-2014 Microsoft Mobile.
 */

/*
 ============================================================================
 Name		: EPongApplication.cpp
 Description : Main application class
 ============================================================================
 */

// INCLUDE FILES
#include "EPong.hrh"
#include "EPongDocument.h"
#include "EPongApplication.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CEPongApplication::CreateDocumentL()
// Creates CApaDocument object
// -----------------------------------------------------------------------------
//
CApaDocument* CEPongApplication::CreateDocumentL()
	{
	// Create an EPong document, and return a pointer to it
	return CEPongDocument::NewL(*this);
	}

// -----------------------------------------------------------------------------
// CEPongApplication::AppDllUid()
// Returns application UID
// -----------------------------------------------------------------------------
//
TUid CEPongApplication::AppDllUid() const
	{
	// Return the UID for the EPong application
	return KUidEPongApp;
	}

// End of File
