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
// 2014/05/13/ Created by Ketty 
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_StatUtil_StatCompareFunc_h
#define AOS_StatUtil_StatCompareFunc_h

#include "Rundata/Ptrs.h"
#include "Thread/Ptrs.h"
#include "Util/RCObject.h"
#include "Util/CompareFun.h"


class AosStatCompareFunc: public AosCompareFun
{
	
private:
	u32		mFieldPos;
	bool	mIsKeyField;
	AosDataType::E mFieldDataType;
	vector<OmnString>&	mAllKeyValues;

public:
	AosStatCompareFunc(
			const u32 entry_size,
			const bool reverse,
			const u32 field_pos,
			const bool is_key_field,
			const AosDataType::E field_data_tp,
			vector<OmnString> &all_key_values)
	:
	AosCompareFun(entry_size, reverse),
	mFieldPos(field_pos),
	mIsKeyField(is_key_field),
	mFieldDataType(field_data_tp),
	mAllKeyValues(all_key_values)
	{
	};

	virtual FunType getFuncType() const { return eStatComp; };
	virtual bool isSame(const AosCompareFunPtr &rhs) const
	{
		aos_assert_r(rhs, false);
		return rhs->getFuncType() == eStatComp;
	}
	
	virtual AosCompareFunPtr clone()
	{
		return OmnNew AosStatCompareFunc(size, mReverse,
				mFieldPos, mIsKeyField, mFieldDataType, mAllKeyValues);
	}

private:
	virtual int cmpPriv(const char* lhs, const char* rhs)
	{
		if(mIsKeyField)		return compareKeyField(lhs, rhs);
	
		switch(mFieldDataType)
		{
		case AosDataType::eU32:
		{
			u32 l_value = *(u32 *)(lhs + mFieldPos);
			u32 r_value = *(u32 *)(rhs + mFieldPos);
			if(l_value < r_value)	return -1;
			if(l_value == r_value)	return 0;
			return 1;
		}
		
		case AosDataType::eU64:
		{
			u64 l_value = *(u64 *)(lhs + mFieldPos);
			u64 r_value = *(u64 *)(rhs + mFieldPos);
			if(l_value < r_value)	return -1;
			if(l_value == r_value)	return 0;
			return 1;
		}

		case AosDataType::eInt64:
		{
			int64_t l_value = *(int64_t *)(lhs + mFieldPos);
			int64_t r_value = *(int64_t *)(rhs + mFieldPos);
			if(l_value < r_value)	return -1;
			if(l_value == r_value)	return 0;
			return 1;
		}
		
		default:
			OmnNotImplementedYet;
			break;
		}
		
		return -1;
	}
	
	int	 compareKeyField(const char* lhs, const char* rhs)
	{
		u32 l_idx = *(u32*)(lhs + mFieldPos);
		u32 r_idx = *(u32*)(rhs + mFieldPos);
		
		if (mAllKeyValues[l_idx] < mAllKeyValues[r_idx]) return -1;
		if (mAllKeyValues[l_idx] == mAllKeyValues[r_idx]) return 0;
		return 1;
	}

};


#endif

