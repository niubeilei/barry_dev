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
// 03/29/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_Util_StrU64Array_h
#define AOS_Util_StrU64Array_h

#include "Rundata/Ptrs.h"
#include "Thread/Ptrs.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/String.h"
#include "Util/Orders.h"
#include "Util/Buff.h"
#include <string.h>
#include <vector>
using namespace std;

struct AosIILStrEntry
{
	enum {eMaxStrLen = 80};
	char name[eMaxStrLen];
	u64 value;
};


class AosCompareFun1 : public OmnRCObject
{
	OmnDefineRCObject;
public:
	bool operator()(const AosIILStrEntry &lhs, const AosIILStrEntry &rhs)
	{
		return cmp(lhs, rhs) < 0;
	}
	int cmp(const AosIILStrEntry &lhs, const AosIILStrEntry &rhs)
	{
		int r = strcmp(lhs.name, rhs.name);
		if (r == 0)
		{
			if (lhs.value == rhs.value)
			{
				return 0;
			}
			else if(lhs.value > rhs.value)
			{
				return 1;
			}
			else
			{
				return -1;
			}
		}
		return r;
	}
};


class AosStrU64Array : public OmnRCObject
{
	OmnDefineRCObject;

private:
	enum
	{
		eInitSize = 10000
	};

	OmnMutexPtr	mLock;
	AosCompareFun1Ptr mComp;
	bool        mStable;
	AosIILStrEntry *mEntries;
	AosBuffPtr  mData;
	u32			mNumEntries;
	u32			mMemCap;
	bool		mIsDirty;
	int 		mCrtIdx;

public:
	AosStrU64Array(const bool stable = false);
	AosStrU64Array(const AosCompareFun1Ptr &comp, const bool stable);
	AosStrU64Array(const AosBuffPtr &buff,const AosCompareFun1Ptr &comp, const bool stable);
	~AosStrU64Array();

	bool	addValue(
				const char *name, 
				const int len,
				const u64 &value, 
				AosRundata *rdata);
	bool	addValue(
				const OmnString &name, 
				const u64 &value, 
				AosRundata *rdata);
	u32		getNumEntries() const {return mNumEntries;}
	u32		dataLength() const {return mNumEntries * sizeof(AosIILStrEntry);}
	void	clear(); 
	void 	nextValue(OmnString &name, u64 &value, bool &finished);
	bool    sort();
	bool	isDirty() const {return mIsDirty;}
	AosBuffPtr  getBuff()
			{
				mData->setDataLen(mNumEntries*sizeof(AosIILStrEntry));
				return mData;
			}
	AosIILStrEntry* first() {return mEntries;}
	AosIILStrEntry* last() {return mEntries+mNumEntries-1;}
	OmnString getValue(const int idx) const 
	{
		aos_assert_r(idx >= 0 && (u32)idx < mNumEntries, "");
		return mEntries[idx].name;
	}
	
	u64 getU64Value(const int idx) const
	{
		aos_assert_r(idx >= 0 && (u32)idx < mNumEntries, 0);
		return mEntries[idx].value;
	}

	void setNumEntries(const u32 nn) {mNumEntries = nn;}
	
private:
	bool expandMemoryPriv();
	inline static int search();
};

/*
inline bool 
operator < (const AosIILStrEntry &lhs, const AosIILStrEntry &rhs)
{
	return AosStrU64Array::Comp((const void*)&lhs, (const void*)&rhs) < 0;
}

inline bool 
operator == (const AosIILStrEntry &lhs, const AosIILStrEntry &rhs)
{
	return AosStrU64Array::Comp((const void*)&lhs, (const void*)&rhs) == 0;
}

inline bool 
operator > (const AosIILStrEntry &lhs, const AosIILStrEntry &rhs)
{
	return AosStrU64Array::Comp((const void*)&lhs, (const void*)&rhs) > 0;
}

inline bool 
operator <= (const AosIILStrEntry &lhs, const AosIILStrEntry &rhs)
{
	return AosStrU64Array::Comp((const void*)&lhs, (const void*)&rhs) <= 0;
}

inline bool 
operator >= (const AosIILStrEntry &lhs, const AosIILStrEntry &rhs)
{
	return AosStrU64Array::Comp((const void*)&lhs, (const void*)&rhs) >= 0;
}
*/
#endif

