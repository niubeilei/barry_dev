////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: LogSetLinesKernelApi.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef KERNELAPI_KERNELWRAPPER_LOGSETLINESKERNELAPI_H
#define KERNELAPI_KERNELWRAPPER_LOGSETLINESKERNELAPI_H

struct aosKernelApiParms;
int aosSetLogLinesKapi(
            char *data,
            unsigned int *length,
            struct aosKernelApiParms *parms,
            char *errmsg,
            const int errlen);

#endif

