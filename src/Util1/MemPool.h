////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: MemPool.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_Util_MemPool_h
#define Omn_Util_MemPool_h

#include "Thread/Ptrs.h"
#include "Util/BasicTypes.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"


class OmnMutex;
class OmnMemBlock;
class OmnString;

class OmnMemPool : public OmnRCObject
{
	OmnDefineRCObject;

private:
	struct Entry
	{
		Entry *		mNext;
	};

	Entry *				mHead;
	const uint			mSize;
	OmnMutexPtr			mLock;
	uint				mNumElem;
	const uint			mMaxBlocks;
	int					mTotalAllocated;

public:
	OmnMemPool(const uint size, 
		const uint maxBlocks = 500);
	~OmnMemPool();

	char *		allocate();
	char *		allocate(const char *file, const int line, const uint size);
	bool		allocate(const char *file, const int line, 
					OmnMemBlock &block, const uint size);
	void		release(const uint size, char *ptr);
	uint		purge();
	void		checkMemory(OmnString &result);
	OmnString	getObjCnt() const;
};
#endif
