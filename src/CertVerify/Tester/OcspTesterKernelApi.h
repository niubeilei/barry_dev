////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: OcspTesterKernelApi.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Aos_CertVerify_Tester_OcspTesterKernelApi_h
#define Aos_CertVerify_Tester_OcspTesterKernelApi_h

struct aosKernelApiParms;
extern int aosTestOcspStartApi(
            char *data,
            unsigned int *length,
            struct aosKernelApiParms *parms,
            char *errmsg,
            const int errlen);

int aosTestOcspRetrieveApi(
            char *data,
            unsigned int *length,
            struct aosKernelApiParms *parms,
            char *errmsg,
            const int errlen);

#endif

