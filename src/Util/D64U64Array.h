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
#ifndef AOS_Util_U64U64Array_h
#define AOS_Util_U64U64Array_h

#include "Rundata/Ptrs.h"
#include "Thread/Ptrs.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/Buff.h"
#include <vector>
using namespace std;

struct AosIILU64Entry
{
	u64 name;
	u64 value;
};


class AosU64U64Array : public OmnRCObject
{
	OmnDefineRCObject;

private:

	OmnMutexPtr	mLock;
	AosIILU64Entry *mEntries;
	AosBuffPtr  mData;
	u32			mNumEntries;
	u32			mMemCap;
	bool		mIsDirty;
	int 		mCrtIdx;

public:
	AosU64U64Array();
	AosU64U64Array(const AosBuffPtr &buff);
	~AosU64U64Array();

	bool	addValue(
				const u64 &name, 
				const u64 &value, 
				AosRundata *rdata);
	u32		getNumEntries() const {return mNumEntries;}
	u32		dataLength() const {return mNumEntries * sizeof(AosIILU64Entry);}
	void	clear(); 
	void 	nextValue(u64 &name, u64 &value, bool &finished);
	bool    sort();
	bool	isDirty() const {return mIsDirty;}
	AosBuffPtr  getBuff()
			{
				mData->setDataLen(mNumEntries*sizeof(AosIILU64Entry));
				return mData;
			}
	AosIILU64Entry* first() {return mEntries;}
	AosIILU64Entry* last() {return mEntries+mNumEntries-1;}
	
	inline static int	Comp(const void *p1, const void *p2)
	{
		AosIILU64Entry *pp1 = (AosIILU64Entry *) p1;
		AosIILU64Entry *pp2 = (AosIILU64Entry *) p2;

		if(pp1->value > pp2->value) return 1;
		if(pp1->value < pp2->value) return -1;
		if(pp1->value > pp2->value) return 1;
		if(pp1->value < pp2->value) return -1;
		return 0;
	}

private:
	bool expandMemoryPriv();
	
	inline static int search();
};

inline bool 
operator < (const AosIILU64Entry &lhs, const AosIILU64Entry &rhs)
{
	return AosU64U64Array::Comp((const void*)&lhs, (const void*)&rhs) < 0;
}

inline bool 
operator == (const AosIILU64Entry &lhs, const AosIILU64Entry &rhs)
{
	return AosU64U64Array::Comp((const void*)&lhs, (const void*)&rhs) == 0;
}

inline bool 
operator > (const AosIILU64Entry &lhs, const AosIILU64Entry &rhs)
{
	return AosU64U64Array::Comp((const void*)&lhs, (const void*)&rhs) > 0;
}

inline bool 
operator <= (const AosIILU64Entry &lhs, const AosIILU64Entry &rhs)
{
	return AosU64U64Array::Comp((const void*)&lhs, (const void*)&rhs) <= 0;
}

inline bool 
operator >= (const AosIILU64Entry &lhs, const AosIILU64Entry &rhs)
{
	return AosU64U64Array::Comp((const void*)&lhs, (const void*)&rhs) >= 0;
}

#endif

