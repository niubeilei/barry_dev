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
// 	05/11/2010	Copied from MemBlock.cpp
////////////////////////////////////////////////////////////////////////////
#include "Util1/Memory.h"

#include "Debug/Debug.h"
#include "Util/OmnNew.h"
#include "Util1/MemPool.h"



bool
AosMemory::clone(AosMemory &block) const
{
	if (!mMemPool)
	{
		cout << "<" << __FILE__ << ":" << __LINE__ << "> **********: "
			<< "Pool is null!" << endl;
		return false;
	}

	char *data = mMemPool->allocate(mSize, __FILE__, __LINE__);
	block.init(mMemPool, mSize, data);
	return true;
}

