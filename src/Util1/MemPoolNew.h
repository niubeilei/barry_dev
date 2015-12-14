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
// 	05/10/2010	Copied from MemPool.h
////////////////////////////////////////////////////////////////////////////
#ifndef Omn_Util_MemPoolNew_h
#define Omn_Util_MemPoolNew_h

#include "aosUtil/Types.h"
#include "Thread/Ptrs.h"
#include "Util/BasicTypes.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"


class OmnMutex;
class OmnMemBlock;
class OmnString;

class AosMemPool : public OmnRCObject
{
	OmnDefineRCObject;

public:
	enum
	{
		eOversizedPool = -1,
		eMemoryFlagOffset = 8,
		eUserDataOffset = 12,
		eExtraSpace = 20,
		eMemoryFlag = 1234567890
	};

private:
	struct Entry
	{
		Entry *		mNext;
	};

	Entry *			mHead;
	const int		mSize;
	OmnMutexPtr		mLock;
	int				mNumElem;
	const int		mMaxBlocks;
	int				mTotalAllocated;

public:
	AosMemPool(const int size, const int maxBlocks = 500);
	~AosMemPool();

	char *		allocate(const int size, const char *file, const int line);
	bool		release(char *ptr, const char *file, const int line);
	int			purge();
	// void		checkMemory(OmnString &result);
	int			getSize() const {return mSize;}
	OmnString	getObjCnt() const;
	bool		checkMemory(const char *mem, const int size)
				{
					int *ss = (int*)&mem[-4];
					return size <= *ss;
				}

private:
	inline bool checkAndResetMemory(char *ptr)
	{
		// Check memory
		int *size = (int *)ptr;
		int *flag = (int *)&ptr[*size+eMemoryFlagOffset];
		if (*flag != eMemoryFlag)
		{
			std::cout << "<" << __FILE__ << ":" << __LINE__ << "> **********: "
				<< "Memory corrupted!" << std::endl;
			return false;
		}
		*flag = 0;
		return true;
	}

	inline void setMemoryFlag(const int size, char *data)
	{
		int *flag = (int*)&data[size+eMemoryFlagOffset];
		*flag = eMemoryFlag;
	}
};
#endif
