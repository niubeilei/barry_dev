////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
//
// Modification History:
// 04/26/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_Task_Ptrs_h
#define Aos_Task_Ptrs_h

#include "Util/SPtr.h"


OmnPtrDecl(AosTaskScheduler, AosTaskSchedulerPtr)
OmnPtrDecl(AosTaskThrd, AosTaskThrdPtr)
OmnPtrDecl(AosTaskMgr, AosTaskMgrPtr)
OmnPtrDecl(AosTaskAction, AosTaskActionPtr)
OmnPtrDecl(AosDocidAllocator, AosDocidAllocatorPtr)		// Ketty 2013/12/23

#endif
