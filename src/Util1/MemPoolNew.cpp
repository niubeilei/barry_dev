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
//	05/10/2010	Copied from MemPool.cpp 
////////////////////////////////////////////////////////////////////////////
#include "Util1/MemPoolNew.h"

#include "Debug/Debug.h"
#include "Debug/Error.h"
#include "Alarm/Alarm.h"
#include "Thread/Mutex.h"
#include "Util/OmnNew.h"
#include "Util1/MemMgr.h"
#include "Util1/Ptrs.h"

// add iostream.h head file by xiaoqing 03-17-2006
// in order to resolve userland compile issue
#include <iostream>

const int sgAosMemoryFlag = 1234567890;

AosMemPool::AosMemPool(const int size, const int maxBlocks)
:
mHead(0),
mSize(size),
mLock(OmnNew OmnMutex()),
mNumElem(0),
mMaxBlocks(maxBlocks),
mTotalAllocated(0)
{
}


AosMemPool::~AosMemPool()
{
	//
	// Release all memory
	Entry *next = mHead;
	while (mHead)
	{
		next = mHead->mNext;
		OmnDelete [] mHead;
		mHead = next;
		mTotalAllocated--;
		mNumElem--;
	}	
}


bool
AosMemPool::release(char *ptr, const char *file, const int line)
{
	if (!ptr) return true;
	ptr -= 4;

	if (!checkAndResetMemory(ptr)) return false;

	if (mSize == eOversizedPool)
	{
		// This is an oversize memory pool. Simply delete the memory.
		OmnDelete [] ptr;
		return true;
	}

	mLock->lock();
	if (mNumElem >= mMaxBlocks)
	{
		// There are too many. Not inserted into the list.
		mTotalAllocated--;
		mLock->unlock();
		OmnDelete [] ptr;
		return true;
	}

	//
	// Add it to the list.
	//
	Entry *entry = (Entry *)ptr;
	entry->mNext = mHead;
	mHead = entry;
	mNumElem++;
	mLock->unlock();
	return true;
}


/*
void
AosMemPool::checkMemory(OmnString &result)
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
*/


OmnString
AosMemPool::getObjCnt() const
{
	OmnString str;
	str << "MemPool, Size=" << OmnStrUtil::itoa(mSize)
		<< ", Total Blocks: " << mTotalAllocated
		<< ", Idle: " << (int)mNumElem << "\n";
	return str;
}


char *
AosMemPool::allocate(const int size, const char *file, const int line)
{
    //
    // When needs a block from this memory pool, call this function to get
    // one. If there are idle blocks, it returns directly. Otherwise,
    // it news a block and returns it. It is important to return the
    // block when not needed. Otherwise, it is memory leak.
	//
	// IMPORTANT: the first four bytes are used to store the 
	// memory length. 

// OmnScreen << mSize << endl;
	if (size <= 0)
	{
		std::cout << "<" << __FILE__ << ":" << __LINE__ << "> **********: "
			<< "Size is invalid: " << size << std::endl;
		return 0;
	}

	int allocatedSize = -1;
	char *data = 0;
	if (mSize == eOversizedPool)
	{
		// This is an oversize memory pool. Simply new the memory
		data = OmnNew char[size + eExtraSpace];
		allocatedSize = size;
// OmnScreen << file << ":" << line << ":" << "size: " << (int)data << ":" << allocatedSize << ":" << size << endl;
	}
	else
	{
    	// If the idle list is not empty, get one from the list.
    	mLock->lock();
    	if (mHead)
    	{
        	// It is not empty.
        	Entry *ptr = mHead;
        	mHead = mHead->mNext;
        	mNumElem--;
        	mLock->unlock();
			data = (char *)ptr;
// OmnScreen << file << ":" << line << ":" << (int)data << endl;
    	}
		else
		{
    		// It is empty. Allocate one
    		mTotalAllocated++;
    		mLock->unlock();
			data = OmnNew char[mSize+eExtraSpace];
// OmnScreen << file << ":" << line << ":" << (int)data << ":" << mSize << endl;
		}
		allocatedSize = mSize;
// OmnScreen << allocatedSize << endl;
	}

	// Set the memory flag
// OmnScreen << allocatedSize << ":" << (int)data << endl;
	setMemoryFlag(allocatedSize, data);

	int *len = (int *)data;
	data += 4;
	*len = allocatedSize;

	return data;
}

