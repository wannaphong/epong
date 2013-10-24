/*
 * Copyright (c) 2011 Nokia Corporation.
 */

#include "memprofile.h"

void GetProfilingExtensionData(EGLDisplay aDisp, int& aTotalGRAM, int& aUsedGRAM, int& aUsedPrivate, int& aUsedShared)
{
    // Get profiling extension pointer (NULL if not there)
    static EGLBoolean (*eglQueryProfilingDataNOK)(EGLDisplay dpy, EGLint query_bits, EGLint *data, EGLint data_size, EGLint *data_count)=NULL;

    if (!eglQueryProfilingDataNOK)
    {
        eglQueryProfilingDataNOK = (EGLBoolean (*)(EGLDisplay, EGLint, EGLint *, EGLint, EGLint *)) eglGetProcAddress("eglQueryProfilingDataNOK");
    }

    if (eglQueryProfilingDataNOK)
    {
    	TUint64 processid;
        RProcess proc;
        processid = proc.Id().Id();
        EGLint data_count;

        eglQueryProfilingDataNOK(aDisp, EGL_PROF_QUERY_MEMORY_USAGE_BIT_NOK | EGL_PROF_QUERY_GLOBAL_BIT_NOK, NULL, 0, &data_count);
        EGLint* data = static_cast<EGLint *>(new EGLint[data_count]);
        eglQueryProfilingDataNOK(aDisp, EGL_PROF_QUERY_MEMORY_USAGE_BIT_NOK | EGL_PROF_QUERY_GLOBAL_BIT_NOK, data, data_count, &data_count);

        int i = 0;
        bool enable = false;

        while(i < data_count)
        {
            switch(data[i++])
            {
                case(EGL_PROF_TOTAL_MEMORY_NOK):
                {
                    aTotalGRAM = data[i++];
                    break;
                }
                case(EGL_PROF_USED_MEMORY_NOK):
                {
                    aUsedGRAM = data[i++];
                    break;
                }
                case(EGL_PROF_PROCESS_ID_NOK):
                {
                    TUint64 process_id = ((TUint64)(data[i])) + (((TUint64)(data[i + 1])) << 32);
                    i += 2;
                    if(processid == process_id)
                        enable = true;
                    else
                        enable = false;
                    break;
                }
                case(EGL_PROF_PROCESS_USED_PRIVATE_MEMORY_NOK):
                {
                    if(enable)
                        aUsedPrivate = data[i];
                    i++;
                    break;
                }
                case(EGL_PROF_PROCESS_USED_SHARED_MEMORY_NOK):
                {
                    if(enable)
                        aUsedShared = data[i];
                    i++;
                    break;
                }
                default:
                {
                  i++;
                  break;
                }
            }
        }
        delete data;
    }
    else
    {
        aTotalGRAM = -1;
        aUsedGRAM  = -1;
    }

}

