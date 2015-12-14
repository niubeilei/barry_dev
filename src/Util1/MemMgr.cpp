////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: MemMgr.cpp
// Description:
//	This class provides memory management. As soon as an application
//  starts, it calls setMemoryHandler() to change the default memory
//  handler to the one defined in this class. From then on, if a 
//  new failed, it will call memoryHandler() member function. This 
//  function will call all managed objects to release some memory. 
//  Hopefully, this resolves memory problem.
//
//  Singleton Dependency:
//  	OmnAlarmMgr   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#include "Util1/MemMgr.h"

#include "Alarm/Alarm.h"
#include "Debug/Error.h"
#include "Debug/Debug.h"
#include "SingletonClass/SingletonImpl.cpp"
#include "Thread/Mutex.h"
#include "Util1/MemPoolNew.h"
#include "Util1/Ptrs.h"
#include "XmlUtil/XmlTag.h"
#include <new>


#include <stdio.h>
#include <new>

using namespace std;

OmnSingletonImpl(OmnMemMgrSingleton,
                 OmnMemMgr,
                 OmnMemMgrSelf,
                "OmnMemMgr");


static OmnMutex		sgLock;
static int64_t		sgMemoryUsed = 0;

OmnMemMgr::OmnMemMgr()
:
mPreviousHandler(0)
{
	mPool32 		= OmnNew AosMemPool(32,2000000);
	mPool64 		= OmnNew AosMemPool(64,2000000);
	mPool128 		= OmnNew AosMemPool(128,500000);
	mPool256 		= OmnNew AosMemPool(256,100000);
	mPool512 		= OmnNew AosMemPool(512,100000);
	mOversizePool 	= OmnNew AosMemPool(AosMemPool::eOversizedPool);

	u32 ss = 1023;
	for (int i=0; i<eMaxPoolIdx; i++)
	{
		mPools[i] = OmnNew AosMemPool(ss);
		ss += 1024;
	}
}


OmnMemMgr::~OmnMemMgr()
{
	OmnTrace << "Deleting OmnMemMgr" << endl;
}

bool
OmnMemMgr::start()
{
	return true;
}


bool
OmnMemMgr::stop()
{
	return true;
}


bool
OmnMemMgr::config(const AosXmlTagPtr &def)
{
	if (!def)
	{
		return true;
	}

	return true;
}


void		
OmnMemMgr::setMemoryHandler()
{
	//
	// It sets the memory handler: memoryHandler() and stores the
	// previous handler in mPreviousHandler. 
	//
	sgLock.lock();
	if (mPreviousHandler != 0)
	{
		//
		// This means the function has been called previously. In this
		// current implementation, this is not allowed.
		//
		OmnAlarm << "Try to set memory handler, but someone has already set it"
			<< enderr;
		sgLock.unlock();
		return;
	}

	//
	// Then set the memory handler. Store the existing one in mPreviousHandler
	//
	mPreviousHandler = set_new_handler(memoryHandlerStatic);
	sgLock.unlock();
}


void		
OmnMemMgr::restoreMemoryHandler()
{
	//
	// It resets the memory handler. Before calling this function, the
	// setMemoryHandler() should have called. Otherwise, it is an error.
	//
	sgLock.lock();
	if (mPreviousHandler == 0)
	{
		//
		// The setMemoryHandler() was not called. This is an error.
		//
		OmnAlarm << "Try to set memory handler, but someone has already set it"
			<< enderr;
		sgLock.unlock();
		return;
	}

	//
	// Otherwise, retore the handler.
	//
	set_new_handler(mPreviousHandler);
	sgLock.unlock();
}


void		
OmnMemMgr::memoryHandlerStatic()
{
	if (!OmnMemMgrSelf)
	{
		return;
	}

	OmnMemMgrSelf->memoryHandler();
}


