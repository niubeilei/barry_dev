////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: MemBlock.cpp
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#include "Util1/MemBlock.h"

#include "Debug/Debug.h"
#include "Util/OmnNew.h"
#include "Util1/MemPool.h"


int OmnMemBlock::mObjCnt = 0;


bool
OmnMemBlock::clone(OmnMemBlock &block) const
{
	if (mMemPool)
	{
		mMemPool->allocate(__FILE__, __LINE__, block, mSize);
	}
	else
	{
		char *data = OmnNew char[mSize+5];

		data[0] = 0;
		block.init(0, mSize, &data[1], &data[0]);
	}

	return true;
}


void		
OmnMemBlock::removeRef()
{
	if (!mRef || !mData)
	{
		return;
	}

	(*mRef)--;
    if (*mRef == 0)
    {
        if (mMemPool)
        {
            mMemPool->release(mSize, mData);
        }
        else
        {
			mData--;	

			// Chen Ding, 05/10/2010
			// OmnDelete(mData);
			OmnDelete [] mData;
        }

		mData = 0;
    }
}
