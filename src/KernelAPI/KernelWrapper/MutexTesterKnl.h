////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: MutexTesterKnl.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Aos_KernelAPI_KernelWrapper_MutexTesterKernel_h
#define Aos_KernelAPI_KernelWrapper_MutexTesterKernel_h


struct aosKernelApiParms;
extern int aos_mutex_tester_cli(char *data,
                       unsigned int *length,
                       struct aosKernelApiParms *parms,
                       char *errmsg,
                       const int errlen);

#endif

