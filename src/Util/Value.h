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
// 12/18/2011: Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#if 0
#ifndef Aos_Util_Value_h
#define Aos_Util_Value_h

#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "XmlUtil/Ptrs.h"

#include <vector>
using namespace std;

class AosValue
{
public:
	enum
	{
		eMaxIntegers = 1000000,				// 1M
		eMaxStrings  = 100000,
		eMaxXmls 	 = 100000
	};

private:
	int						mMaxIntegers;
	int						mMaxStrings;
	int						mMaxXmls;

	vector<int64_t> 		mInt64Values;
	vector<u64> 			mU64Values;
	vector<OmnString>		mStrings;
	vector<AosXmlTagPtr>	mXmls;

public:
	AosValue()
	:
	mMaxIntegers(eMaxIntegers),
	mMaxStrings(eMaxStrings),
	mMaxXmls(eMaxXmls)
	{
	}

	AosValue(const int max_integers, 
			const int max_strings,
			const int max_xmls)
	:
	mMaxIntegers(max_integers),
	mMaxStrings(max_strings),
	mMaxXmls(max_xmls)
	{
		if (max_integers < 0) mMaxIntegers = eMaxIntegers;
		if (max_strings < 0) mMaxStrings = eMaxStrings;
		if (max_xmls < 0) mMaxXmls = eMaxXmls;
	}

	~AosValue() {}

	bool setInt64(const int idx, const int64_t &value)
	{
		aos_assert_r(idx >= 0 && idx < mMaxIntegers, false);
		if ((u32)idx < mInt64Values.size()) 
		{
			mInt64Values.resize(idx);
		}
		mInt64Values[idx] = value;
		return true;
	}

	int64_t getInt64(const int idx, const int64_t &dft)
	{
		aos_assert_r(idx >= 0 && (u32)idx < mInt64Values.size(), dft);
		return mInt64Values[idx];
	}

	bool setU64(const int idx, const u64 &value)
	{
		aos_assert_r(idx >= 0 && idx < mMaxIntegers, false);
		if ((u32)idx < mU64Values.size()) 
		{
			mU64Values.resize(idx);
		}
		mU64Values[idx] = value;
		return true;
	}

	u64 getU64(const int idx, const u64 &dft)
	{
		aos_assert_r(idx >= 0 && (u32)idx < mU64Values.size(), dft);
		return mU64Values[idx];
	}

	bool setString(const int idx, const OmnString &value)
	{
		aos_assert_r(idx >= 0 && idx < mMaxStrings, false);
		if ((u32)idx < mStrings.size()) 
		{
			mStrings.resize(idx);
		}
		mStrings[idx] = value;
		return true;
	}

	OmnString getString(const int idx, const OmnString &dft)
	{
		aos_assert_r(idx >= 0 && (u32)idx < mStrings.size(), dft);
		return mStrings[idx];
	}

	bool setXml(const int idx, const AosXmlTagPtr &value)
	{
		aos_assert_r(idx >= 0 && idx < mMaxXmls, false);
		if ((u32)idx < mXmls.size()) 
		{
			mXmls.resize(idx);
		}
		mXmls[idx] = value;
		return true;
	}

	AosXmlTagPtr getXml(const int idx)
	{
		aos_assert_r(idx >= 0 && (u32)idx < mXmls.size(), 0);
		return mXmls[idx];
	}
};
#endif
#endif

