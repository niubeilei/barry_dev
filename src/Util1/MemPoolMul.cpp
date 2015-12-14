////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: MemPoolMul.cpp
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#include "Util1/MemPoolMul.h"

#include "Debug/Error.h"
#include "Debug/Debug.h"
#include "Alarm/Alarm.h"
#include "Thread/Mutex.h"
#include "Util/OmnNew.h"
#include "Util1/MemBlock.h"
#include "Util1/MemMgr.h"
#include "Util1/MemPool.h"
#include "Util1/Ptrs.h"


OmnMemPoolPtr OmnMemPoolMul::mSize1Pool = OmnNew OmnMemPool(
	OmnMemPoolMul::eSize1, 
	OmnMemPoolMul::eMaxBlocks1);

OmnMemPoolPtr OmnMemPoolMul::mSize2Pool = OmnNew OmnMemPool(
	OmnMemPoolMul::eSize2, 
	OmnMemPoolMul::eMaxBlocks2);

OmnMemPoolPtr OmnMemPoolMul::mSize3Pool = OmnNew OmnMemPool(
	OmnMemPoolMul::eSize3, 
	OmnMemPoolMul::eMaxBlocks3);

OmnMemPoolPtr OmnMemPoolMul::mSize4Pool = OmnNew OmnMemPool(
	OmnMemPoolMul::eSize4, 
	OmnMemPoolMul::eMaxBlocks4);

OmnMemPoolPtr OmnMemPoolMul::mSize5Pool = OmnNew OmnMemPool(
	OmnMemPoolMul::eSize5, 
	OmnMemPoolMul::eMaxBlocks5);

int OmnMemPoolMul::mTotalOversizeObj = 0;

bool
OmnMemPoolMul::allocate(const char *file, 
						const int line, 
						OmnMemBlock &block)
{
	return allocate(file, line, block, eSize1);
}


bool
OmnMemPoolMul::allocate(const char *file, 
						const int line, 
						OmnMemBlock &block, 
						const uint &size)
{
	//
	// When needs a block from this memory pool, call this function to get
	// one. If there are idle blocks, it returns directly. Otherwise, 
	// it news a block and returns it. It is important to return the 
	// block when not needed. Otherwise, it is memory leak.
	//

	//
	// Check whether the size is correct
	//

	//OmnTrace << "Allocate: " << size << endl;

	if (size <= eSize1 && !mSize1Pool.isNull())
	{
		return mSize1Pool->allocate(file, line, block, eSize1);
	}

	if (size <= eSize2 && !mSize2Pool.isNull())
	{
		return mSize2Pool->allocate(file, line, block, eSize2);
	}

	if (size <= eSize3 && !mSize3Pool.isNull())
	{
		return mSize3Pool->allocate(file, line, block, eSize3);
	}

	if (size <= eSize4 && !mSize4Pool.isNull())
	{
		return mSize4Pool->allocate(file, line, block, eSize4);
	}

	if (size <= eSize5 && !mSize5Pool.isNull())
	{
		return mSize5Pool->allocate(file, line, block, eSize5);
	}

	mTotalOversizeObj++;
	//char *data = (char *)::operator OmnNew(size+5);
	char *data = OmnNew char[size+5];	

//	OmnTrace << "Allocated: " << (void*)data << ". size = " << size << endl;

	data[0] = 0;
	block.init(0, size, &data[1], &data[0]);

	return true;
}


void		
OmnMemPoolMul::release(const uint size, char *ptr)
{
	//
	// Check the size
	//
	switch (size)
	{
	case eSize1:
		 mSize1Pool->release(eSize1, ptr);
		 break;

	case eSize2:
		 mSize2Pool->release(eSize2, ptr);
		 break;

	case eSize3:
		 mSize3Pool->release(eSize3, ptr);
		 break;

	case eSize4:
		 mSize4Pool->release(eSize4, ptr);
		 break;

	case eSize5:
		 mSize5Pool->release(eSize5, ptr);
		 break;

	default:
		ptr--;
		mTotalOversizeObj--;
		OmnTrace << "To delete memory: " 
			<< (void*)ptr << ". size = " << size << endl;

		cout << __FILE__ << ":" << __LINE__ << "To delete: " << (void*)ptr << endl;
		//::operator OmnDelete(ptr);
		OmnDelete(ptr);
		return;
	}
}


uint
OmnMemPoolMul::purge()
{
	//
	// This function may be called to release some memory, if any.
	// This may be due to the system failed to allocate memory.
	// If the idle list is not empty, it releases half of the blocks.
	// It reutrns the total number of bytes released. If no bytes 
	// are released, return 0.
	//
	uint purged = mSize1Pool->purge();
	purged += mSize2Pool->purge();
	purged += mSize3Pool->purge();
	purged += mSize4Pool->purge();
	purged += mSize5Pool->purge();

	return purged;
}


void
OmnMemPoolMul::checkMemory(OmnString &result)
{
	mSize1Pool->checkMemory(result);
	mSize2Pool->checkMemory(result);
	mSize3Pool->checkMemory(result);
	mSize4Pool->checkMemory(result);
	mSize5Pool->checkMemory(result);
}


OmnString
OmnMemPoolMul::getObjCnt()
{
	OmnString str;
	
	str << "MemPoolMul::mTotalOversizeObj: " 
		<< OmnStrUtil::itoa(mTotalOversizeObj) << "\n";

	str << "Size1: " << mSize1Pool->getObjCnt();
	str << "Size2: " << mSize2Pool->getObjCnt();
	str << "Size3: " << mSize3Pool->getObjCnt();
	str << "Size4: " << mSize4Pool->getObjCnt();
	str << "Size5: " << mSize5Pool->getObjCnt();

	return str;
}
