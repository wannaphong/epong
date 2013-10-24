/*
 * Copyright (c) 2011 Nokia Corporation.
 */

/*
 ============================================================================
 Name		: EPong.pan
 Author	  : 
 Description : This file contains panic codes.
 ============================================================================
 */

#ifndef __EPong_PAN__
#define __EPong_PAN__

/** EPong application panic codes */
enum TEPongPanics
	{
	EEPongUi = 1
	// add further panics here
	};

inline void Panic(TEPongPanics aReason)
	{
	_LIT(applicationName, "EPong");
	User::Panic(applicationName, aReason);
	}

#endif // __EPong_PAN__
