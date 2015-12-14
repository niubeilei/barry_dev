////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: KernelLock.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_KernelUtil_KernelLock_h
#define Omn_KernelUtil_KernelLock_h

#include "Porting/Mutex.h"

// 
// This is the userland simulation of kernel locks. 
//
#define OmnKernelLockDef OmnMutexType

inline void OmnKernelLockInit(OmnKernelLockDef *lock)
{
	OmnInitMutex(*lock);
}


inline void OmnKernelLockDestroy(OmnKernelLockDef *lock)
{
	OmnDestroyMutex(*lock);
}


inline void OmnKernelLock(OmnKernelLockDef *lock)
{
	OmnMutexLock(*lock);
}


inline void OmnKernelUnlock(OmnKernelLockDef *lock)
{
	OmnMutexUnlock(*lock);
}



#endif