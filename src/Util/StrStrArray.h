////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// This type of IIL maintains a list of (string, docid) and is sorted
// based on the string value. 
//
// Modification History:
// 05/05/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_Util_StrStrArray_h
#define AOS_Util_StrStrArray_h

#include "Rundata/Ptrs.h"
#include "Thread/Ptrs.h"
#include "Thread/Mutex.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/String.h"
#include "Util/Orders.h"
#include "Util/Buff.h"
#include <string.h>
#include <vector>
using namespace std;


class AosStrStrArray : public OmnRCObject
{
	OmnDefineRCObject;

public:
	struct Entry
	{
		enum
		{
			eFieldLen = 80
		};

		char value1[eFieldLen];
		char value2[eFieldLen];
	};

private:
	OmnMutexPtr	mLock;
	AosOrder::E	mOrder;
	Entry *		mEntries;
	AosBuffPtr  mData;
	u32			mNumEntries;
	u32			mMemCap;
	bool		mIsDirty;
	int 		mCrtIdx;

public:
	AosStrStrArray(const AosOrder::E order = AosOrder::eAlphabetic)
	:
	mLock(OmnNew OmnMutex()),
	mOrder(order),
	mEntries(0),
	mData(0),
	mNumEntries(0),
	mMemCap(0),
	mIsDirty(false),
	mCrtIdx(0)
	{
	}

	AosStrStrArray(const AosBuffPtr &buff, const AosOrder::E order)
	:
	mLock(OmnNew OmnMutex()),
	mOrder(order),
	mEntries((Entry*)buff->data()),
	mData(buff),
	mNumEntries(buff->dataLen()/sizeof(Entry)),
	mMemCap(buff->dataLen()/sizeof(Entry)),
	mIsDirty(false),
	mCrtIdx(0)
	{
	}

	~AosStrStrArray()
	{
		mEntries = 0;
	}

	u32		getNumEntries() const {return mNumEntries;}
	u32		dataLength() const {return mNumEntries * sizeof(Entry);}
	bool	isDirty() const {return mIsDirty;}
	AosBuffPtr  getBuff()
			{
				mData->setDataLen(mNumEntries*sizeof(Entry));
				return mData;
			}
	Entry* first() {return mEntries;}
	Entry* last() {return mEntries+mNumEntries-1;}
	
	inline static int Comp(const void *p1, const void *p2)
	{
		Entry *pp1 = (Entry *) p1;
		Entry *pp2 = (Entry *) p2;

		int r = strcmp(pp1->value1, pp2->value1);
		if (r == 0)
		{
			r = strcmp(pp1->value2, pp2->value2);
		}
		return r;
	}

	bool addValue(const OmnString &value1, 
					const OmnString &value2,
					AosRundata *rdata)
	{
		// This function does the following:
		//  1. Retrieve the entry [name, value] by 'name'. 
		//  2. If the entry does not exist in the iil, add the entry [name, value]
		//  3. If the entry is found, add 'value' to the entry's value.
		// This function should always success. 
		//
		// After adding, if the entry has too many entries, create an IIL transaction
		// and sends it to the IILMgr to process it. It then clear some of the 
		// old entries from memory.
		mLock->lock();
		if (mNumEntries == 0 || mNumEntries >= mMemCap)
		{
			bool rslt = expandMemoryPriv();
			aos_assert_rl(rslt, mLock, false);
		}
		mIsDirty = true;
		
		//felicia, 2013/05/20
		//strcpy(mEntries[mNumEntries].value1, value1.data());
		//strcpy(mEntries[mNumEntries].value2, value2.data());
		aos_assert_r((int)strlen(mEntries[mNumEntries].value1) > value1.length(), false);
		aos_assert_r((int)strlen(mEntries[mNumEntries].value2) > value2.length(), false);
		strncpy(mEntries[mNumEntries].value1, value1.data(), value1.length());
		strncpy(mEntries[mNumEntries].value2, value2.data(), value2.length());
		mNumEntries++;
		mLock->unlock();
		return true;
	}

	bool sort()
	{
		mLock->lock();
		qsort(mEntries, mNumEntries, sizeof(Entry), Comp);
		mLock->unlock();
		return true;
	}

	void clear()
	{
		mLock->lock();
		mCrtIdx = 0;
		mNumEntries = 0;
		mMemCap = 0,
		mIsDirty = false;
		mEntries = 0;
		mLock->unlock();
	}

	void nextValue(OmnString &value1, OmnString &value2, bool &finished)
	{
		mLock->lock();
		finished = false;
		if (mCrtIdx < 0 || (u64)mCrtIdx >= mNumEntries)
		{
			finished = true;
			mLock->unlock();
			return ;
		}
	
		value1 = mEntries[mCrtIdx].value1;
		value2 = mEntries[mCrtIdx].value2;
		mCrtIdx++;
		mLock->unlock();
	}

private:
	inline static int search();

	bool expandMemoryPriv()
	{
		// This function expands the space for the in-memory IIL. 
		// Note that it does not check whether the list is too big to expand. 
		u32 newsize = mMemCap + mMemCap;
		if (newsize == 0) newsize = 1000;
		AosBuffPtr buff = OmnNew AosBuff(newsize*sizeof(Entry), 0 AosMemoryCheckerArgs);
		if (mNumEntries > 0)
		{
			aos_assert_r(mMemCap > 0, false);
			memcpy(buff->data(), mEntries, sizeof(Entry) * mMemCap);
		}

		mEntries = (Entry*)buff->data();
		mMemCap = newsize;
		mData = buff;
		return true;
	}
};

inline bool 
operator < (const AosStrStrArray::Entry &lhs, const AosStrStrArray::Entry &rhs)
{
	return AosStrStrArray::Comp((const void*)&lhs, (const void*)&rhs) < 0;
}

inline bool 
operator == (const AosStrStrArray::Entry &lhs, const AosStrStrArray::Entry &rhs)
{
	return AosStrStrArray::Comp((const void*)&lhs, (const void*)&rhs) == 0;
}

inline bool 
operator > (const AosStrStrArray::Entry &lhs, const AosStrStrArray::Entry &rhs)
{
	return AosStrStrArray::Comp((const void*)&lhs, (const void*)&rhs) > 0;
}

inline bool 
operator <= (const AosStrStrArray::Entry &lhs, const AosStrStrArray::Entry &rhs)
{
	return AosStrStrArray::Comp((const void*)&lhs, (const void*)&rhs) <= 0;
}

inline bool 
operator >= (const AosStrStrArray::Entry &lhs, const AosStrStrArray::Entry &rhs)
{
	return AosStrStrArray::Comp((const void*)&lhs, (const void*)&rhs) >= 0;
}

#endif

