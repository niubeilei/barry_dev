////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: Util.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef aosUtil_Util_h
#define aosUtil_Util_h

struct aosKernelApiParms;

extern int AosUtil_showCli(
			char *data,
            unsigned int *length,
            struct aosKernelApiParms *parms,
            char *errmsg,
            const int errlen);
extern int AosUtil_clearConfCli(
                char *data,
                unsigned int *length,
                struct aosKernelApiParms *parms,
                char *errmsg,
                const int errlen);
extern int AosUtil_saveConfCli(
                char *data,
                unsigned int *length,
                struct aosKernelApiParms *parms,
                char *errmsg,
                const int errlen);
#endif

