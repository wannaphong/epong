/**
 * Copyright (c) 2011-2014 Microsoft Mobile and/or its subsidiary(-ies).
 * All rights reserved.
 *
 * For the applicable distribution terms see the license.txt -file, included in
 * the distribution.
 */

#include "drmfile_native_symbian.h"

#include <caf/caferr.h>
#include <caf/embeddedobject.h>
#include <caf/streamableptrarray.h>
#include <caf/data.h>

#include <e32base.h>
#include <f32file.h>

DRMFileNativeSymbian::DRMFileNativeSymbian() :
        CAF_file(NULL), CAF_data(NULL)
{

}

DRMFileNativeSymbian::~DRMFileNativeSymbian()
{
    close();
}

int DRMFileNativeSymbian::open(TPtrC16 fileName_TextPtr)
{
    int result = KErrNone;

    // QString symbianFileName;
    // symbianFileName = fileName;
    // QChar find('/');
    // QChar replace('\\');
    // symbianFileName.replace(find,replace,Qt::CaseInsensitive);
    // TPtrC16 fileName_TextPtr(reinterpret_cast<const TUint16*>(symbianFileName.utf16()));

    TRAPD(panic_code, result = openL(fileName_TextPtr))
    if(panic_code != KErrNone)
        result = panic_code;
    else
        intentExecuted = EFalse;

    return result;
}

int DRMFileNativeSymbian::read(unsigned char*& data, int length)
{
    int result = KErrNone;
    data = NULL;

    while(true)
    {
        if(CAF_data == NULL)
        {
            result = KErrBadHandle;
            break;
        }

        RBuf8 buffer;
        if(!length)
            TRAP(result, CAF_data->DataSizeL(length));

        if(result != KErrNone)
            break;

        length++; //to be able use PtrZ() call

        TRAP (result, buffer.CreateL(length));
        if(result != KErrNone)
            break;

        if(!intentExecuted)
            result = CAF_data->ExecuteIntent(ContentAccess::EExecute);

        if(result != KErrNone)
            break;

        intentExecuted = ETrue;
        result = CAF_data->Read(buffer);
        if(result != KErrNone)
            break;

        result = length;
        unsigned char* ptr = const_cast<unsigned char*>(buffer.PtrZ()); //give up ownership from RBuf8
        data = ptr;
        break;
    }

    if(result < KErrNone)
        RDebug::Print(_L("DRMFilePrivate::read ContentAccess::CData::Read error %d"), result);

    return result;
}

void DRMFileNativeSymbian::close()
{
    if(CAF_data!=NULL)
    {
        delete CAF_data;
        CAF_data = NULL;
    }
    if(CAF_file!=NULL)
    {
        delete CAF_file;
        CAF_file = NULL;
    }    
}

int DRMFileNativeSymbian::openL(const TDesC& fileName)
{
    int result = KErrNone;

    if((CAF_file!=NULL) || (CAF_data!=NULL)) 
    	return KErrInUse;
    
    CAF_file = CContent::NewL(fileName);

    RStreamablePtrArray <CEmbeddedObject> myArray;
	CleanupClosePushL(myArray);

	// Get an array of the embedded objects within the current container in the file
	CAF_file->GetEmbeddedObjectsL(myArray);
	if(myArray.Count() > 0)
	{
		// open the first object, multiple objects per container are not yet supported
                CAF_data = CAF_file->OpenContentL(ContentAccess::EExecute, myArray[0]->UniqueId());
	}
	else
	{
		// no objects found
		result = KErrNotFound;
	}

	CleanupStack::PopAndDestroy(&myArray);
	if (result < 0)
		RDebug::Print(_L("DRMFilePrivate::read ContentAccess::CData::Read error %d"), result);

	return result;
}

int DRMFileNativeSymbian::size()
{
        int size = 0;
        if(CAF_data!=NULL)
                TRAP_IGNORE(CAF_data->DataSizeL(size));
        return size;
}

//#ifdef SYMBIAN_ENABLE_64_BIT_FILE_SERVER_API
//qint64 DRMFileNativeSymbian::size64()
//{
//	TInt64 size = 0;
//	if(CAF_data!=NULL)
//		TRAP_IGNORE(CAF_data->DataSize64L(size));
//	return size;
//}
//#endif

bool DRMFileNativeSymbian::isDRMError(int error)
{
	bool result = false;
	if(KErrCA_LowerLimit <= error && error <= KErrCA_UpperLimit)
		result = true;
	return result;
}

