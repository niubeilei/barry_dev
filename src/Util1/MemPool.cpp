////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: MemPool.cpp
// Description:
//	This class maintains a list of mSize memory blocks. When someone
//  wants a block of this size, call allocate(size). When someone
//  finishes the memory, call release(size, ptr). Note that the size
//  in the two member function calls must match mSize. Otherwise, 
//  it uses ::operator new() instead. 
//
//  This class is used as a more efficient way of handling memory.
//  Refer to Mayer Book for details.
//
//  mNumElem tells how many idle blocks are in the list. This number
//  should not exceed mMaxBlocks. When it exceeds, blocks are deleted
//  instead of being inserted into the list.
//
//  This class assumes OmnMemMgr. When an instance is created, it 
//  registers with MemMgr. When an instance is deleted, it unregisters
//  from it. When the system requires more memory, OmnMemMgr may call
//  this class' purge() member function to release more memory.
//
//	mTotalAllocated keeps track of how many blocks have been allocated
//  so far. mTotalAllocated - mSize is the number of blocks allocated
//  and held by someone. If this number keeps on growing, it is a 
//  good indication of memory leak.   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#include "Util1/MemPool.h"

#include "Debug/Error.h"
#include "Alarm/Alarm.h"
#include "Thread/Mutex.h"
#include "Util/OmnNew.h"
#include "Util1/MemBlock.h"
#include "Util1/MemMgr.h"
#include "Util1/Ptrs.h"

// add iostream.h head file by xiaoqing 03-17-2006
// in order to resolve userland compile issue
#include <iostream>

using namespace std;
extern OmnMemMgr * OmnMemMgrSelf;


OmnMemPool::OmnMemPool(const uint size,
					   const uint maxBlocks)
:
mHead(0),
mSize(size),
mLock(OmnNew OmnMutex()),
mNumElem(0),
mMaxBlocks(maxBlocks),
mTotalAllocated(0)
{
}


OmnMemPool::~OmnMemPool()
{
	//
	// Release all memory
	Entry *next = mHead;
	while (mHead)
	{
		next = mHead->mNext;

		// Chen Ding, 05/10/2010
		// OmnDelete mHead;
		OmnDelete [] mHead;
		mHead = next;
		mTotalAllocated--;
		mNumElem--;
	}	
}


char *
OmnMemPool::allocate(const char *file, 
					 const int line, 
					 const uint size)
{
    //
    // When needs a block from this memory pool, call this function to get
    // one. If there are idle blocks, it returns directly. Otherwise,
    // it news a block and returns it. It is important to return the
    // block when not needed. Otherwise, it is memory leak.
    mLock->lock();

    // Check whether the size is correct
    if (size != mSize)
    {
        OmnAlarm << "Memory Pool size mismatch: "
            << " Actual size: " << size
            << " Expected size: " << mSize << enderr;

		mTotalAllocated++;
		char *data = OmnNew char[size+5];
		mLock->unlock();
		return ++data;
    }

    //
    // If the idle list is not empty, get one from the list.
    //
    if (mHead)
    {
        //
        // It is not empty.
        //
        Entry *ptr = mHead;
        mHead = mHead->mNext;
        mNumElem--;
        mLock->unlock();
		char *data = (char *)ptr;
		return ++data;
    }

    //
    // It is empty. Allocate one
    //
    mTotalAllocated++;
    mLock->unlock();
	char *data = OmnNew char[size+5];
	return ++data;
}

bool
OmnMemPool::allocate(const char *file, 
					 const int line, 
					 OmnMemBlock &block, 
					 const uint size)
{
	//
	// When needs a block from this memory pool, call this function to get
	// one. If there are idle blocks, it returns directly. Otherwise, 
	// it news a block and returns it. It is important to return the 
	// block when not needed. Otherwise, it is memory leak.
	//

	//OmnTrace << "To allocate: [" << file << ":" << line << endl;

	//
	// Check whether the size is correct
	//
	mLock->lock();
	if (size != mSize)
	{
		OmnAlarm << "Memory Pool size mismatch: "
			<< " Actual size: " << size
			<< " Expected size: " << mSize << enderr;

		mTotalAllocated++;
		mLock->unlock();
		char *data = OmnNew char[size+5];
		data[0] = 0;
		block.init(0, size, &data[1], &data[0]);
		return true;
	}

	//
	// If the idle list is not empty, get one from the list.
	//
	char *data;
	if (mHead)
	{
		//
		// It is not empty. 
		//
		Entry *ptr = mHead;
		mHead = mHead->mNext;
		mNumElem--;
		mLock->unlock();
		data = (char *)ptr;
	}
	else
	{
		//
		// It is empty. Allocate one
		//
		mTotalAllocated++;
		mLock->unlock();
		data = OmnNew char[size+5];
	}

	data[0] = 0;
	block.init(this, mSize, &data[1], &data[0]);
	return true;
}


