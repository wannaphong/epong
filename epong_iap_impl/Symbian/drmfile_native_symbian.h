/**
 * Copyright (c) 2011 Nokia Corporation and/or its subsidiary(-ies).
 * All rights reserved.
 *
 * For the applicable distribution terms see the license.txt -file, included in
 * the distribution.
 */

#ifndef DRMFILE_P_H
#define DRMFILE_P_H

#include <caf/content.h>

using namespace ContentAccess;

/**
 * @class DRMFilePrivate
 *
 * @brief Content Access Framework (CAF) wrapper implementation
 *
 */
class DRMFileNativeSymbian
{
public:
    /**
     * @brief constructor
     *
     */
    explicit DRMFileNativeSymbian();
    /**
     * @brief destructor
     *
     */
    virtual ~DRMFileNativeSymbian();

public:
    /**
     * @brief lets Content Access Framework to open either DRM encrypted file package or non-protected file
     *        NOTE: Access to unprotected content is unrestricted by CAF thus regular files can
     *              be opened and read successfully
     *
     * @param fileName
     */
    int open(TPtrC16 fileName_TextPtr);

    /**
     * @brief Reads the whole data or requested length into allocated memory buffer.
     *
     * @param data   - reference to pointer on allocated memory buffer.
     *                 It is client's responcibility to release allocated memory
     * @param length - requested data length to read, if it is equal to '0' it will read the whole file
     */
    int read(unsigned char*& data, int length = 0);
    /**
     * @brief closes file or DRM file package
     *
     */
    void close();
    /**
     * @brief returns opened file or DRM file package
     *
     */
    int size();
//#ifdef SYMBIAN_ENABLE_64_BIT_FILE_SERVER_API
//    qint64 size64();
//#endif
    /**
     * @brief obsolete, checks whether given error code belongs to CAF error code range
     *
     * @param error
     */
    static bool isDRMError(int error);
    
private:
    /**
     * @brief function helper to 'open' method
     *
     * @param fileName
     */
    int openL(const TDesC& fileName);
   
private:
    CContent *CAF_file; /**< TODO */
    CData *CAF_data; /**< TODO */
    TBool intentExecuted; /**< TODO */
};

#endif // DRMFILE_P_H
