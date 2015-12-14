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
// Modification History:
// 2013/09/08	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "QueryTorturer/QueryFieldStr.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "API/AosApi.h"
#include "QueryTorturer/SmartQueryTester.h"
#include "QueryTorturer/SmartStatisticsTester.h"
#include "QueryTorturer/QueryTesterUtil.h"
#include "Random/RandomUtil.h"
#include "Tester/TestMgr.h"
#include "Util/File.h"

//static bool sgShowLog = false;
const int sgNumDividers = 20;
static u64 sgDividers[sgNumDividers] = 
{
	1, 
	10,
	100,
	1000,
	10000,
	100000,
	1000000,
	10000000ULL,
	100000000ULL,
	1000000000ULL,
	10000000000ULL,
	100000000000ULL,
	1000000000000ULL,
	10000000000000ULL,
	100000000000000ULL,
	1000000000000000ULL,
	10000000000000000ULL,
	100000000000000000ULL,
	1000000000000000000ULL,
	10000000000000000000ULL
};

AosQueryFieldStr::AosQueryFieldStr(
		const int field_idx, 
		const bool is_time_field)
:
AosQueryField(field_idx, is_time_field)
{
	if (!init())
	{
		OmnAlarm << "Failed creating field" << enderr;
		OmnThrowException("internal error");
		return;
	}
}


AosQueryFieldStr::~AosQueryFieldStr()
{
}


bool
AosQueryFieldStr::init()
{
	AosQueryField::init();

	mIsNumAlpha = determineNumAlpha();
	mMinValue = 0x99999;
	mMaxValue = 0;

	return true;
}


bool 
AosQueryFieldStr::determineNumAlpha()
{
	return false;
}

int
AosQueryFieldStr::compareTwoNumbers(u64 value1, u64 value2)
{
	// It compares two numbers and returns:
	// 		-1 if (value1 < value2)
	// 		0  if (value1 == value2)
	// 		1  if (value1 > value2)
	int nn1 = getNumDigits(value1);
	int nn2 = getNumDigits(value2);

	if (nn1 < nn2)
	{
		if (mIsNumAlpha) return -1;
		aos_assert_r(nn2 - nn1 < sgNumDividers, false);
		value2 = value2/sgDividers[nn2-nn1];
	}
	else if (nn1 > nn2)
	{
		if (mIsNumAlpha) return 1;
		aos_assert_r(nn1 - nn2 < sgNumDividers, false);
		value1 = value1/sgDividers[nn1-nn2];
	}

	if (value1 < value2) return -1;
	if (value1 == value2) return 0;
	return 1;
}


bool 
AosQueryFieldStr::setMinMax(const u64 docid)
{
	if (mGroupSeedSize > 0 && docid < eMaxKnownDocid)
	{
		mKnownDocids.push_back(docid);
	}

	u64 value = docid2U64Value(docid);
	if (!mIsNumAlpha)
	{
		// It is not numerical alphabetic. The comparison of two numbers
		// is very different from that of two strings. 
		if (compareTwoNumbers(value, mMinValue) < 0)
		{
			mMinValue = value;
			mMinDocid = docid;
		}

		if (compareTwoNumbers(mMaxValue, value) < 0)
		{
			mMaxValue = value;
			mMaxDocid = docid;
		}

		// OmnScreen << "Field: " << mFieldIdx << " [" << mMinValue << ", " << mMaxValue << "]" << endl;
		return true;
	}
	
	// It is numerical alphabetic. The string order is the same as the 
	// numerical order.
	if (getNumDigits(value) < getNumDigits(mMinValue) || value < mMinValue)
	{
		mMinValue = value;
	}

	if (getNumDigits(value) > getNumDigits(mMinValue) || value > mMaxValue)
	{
		mMaxValue = value;
	}
	return true;
}


bool 
AosQueryFieldStr::setRandomMinMax()
{
	u64 value1 = rand();
	u64 value2 = rand();
	if (compareTwoNumbers(value1, value2) < 0)
	{
		mMinValue = value1;
		mMaxValue = value2;
	}
	else
	{
		mMinValue = value2;
		mMaxValue = value1;
	}
	return true;
}


OmnString 
AosQueryFieldStr::getFieldIILName()
{
	OmnString iilname = "_zt44_str_index_";
	iilname << mFieldIdx;
	return iilname;
}


