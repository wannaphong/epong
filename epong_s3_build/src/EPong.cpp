/*
 * Copyright (c) 2011-2014 Microsoft Mobile.
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

