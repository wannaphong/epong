/*
 * Copyright (c) 2011-2014 Microsoft Mobile.
 */

/*
 ============================================================================
 Name		: EPongDocument.cpp
 Description : CEPongDocument implementation
 ============================================================================
 */

// INCLUDE FILES
#include "EPongAppUi.h"
#include "EPongDocument.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CEPongDocument::NewL()
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CEPongDocument* CEPongDocument::NewL(CEikApplication& aApp)
	{
	CEPongDocument* self = NewLC(aApp);
	CleanupStack::Pop(self);
	return self;
	}

// -----------------------------------------------------------------------------
// CEPongDocument::NewLC()
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CEPongDocument* CEPongDocument::NewLC(CEikApplication& aApp)
	{
	CEPongDocument* self = new (ELeave) CEPongDocument(aApp);

	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

// -----------------------------------------------------------------------------
// CEPongDocument::ConstructL()
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CEPongDocument::ConstructL()
	{
	// No implementation required
	}

// -----------------------------------------------------------------------------
// CEPongDocument::CEPongDocument()
// C++ default constructor can NOT contain any code, that might leave.
// -----------------------------------------------------------------------------
//
CEPongDocument::CEPongDocument(CEikApplication& aApp) :
	CAknDocument(aApp)
	{
	// No implementation required
	}

// ---------------------------------------------------------------------------
// CEPongDocument::~CEPongDocument()
// Destructor.
// ---------------------------------------------------------------------------
//
CEPongDocument::~CEPongDocument()
	{
	// No implementation required
	}

// ---------------------------------------------------------------------------
// CEPongDocument::CreateAppUiL()
// Constructs CreateAppUi.
// ---------------------------------------------------------------------------
//
CEikAppUi* CEPongDocument::CreateAppUiL()
	{
	// Create the application user interface, and return a pointer to it;
	// the framework takes ownership of this object
	return new (ELeave) CEPongAppUi;
	}

// End of File
