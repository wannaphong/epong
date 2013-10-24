/*
* Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:  Client/server interface for GOOM Monitor.
*
*/


#ifndef R_GOOMMONITORSESSION_H
#define R_GOOMMONITORSESSION_H

#include <e32std.h>

// Goom Events through Window Server
#define KGoomMemoryLowEvent 0x10282DBF
#define KGoomMemoryGoodEvent 0x20026790

/**
 *  The maximum amount of memory that can be allocated without permission.
 */
const TInt KGOomMaxAllocationWithoutPermission = 1048576;

/**
 *  The client interface for GOOM (Out Of Memory) monitor.
 *
 *  @lib oommonitor.lib
 */
class RGOomMonitorSession : public RSessionBase
    {
public:

    /**  Defines the application priorities of GOOM monitor. */
    enum TGOomPriority
        {
        /**
        * Application can be closed if needed.
        */
        EGOomPriorityNormal = 0,

        /**
        * Application should not be closed if possible.
        */
        EGOomPriorityHigh,

        /**
        * Application is busy and should not be closed.
        */
        EGOomPriorityBusy
        };

public:
    /**
    * Connects a new session.
    * Sessions must be connected before any other APIs can be used.
    * When the client has finished using a session, Close() must be called.
    * @return KErrNone if successful, error code otherwise.
    */
    IMPORT_C TInt Connect();

    /**
    * Tells whether Goom FW is really present and connected
    * In some platforms, there's no need for GOOM to run
    * and all operations do just nothing 
    */
    IMPORT_C TBool IsConnected();

    /**
    * Request that the GOOM monitor attempts to free some memory.
    * This function may take several seconds to execute, depending on
    * the memory state. It will not return until the attempt to recover
    * memory has completed.
    * @param aBytesRequested The number of bytes of free memory that the client requests.
    * @return KErrNone if the request memory is free. KErrNoMemory if that
    *         amount of memory could not be recovered.
    */
    IMPORT_C TInt RequestFreeMemory(TInt aBytesRequested);

    /**
    * Request that the GOOM monitor attempts to free some memory.
    * This is an asynchronous version of the request for free memory.
    * @param aBytesRequested The number of bytes of free memory that the client requests.
    * @param aStatus will be completed when the attempt to recover memory
    *        has finished. This may take several seconds, depending on
    *        the memory state. On completion, aStatus will be set to
    *        KErrNone if the request memory is free. KErrNoMemory if that
    *        amount of memory could not be recovered.
    */
    IMPORT_C void RequestFreeMemory(TInt aBytesRequested, TRequestStatus& aStatus);

    /**
    * Request that the GOOM monitor attempts to free some memory for an optional allocation.
    * The passed in plugin ID is used to determine the priority for this allocation.
    * Lower priority OOM actions may be executed to free enough RAM for this allocation.
    * This function may take several seconds to execute, depending on
    * the memory state. It will not return until the attempt to recover
    * memory has completed.
    * @param aBytesRequested The number of bytes of free memory that the client requests.
    * @param aPluginId The ID of the plugin that may delete the allocation in event of low memory.
    * @return KErrNone if the request memory is free. KErrNoMemory if that
    *         amount of memory could not be recovered.  
    */
    IMPORT_C TInt RequestOptionalRam(TInt aBytesRequested, TInt aMinimumBytesNeeded, TInt aPluginId, TInt& aBytesAvailable);

    /**
    * Request that the GOOM monitor attempts to free some memory for an optional allocation.
    * The passed in plugin ID is used to determine the priority for this allocation.
    * Lower priority OOM actions may be executed to free enough RAM for this allocation.
    * This function may take several seconds to execute, depending on
    * the memory state. It will not return until the attempt to recover
    * memory has completed.
    * @param aBytesRequested The number of bytes of free memory that the client requests.
    * @param aPluginId The ID of the plugin that may delete the allocation in event of low memory.
    * @param aStatus The TRequestStatus (completes with the number of bytes freed (aStatus >= 0) or an error (aStatus <= 0))
    * @return None
    */
    IMPORT_C void RequestOptionalRam(TInt aBytesRequested, TInt aMinimumBytesNeeded, TInt aPluginId, TRequestStatus& aStatus);

    /**
    * Cancels the asynchronous request for free memory.
    */
    IMPORT_C void CancelRequestFreeMemory();

    /**
    * Notify the GOOM monitor that this application is not responding
    * to the EEikCmdExit request to exit the application.
    * This function is used by the Avkon framework's app shutter.
    * @param aWgId the window group identifier of the app that is not exiting.
    */
    IMPORT_C void ThisAppIsNotExiting(TInt aWgId);

    /**
    * Notify the GOOM monitor that this application has the specified priority
    * @param aPriority the priority of the application
    */
    IMPORT_C void SetGOomPriority(TGOomPriority aPriority);

    /**
    * Notify the GOOM monitor that this application about to laucnh
    * effectively set the application specific threshold if found from 
    * config file
    * @param aAppUid the identifier of the application
    */
    IMPORT_C void ApplicationAboutToStart(const TUid& aAppUid);

    /**
    * Tell goom that number requested by application should be used as such
    * and not combined with currently active global threshold
    * @param aUseAbsoluteAmount whtether global thresholds are combined with targets
    */
    IMPORT_C void UsesAbsoluteMemTargets(TBool aUseAbsoluteAmount = ETrue);

    /**
    * Asynchronous version of Connect.
    * @param aStatus request status, completed later only 
    *        if return value equals to KErrNone.
    * @return error code, KErrNone if connection started.
    */
    IMPORT_C TInt Connect(TRequestStatus& aStatus);

    /**
    * Asynchronous version of AppAboutToStart.
    * @param aStatus request status.
    * @param aAppUid application UID being started.
    */
    IMPORT_C void AppAboutToStart(TRequestStatus& aStatus, const TUid& aAppUid);
    
    /**
    * Notify the GOOM monitor that this application has finished allocating memory.  
    */
    IMPORT_C void MemoryAllocationsComplete();

private://data 
    TInt iFlags;
    };
    
    

#endif // R_GOOMMONITORSESSION_H
