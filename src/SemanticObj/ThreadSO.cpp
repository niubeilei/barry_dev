////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// Description:
//   
//
// Modification History:
// 12/07/2007: Created by Chen Ding
// 
////////////////////////////////////////////////////////////////////////////
#include "SemanticObj/ThreadSO.h"

#include "Semantics/Asserts.h"
#include "SemanticObj/LockSO.h"



AosLockSOPtr
AosThreadSO::getPendingLock() const
{
	return mPendingLock;
}


void
AosThreadSO::tryToLock(const AosLockSOPtr &lock)
{
	// 
	// A thread shall not lock more than one lock at a time.
	// This means mIsTryingToLock shall be false and 
	// mPendingLock shall be null.
	//
	aos_semantic_assert(!mIsTryingToLock);
	aos_semantic_assert(!mPendingLock);

	mIsTryingToLock = true;
	mPendingLock = lock;
	return;
}