bool 
AosQueryFieldStr::createCond(AosQueryTesterUtil::CondDef &cond)
{
	AosQueryField::createCond(cond);
	cond.is_str_field = true;
	return true;
}


bool 
AosQueryFieldStr::createAnyCond(AosQueryTesterUtil::CondDef &cond)
{
	AosQueryField::createAnyCond(cond);
	cond.is_str_field = true;
	return true;
}


bool
AosQueryFieldStr::isInRange(const u64 docid, u64 &value)
{
	// This function checks whether the docid 'docid' is in 
	// the field's range.
	value = docid2U64Value(docid);
	switch (mOpr)
	{
	case eAosOpr_gt:
		 return (compareTwoNumbers(value, mMinValue) > 0);
		 
	case eAosOpr_ge:
		 return (compareTwoNumbers(value, mMinValue) >= 0);

	case eAosOpr_eq:
		 return (compareTwoNumbers(value, mMinValue) == 0);

	case eAosOpr_lt:
		 return (compareTwoNumbers(value, mMaxValue) < 0);

	case eAosOpr_le:
		 return (compareTwoNumbers(value, mMaxValue) <= 0);

	case eAosOpr_an:
		 return true;

	case eAosOpr_range_ge_le:
		 if (compareTwoNumbers(value, mMinValue) >= 0 &&
			 compareTwoNumbers(value, mMaxValue) <= 0) return true;
		 return false;

	case eAosOpr_range_ge_lt:
		 if (compareTwoNumbers(value, mMinValue) >= 0 &&
			 compareTwoNumbers(value, mMaxValue) < 0) return true;
		 return false;

	case eAosOpr_range_gt_le:
		 if (compareTwoNumbers(value, mMinValue) > 0 &&
			 compareTwoNumbers(value, mMaxValue) <= 0) return true;
		 return false;

	case eAosOpr_range_gt_lt:
		 if (compareTwoNumbers(value, mMinValue) > 0 &&
			 compareTwoNumbers(value, mMaxValue) < 0) return true;
		 return false;

	case eAosOpr_ne:
	case eAosOpr_Objid:
	case eAosOpr_prefix:
	case eAosOpr_like:
	case eAosOpr_date:
	case eAosOpr_epoch:
		 OmnAlarm << "Not supported yet" << enderr;
		 return false;

	default:
		 break;
	}

	OmnAlarm << "Unrecognized opr: " << mOpr << enderr;
	return false;
}


AosQueryFieldPtr
AosQueryFieldStr::clone()
{
	return OmnNew AosQueryFieldStr(*this);
}


bool 
AosQueryFieldStr::initField(const FieldInfo &info)
{
	AosQueryField::initField(info);

	mIsNumAlpha = info.mIsNumAlpha;
	mStartDocid = info.mStartDocid;
	return true;
}


AosQueryField::FieldInfo
AosQueryFieldStr::getFieldInfo()
{
	FieldInfo field_info = AosQueryField::getFieldInfo();
	field_info.mIsNumAlpha = mIsNumAlpha;
	// field_info.mSeeds = mSeeds;
	field_info.mStartDocid = mStartDocid;
	return field_info;
}


bool
AosQueryFieldStr::serializeFromBuff(const AosBuffPtr &buff)
{
	// It assumes the field type has been read.
	bool rslt = AosQueryField::serializeFromBuff(buff);
	aos_assert_r(rslt, false);
	return true;
}


bool
AosQueryFieldStr::serializeToBuff(const AosBuffPtr &buff)
{
	// 	FieldType(1)		int
	// 	(refer to parent)
	buff->setInt(1);
	bool rslt = AosQueryField::serializeToBuff(buff);
	aos_assert_r(rslt, false);
	return true;
}


OmnString 
AosQueryFieldStr::getMinValue() const
{
	// Chen Ding, 2013/10/10
	// u64 value = getFieldValue(mMinValue);
	// OmnString ss;
	// ss << value;
	OmnString ss;
	ss << mMinValue;
	return ss;
}


OmnString 
AosQueryFieldStr::getMaxValue() const
{
	// Chen Ding, 2013/10/10
	// u64 value = getFieldValue(mMaxValue);
	OmnString ss;
	ss << mMaxValue;
	return ss;
}

OmnString
AosQueryFieldStr::getFieldname() const
{
	OmnString ss;
	//ss << "str_field_" << mFieldIdx;
	ss << "k" << mFieldIdx;
	return ss;
}
