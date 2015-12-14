////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: MemMgr.h
// Description:
//   This class provides memory management.   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_Util1_MemMgr_h
#define Omn_Util1_MemMgr_h

#include "alarm_c/alarm.h"
//#include "Config/ConfigEntity.h"
#include "SingletonClass/SingletonTplt.h"
#include "Util/ValList.h"
#include "Util/SPtr.h"
#include "Util/String.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util1/Ptrs.h"
#include "XmlUtil/Ptrs.h"



class AosMemPool;

OmnDefineSingletonClass(OmnMemMgrSingleton,
						OmnMemMgr,
						OmnMemMgrSelf,
                		OmnSingletonObjId::eMemMgr, 
						"MemMgr");


// A convenient type def
typedef void (*newHandler) ();

class OmnMemMgr
{
private:
	enum 
	{
		eMaxPoolIdx = 10,
		RELEASE_THRESHOLD = 10000	// The mininum number of bytes to release 
									// when calling memoryHandler(). 
	};

	newHandler		mPreviousHandler;

	AosMemPoolPtr	mPools[eMaxPoolIdx];
	AosMemPoolPtr	mPool32;
	AosMemPoolPtr	mPool64;
	AosMemPoolPtr	mPool128;
	AosMemPoolPtr	mPool256;
	AosMemPoolPtr	mPool512;
	AosMemPoolPtr	mOversizePool;

public:
	OmnMemMgr();
	virtual ~OmnMemMgr();

	// Singleton Interface
	static OmnMemMgr *	getSelf();
	virtual bool	start();
	virtual bool	stop();
	virtual bool	config(const AosXmlTagPtr &def);

	void		setMemoryHandler();
	void		restoreMemoryHandler();
	void		memoryHandler();
	static void	memoryHandlerStatic();
	void		checkMemory(OmnString &result);
	OmnString	getObjCnt();

	char *		allocate(int size, AosMemPoolPtr &pool, const char *file, const int line);
	char *		allocate(int size, const char *file, const int line);
	bool		release(char *memory, const char *file, const int line);

	static int64_t getMemoryUsed();

private:
	AosMemPoolPtr	getPool(const int size);
};

inline char *AosAllocateMem(const int size, const char *file, const int line)
{
	if (!OmnMemMgrSelf) OmnNew OmnMemMgrSingleton();
	return OmnMemMgrSelf->allocate(size, file, line);
}

inline char *AosAllocateMem(const int size, AosMemPoolPtr &pool, const char *file, const int line)
{
	if (!OmnMemMgrSelf) OmnNew OmnMemMgrSingleton();
	return OmnMemMgrSelf->allocate(size, pool, file, line);
}

inline bool AosCopyMemStr(char **toMem, const char *fromMem, const char *file, const int line)
{
	// It assumes both 'toMem' and 'fromMem' are allocated through
	// AosMemPool. 'fromMem' holds a string that is null terminated. 
	// It checks whether 'toMem' holds the same type of memory.
	// If not, it re-allocates the memory.
	
	int size2 = *(int*)&fromMem[-4];
	char *mm = *toMem;
	if (mm == 0)
	{
		// No memory allocated yet. 
		*toMem = OmnMemMgrSelf->allocate(size2+1, file, line);
		mm = *toMem;
		aos_assert_r(mm, false);
	}

	int size1 = *(int*)&mm[-4];
	if (size1 != size2)
	{
		OmnMemMgrSelf->release(*toMem, file, line);
		*toMem = OmnMemMgrSelf->allocate(size2+1, file, line);
		aos_assert_r(*toMem, false);
	}

	// Chen Ding, 2011/12/13
	// strcpy(*toMem, fromMem);
	strncpy(*toMem, fromMem, size2);
	(*toMem)[size2] = 0;
	return true;
}

inline bool AosCopyMemStr(
		char **toMem, 
		const char *fromMem, 
		const int size2,
		const char *file, 
		const int line)
{
	// Same as the previous one except that 'frommem' is not allocated
	// by memory pool.
	char *mm = *toMem;
	if(!mm)
	{
		*toMem = OmnMemMgrSelf->allocate(size2+1, file, line);
		aos_assert_r(*toMem, false);
		
		//felicia, 2013/05/20
		//strcpy(*toMem, fromMem);
		strncpy(*toMem, fromMem, size2);
		(*toMem)[size2] = 0;
		return true;
	}
	int size1 = *(int*)&mm[-4];
	if (size1 != size2)
	{
		OmnMemMgrSelf->release(*toMem, file, line);
		*toMem = OmnMemMgrSelf->allocate(size2+1, file, line);
		aos_assert_r(*toMem, false);
	}

	// Chen Ding, 2011/12/13
	// strcpy(*toMem, fromMem);
	strncpy(*toMem, fromMem, size2);
	(*toMem)[size2] = 0;
	return true;
}

#endif