void		
OmnMemPool::release(const uint size, char *ptr)
{
	if (!ptr)
	{
		OmnAlarm << "Null pointer passed in" << enderr;
		return;
	}
	
	ptr--;

	mLock->lock();
	if (size != mSize)
	{
		OmnAlarm << "Memory Pool size mismatch: "
			<< " Actual size: " << size
			<< " Expected size: " << mSize << enderr;

		mTotalAllocated--;
		mLock->unlock();
		cout << __FILE__ << ":" << __LINE__ << "To delete: " << (void*)ptr << endl;

		// Chen Ding, 05/10/2010
		// OmnDelete ptr;
		OmnDelete [] ptr;
		return;
	}

	if (mNumElem >= mMaxBlocks)
	{
		//
		// There are too many. Not inserted into the list.
		//
		mTotalAllocated--;
		mLock->unlock();

		// Chen Ding, 05/10/2010
		// OmnDelete ptr;
		OmnDelete [] ptr;
		return;
	}

	//
	// Add it to the list.
	//
	Entry *entry = (Entry *)ptr;
	entry->mNext = mHead;
	mHead = entry;
	mNumElem++;
	mLock->unlock();
	return;
}


uint
OmnMemPool::purge()
{
	//
	// This function may be called to release some memory, if any.
	// This may be due to the system failed to allocate memory.
	// If the idle list is not empty, it releases half of the blocks.
	// It reutrns the total number of bytes released. If no bytes 
	// are released, return 0.
	//
	if (mNumElem == 0)
	{
		return 0;
	}


	mLock->lock();
	uint blocksToRelease = mNumElem >> 1;		// Equivalent to mNumElem/2
	Entry *next;
	uint bytesRemoved = 0;
	for (uint i=0; i<blocksToRelease; i++)
	{
		if (!mHead)
		{
			//
			// This is a program error. Should never happen.
			//
			OmnAlarm << "Memory Pool: " << mSize
				<< " mHead is null" << enderr;
			mNumElem = 0;
			mLock->unlock();
			return bytesRemoved;
		}

		next = mHead->mNext;

		//OmnTrace << "To delete: " << mHead << endl;
		cout << __FILE__ << ":" << __LINE__ << "To delete: " << (void*)mHead << endl;

		// Chen Ding, 05/10/2010
		// OmnDelete mHead;
		OmnDelete [] mHead;
		mHead = next;
		mNumElem--;
		mTotalAllocated--;
		bytesRemoved += mSize;
	}

	mLock->unlock();
	return bytesRemoved;
}


void
OmnMemPool::checkMemory(OmnString &result)
{
	//
	// One may use this member function to check memory usage.
	// 
	mLock->lock();
	result << "\nSize: " << (int)mSize
		<< ". Total Blocks = " << (int)mTotalAllocated
		<< ". Idle Size = " << (int)mNumElem;
	mLock->unlock();
}


OmnString
OmnMemPool::getObjCnt() const
{
	OmnString str;
	str << "MemPool, Size=" << OmnStrUtil::itoa(mSize)
		<< ", Total Blocks: " << mTotalAllocated
		<< ", Idle: " << (int)mNumElem << "\n";
	return str;
}


char *
OmnMemPool::allocate()
{
    //
    // When needs a block from this memory pool, call this function to get
    // one. If there are idle blocks, it returns directly. Otherwise,
    // it news a block and returns it. It is important to return the
    // block when not needed. Otherwise, it is memory leak.

    // If the idle list is not empty, get one from the list.
    mLock->lock();
    if (mHead)
    {
        //
        // It is not empty.
        //
        Entry *ptr = mHead;
        mHead = mHead->mNext;
        mNumElem--;
        mLock->unlock();
		char *data = (char *)ptr;
		return ++data;
    }

    //
    // It is empty. Allocate one
    //
    mTotalAllocated++;
    mLock->unlock();
	char *data = OmnNew char[mSize+5];
	return ++data;
}

