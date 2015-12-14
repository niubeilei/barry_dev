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
#include "SemanticObj/LockSO.h"

#include "SemanticObj/ThreadSO.h"


AosLockSO::AosLockSO()
:
mIsLocked(false)
{
}


AosLockSO::~AosLockSO()
{
}


AosThreadSOPtr
AosLockSO::getOwner() const
{
	return mOwner;
}


void
AosLockSO::setOwner(const AosThreadSOPtr &owner)
{
	mOwner = owner;
}

