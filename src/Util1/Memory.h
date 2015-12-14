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
// 	05/11/2010	Copied from MemBlock.h
////////////////////////////////////////////////////////////////////////////
#ifndef Omn_Util_Memory_h
#define Omn_Util_Memory_h

#include "Util/BasicTypes.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util1/MemPoolNew.h"


class OmnMemPool;

class AosMemory : public OmnRCObject
{
	OmnDefineRCObject;

private:
	AosMemPool *	mMemPool;
	int				mSize;
	char *			mData;

	AosMemory(const AosMemory &rhs);
	AosMemory & operator = (const AosMemory &rhs);

public:
	AosMemory()
	:
	mMemPool(0),
	mSize(0),
	mData(0)
	{
	}

	~AosMemory()
	{
		if (mMemPool)
		{
			mMemPool->release(mData, __FILE__, __LINE__);
		}
		mMemPool = 0;
		mData = 0;
		mSize = 0;
	}		

	bool clone(AosMemory &block) const;
	void init(AosMemPool *pool, const int size, char *data) 
		 {
			mMemPool = pool;
			mSize = size;
			mData = data;
		 }

	char *	data() const {return mData;}
	int 	size() const {return mSize;}
};

#endif
