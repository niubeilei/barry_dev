////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: MemBlock.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_Util_MemBlock_h
#define Omn_Util_MemBlock_h

#include "Util/BasicTypes.h"


class OmnMemPool;

class OmnMemBlock 
{
private:
	OmnMemPool *	mMemPool;
	int			mSize;
	char *			mRef;	

public:
	char *			mData;
	static int 		mObjCnt;


public:
	OmnMemBlock()
	:
	mMemPool(0),
	mSize(0),
	mRef(0),
	mData(0)
	{
		mObjCnt++;
	}

	OmnMemBlock(const OmnMemBlock &rhs)
	:
	mMemPool(rhs.mMemPool),
	mSize(rhs.mSize),
	mRef(rhs.mRef),
	mData(rhs.mData)
	{
		mObjCnt++;

		if (mRef)
		{
			(*mRef)++;
		}
	}

	OmnMemBlock & operator = (const OmnMemBlock &rhs)
	{
		if (this == &rhs)
		{
			return *this;
		}

		if (mData == rhs.mData)
		{
			//
			// This means the two point to the same memory.
			// Do not need to do anything.
			//
			return *this;
		}

		//
		// Otherwise, release the buffer and take the other
		//
		removeRef();
		mMemPool = rhs.mMemPool;
		mData = rhs.mData;
		mSize = rhs.mSize;
		mRef = rhs.mRef;

		if (mRef)
		{
			(*mRef)++;
		}

		return *this;
	}

	bool clone(OmnMemBlock &block) const;

	
	~OmnMemBlock()
	{
		mObjCnt--;

		removeRef();
	}		

	void init(OmnMemPool *pool, const int size, char *data, char *ref) 
		 {
			mMemPool = pool;
			mSize = size;
			mData = data;
			mRef = ref;
			if (mRef)
			{
				(*mRef)++;
			}
		 }

	char *		data() const {return mData;}
	int		size() const {return mSize;}
	char 		getRef() const {return *mRef;}
	void		returnMemory() {removeRef();}

private:
	void		removeRef();
};

#endif