void
OmnMemMgr::memoryHandler()
{
	//
	// This is the member function to be called when memory request
	// failed. This function checks whether there is any managed
	// object. If yes, it asks everyone to release some memory. 
	// If some memory is released, it returns. Otherwise, it means
	// it could not release any memory. It will abort the execution.
	//
	OmnTrace << "Memory is running low. Call memory handler to release"
		<< " some memory." << endl;

	/*
	sgLock.lock();
	uint sizeReleased = 0;
	while (sgMemPools.hasMore())
	{
		sizeReleased += (sgMemPools.crtValue())->purge();
		if (sizeReleased >= RELEASE_THRESHOLD)
		{
			//
			// It has released enough memory. Return.
			//
			OmnTrace << "Total memory released: " << sizeReleased << endl;
			sgLock.unlock();
			return;
		}

		sgMemPools.next();
	}

	sgLock.unlock();

	//
	// Check whether it has ever released something.
	//
	if (sizeReleased == 0)
	{
		//
		// It did not. This means that application can't release anything.
		//
		OmnAlarm << "Run out of memory" << enderr;
		abort();
	}

	//
	// It did released something, though not as much as we wanted.
	//
	return;
	*/
}


OmnString
OmnMemMgr::getObjCnt() 
{
	OmnString str;

	/*
	sgLock.lock();
	sgMemPools.reset();
	while (sgMemPools.hasMore())
	{
		str << (sgMemPools.crtValue())->getObjCnt() << "\n";
		sgMemPools.next();
	}
	sgLock.unlock();

	str << OmnMemPoolMul::getObjCnt();
	*/
	return str;
}


char *
OmnMemMgr::allocate(int size, AosMemPoolPtr &pool, const char *file, const int line)
{
	pool = getPool(size);
	if (!pool) return 0;
	return pool->allocate(size, file, line);
}


AosMemPoolPtr
OmnMemMgr::getPool(const int size)
{
	// This function allocates the requested memory. If successful, 
	// it returns the memory and the pool.
	// 
	// Memory is managed based on size:
	// 	32
	// 	64
	// 	128
	// 	256
	// 	512
	// 	1023
	// 	...
	//	10000+
	if (size <= 0)
	{
		std::cout << "<" << __FILE__ << ":" << __LINE__
			<< "> ********** size is 0!" << std::endl;
		return 0;
	}
	if (size <= 32)  return mPool32;
	if (size <= 64)  return mPool64;
	if (size <= 128) return mPool128;
	if (size <= 256) return mPool256;
	if (size <= 512) return mPool512;

	// Then divide the size by 1024
	int idx = size;
	idx >>= 10;
	if (idx < eMaxPoolIdx) return mPools[idx];
	return mOversizePool;
}


char *		
OmnMemMgr::allocate(int size, const char *file, const int line)
{
	char* newmem = new char[size + 10];
	*((int *)newmem) = size;
	return (newmem+4);
//	AosMemPoolPtr pool = getPool(size);
//	if (!pool) return 0;
//	return pool->allocate(size, file, line);
}


bool
OmnMemMgr::release(char *ptr, const char *file, const int line)
{
	if (!ptr) return true;
	int *size = (int *)&ptr[-4];
// OmnScreen << file << ":" << line << ":" << *size << endl;
	if (size <= 0)
	{
		std::cout << "<" << __FILE__ << ":" << __LINE__
			<< "> ********** size is 0!" << std::endl;
		return false;
	}
	
	delete[] (ptr-4);
	return true;
/*
	AosMemPoolPtr pool = getPool(*size);
	if (!pool)
	{
		std::cout << "<" << __FILE__ << ":" << __LINE__
			<< "> ********** Failed to get pool!" << std::endl;
		return false;
	}

//	if (pool->getSize() != *size)
//	{
//		OmnMark;
//		pool = getPool(*size);
//	}
	return pool->release(ptr, file, line);
*/
}


int64_t
OmnMemMgr::getMemoryUsed()
{
	sgLock.lock();
	int64_t nn = sgMemoryUsed;
	sgLock.unlock();
	return nn;
}

