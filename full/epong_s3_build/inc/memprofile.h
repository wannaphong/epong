/*
 * Copyright (c) 2011 Nokia Corporation.
 */

/*
 * memprofile.h
 */

#ifndef MEMPROFILE_H_
#define MEMPROFILE_H_

#include <e32base.h>
#include <egl/egl.h>


extern "C" /* static */ void GetProfilingExtensionData(EGLDisplay aDisp, int& aTotalGRAM, int& aUsedGRAM, int& aUsedPrivate, int& aUsedShared);


#endif /* MEMPROFILE_H_ */
