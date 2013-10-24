/*
 * Copyright (c) 2011 Nokia Corporation.
 */

/*
 ============================================================================
 Name		: HdmiOutput.h
 Author	  : jarnoh
 Version	 : 1.0
 Description : Provides HDMI output window and callback when HDMI is plugged in
 ============================================================================
 */

#ifndef HDMIOUTPUT_H
#define HDMIOUTPUT_H

// INCLUDES
#include <e32std.h>
#include <e32base.h>

#include <w32std.h>
#include <coedef.h>


// CAccMonitor API
#include <AccMonitor.h>

// CLASS DECLARATION

class CHdmiOutput;

class MHdmiOutputCallback 
	{
public:
	virtual void OutputChanged(CHdmiOutput *self)=0;
	};


/**
 *  CHdmiOutput
 * 
 */
class CHdmiOutput : public CBase, public MAccMonitorObserver 
	{
public:
	// Constructors and destructor

	/**
	 * Destructor.
	 */
	~CHdmiOutput();

	/**
	 * Two-phased constructor.
	 */
	static CHdmiOutput* NewL();

	/**
	 * Two-phased constructor.
	 */
	static CHdmiOutput* NewLC();

public:
	TBool IsHdmiConnected();
	
	RWindow *HdmiWindow();
	
	// MAccMonitorObserver
private:
	virtual void ConnectedL(CAccMonitorInfo *aAccessoryInfo);
	virtual void DisconnectedL(CAccMonitorInfo *aAccessoryInfo);
	virtual void AccMonitorObserverError(TInt aError);

private:	
	virtual void CreateWindow();
	virtual void DestroyWindow();
	
private:

	/**
	 * Constructor for performing 1st stage construction
	 */
	CHdmiOutput();

	/**
	 * EPOC default constructor for performing 2nd stage construction
	 */
	void ConstructL();

	CAccMonitor *iAccMonitor;

    RWsSession iWsSession;
    CWsScreenDevice *iScreenDevice;
    RWindowGroup *iWindowGroup;
    RWindow *iWindow;

    TBool iHdmiEnabled;
public:
    MHdmiOutputCallback *iCallback;
	
	};

#endif // HDMIOUTPUT_H
