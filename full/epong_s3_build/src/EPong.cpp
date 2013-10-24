/*
 * Copyright (c) 2011 Nokia Corporation.
 */

/*
 ============================================================================
 Name		: EPong.cpp
 Description : Main application class
 ============================================================================
 */

// INCLUDE FILES
#include <eikstart.h>
#include "EPongApplication.h"

LOCAL_C CApaApplication* NewApplication()
	{
	return new CEPongApplication;
	}

GLDEF_C TInt E32Main()
	{
	return EikStart::RunApplication(NewApplication);
	}

