/*
 * Copyright (c) 2011 Nokia Corporation.
 */

/*
 ============================================================================
 Name		: EPongDocument.h
 Author	  : 
 Description : Declares document class for application.
 ============================================================================
 */

#ifndef __EPongDOCUMENT_h__
#define __EPongDOCUMENT_h__

// INCLUDES
#include <akndoc.h>

// FORWARD DECLARATIONS
class CEPongAppUi;
class CEikApplication;

// CLASS DECLARATION

/**
 * CEPongDocument application class.
 * An instance of class CEPongDocument is the Document part of the
 * AVKON application framework for the EPong example application.
 */
class CEPongDocument : public CAknDocument
	{
public:
	// Constructors and destructor

	/**
	 * NewL.
	 * Two-phased constructor.
	 * Construct a CEPongDocument for the AVKON application aApp
	 * using two phase construction, and return a pointer
	 * to the created object.
	 * @param aApp Application creating this document.
	 * @return A pointer to the created instance of CEPongDocument.
	 */
	static CEPongDocument* NewL(CEikApplication& aApp);

	/**
	 * NewLC.
	 * Two-phased constructor.
	 * Construct a CEPongDocument for the AVKON application aApp
	 * using two phase construction, and return a pointer
	 * to the created object.
	 * @param aApp Application creating this document.
	 * @return A pointer to the created instance of CEPongDocument.
	 */
	static CEPongDocument* NewLC(CEikApplication& aApp);

	/**
	 * ~CEPongDocument
	 * Virtual Destructor.
	 */
	virtual ~CEPongDocument();

public:
	// Functions from base classes

	/**
	 * CreateAppUiL
	 * From CEikDocument, CreateAppUiL.
	 * Create a CEPongAppUi object and return a pointer to it.
	 * The object returned is owned by the Uikon framework.
	 * @return Pointer to created instance of AppUi.
	 */
	CEikAppUi* CreateAppUiL();

private:
	// Constructors

	/**
	 * ConstructL
	 * 2nd phase constructor.
	 */
	void ConstructL();

	/**
	 * CEPongDocument.
	 * C++ default constructor.
	 * @param aApp Application creating this document.
	 */
	CEPongDocument(CEikApplication& aApp);

	};

#endif // __EPongDOCUMENT_h__
// End of File
