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
#include "QueryTorturer/QueryField.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "API/AosApi.h"
#include "QueryTorturer/SmartQueryTester.h"
#include "QueryTorturer/SmartStatisticsTester.h"
#include "QueryTorturer/QueryTesterUtil.h"
#include "QueryTorturer/QueryFieldStr.h"
#include "QueryTorturer/Ptrs.h"
#include "Porting/Sleep.h"
#include "Random/RandomUtil.h"
#include "SEInterfaces/TaskObj.h"
#include "SEInterfaces/ActionObj.h"
#include "SEInterfaces/IndexMgrObj.h"
#include "Tester/TestMgr.h"
#include "Thread/Mutex.h"
#include "Util/Opr.h"
#include "Util/File.h"
#include "Util/CompareFun.h"
#include "Util/BuffArray.h"


vector<AosQueryField::SwapEntry> AosQueryField::smFieldSwaps;
AosSmartQueryTester * AosQueryField::smTester1;
AosSmartStatisticsTester * AosQueryField::smTester;

static bool sgInited = false;
static OmnMutex sgLock;

hash_map<const OmnString, OmnFilePtr, Omn_Str_hash, compare_str> sgFileMap;
typedef hash_map<const OmnString, OmnFilePtr, Omn_Str_hash, compare_str>::iterator query_ritr_t;

extern bool gAosSmartQueryTesterUseParal;

AosQueryField::AosQueryField(
		const int field_idx, 
		const bool is_time_field)
:
mLock(OmnNew OmnMutex()),
mFieldIdx(field_idx),
mFieldName(""),
mMinValue(0xffffffffffffffff),
mMaxValue(),
mMinDocid(0xffffffffffffffff),
mMaxDocid(0),
mIsSortingField(false),
mOpr(eAosOpr_Invalid),
mIsNumAlpha(false),
mStartDocid(0),
mGroupSeedSize(0),
mIsTimeField(is_time_field),
mCondValue1(0),
mCondValue2(0)
{
}


AosQueryField::~AosQueryField()
{
}

void
AosQueryField::setTester(AosSmartQueryTester *tester)
{
	smTester1 = tester;
}

void
AosQueryField::setTester(AosSmartStatisticsTester *tester)
{
	smTester = tester;
}

bool
AosQueryField::init()
{
	if (!sgInited)
	{
		sgLock.lock();
		bool rslt = true;
		if (!sgInited)
		{
			rslt = initStatic();			
			sgInited = true;
		}
		sgLock.unlock();
		aos_assert_r(rslt, false);
	}

	aos_assert_r(smTester, false);
	mStartDocid = smTester->getStartDocid();

	// Determine whether it is a grouped field.
	if (rand() % 100 < 30)
	{
		// It is a grouped field.
		int num_seeds = OmnRandom::intByRange(
							2, 10, 30,
							11, 30, 200,
							31, 100, 30,
							101, 1000, 10);
		mGroupSeedSize = 0;
		mGroupSeeds.clear();
		for (int i=0; i<num_seeds; i++)
		{
			u32 seed = (rand() % 1000) + 1000;	
			mGroupSeeds.push_back(seed);
			mGroupSeedSize += seed;
		}
	}
	return true;
}


bool
AosQueryField::initStatic()
{
	bool rslt = createFieldSwappers();
	aos_assert_r(rslt, false);
	return true;
}


bool
AosQueryField::createFieldSwappers()
{
	smFieldSwaps.resize(12);
	switch (smTester->getFieldValueMethod())
	{
	case 2:
		 smFieldSwaps[0] = SwapEntry(2, 14, 3, 12, 7, 10);
		 smFieldSwaps[1] = SwapEntry(3, 14, 4, 12, 6, 10);
		 smFieldSwaps[2] = SwapEntry(1, 14, 3, 12, 5, 10);
		 smFieldSwaps[3] = SwapEntry(4, 14, 2, 12, 6, 10);
		 smFieldSwaps[4] = SwapEntry(6, 14, 4, 12, 2, 10);
		 smFieldSwaps[5] = SwapEntry(7, 14, 6, 12, 5, 10);
		 smFieldSwaps[6] = SwapEntry(7, 14, 5, 12, 3, 10);
		 smFieldSwaps[7] = SwapEntry(2, 19, 3, 15, 7, 10);
		 smFieldSwaps[8] = SwapEntry(3, 19, 4, 15, 6, 13);
		 smFieldSwaps[9] = SwapEntry(1, 19, 3, 15, 5, 13);
		 smFieldSwaps[10] = SwapEntry(4, 19, 2, 15, 6, 13);
		 smFieldSwaps[11] = SwapEntry(6, 19, 4, 15, 2, 13);
		 return true;

	default:
		 break;
	}

	smFieldSwaps[0]  = SwapEntry(0, 2, 1, 3, 0, 0);
	smFieldSwaps[1]  = SwapEntry(0, 2, 1, 4, 0, 0);
	smFieldSwaps[2]  = SwapEntry(0, 2, 1, 5, 0, 0);
	smFieldSwaps[3]  = SwapEntry(0, 2, 1, 6, 0, 0);
	smFieldSwaps[4]  = SwapEntry(0, 2, 1, 7, 0, 0);
	smFieldSwaps[5]  = SwapEntry(0, 3, 1, 4, 0, 0);
	smFieldSwaps[6]  = SwapEntry(0, 3, 1, 5, 0, 0);
	smFieldSwaps[7]  = SwapEntry(0, 3, 1, 6, 0, 0);
	smFieldSwaps[8]  = SwapEntry(0, 3, 1, 7, 0, 0);
	smFieldSwaps[9]  = SwapEntry(0, 4, 1, 5, 0, 0);
	smFieldSwaps[10] = SwapEntry(0, 4, 1, 6, 0, 0);
	smFieldSwaps[11] = SwapEntry(0, 4, 1, 7, 0, 0);
	return true;
}


OmnString 
AosQueryField::getStrIILName(const int field_idx)
{
	OmnString iilname = "_zt44_str_index_";
	iilname << field_idx;
	return iilname;
}


OmnString 
AosQueryField::getU64IILName(const int field_idx)
{
	OmnString iilname = "_zt44_u64_index_";
	iilname << field_idx;
	return iilname;
}


bool
AosQueryField::createCond(AosQueryTesterUtil::CondDef &cond)
{
	// It create a condition for the field 
	cond.reverse = false;	
	cond.field_idx = mFieldIdx;
	cond.field_name = getFieldname();
	cond.maxValue = mMaxValue;
	cond.minValue = mMinValue;
	cond.max = mMaxValue;
	cond.min = mMinValue;

	mCond = cond;
	setCondField(true);

	switch (rand() % 11)
	{
		case 0: return pickGEValue(cond);
		case 1: return pickGTValue(cond);
		case 2: return pickLTValue(cond);
		case 3: return pickLEValue(cond);
		case 4: return pickEQValue(cond);
		case 5: return pickNEValue(cond);
		//	case 6: return pickANValue(cond);
		case 6:
		case 7: return pickR1Value(cond);
		case 8: return pickR2Value(cond);
		case 9: return pickR3Value(cond);
		case 10: return pickR4Value(cond);

		default:
			 OmnAlarm << "internal error" << enderr;
			 return false;
	}

	return true;
}


bool
AosQueryField::pickGEValue(AosQueryTesterUtil::CondDef &cond)
{
	cond.opr = eAosOpr_ge;
	mOpr = eAosOpr_ge;
	mCondValue1 = pickValue();

	cond.value1 = mCondValue1;
	cond.value2 = 0;
	if (cond.value1 >= cond.min)
		cond.min = cond.value1;

	cond.iilname = getFieldIILName();
	return true;
}


OmnString
AosQueryField::getFieldValueStr(const u64 value) const
{
	// The value is defined by:
	// 		[right, pattern, left]
	// 		[right, pattern, left]
	// 		[right, pattern, left]
	OmnString ss;
	ss << value;
	/*
	if(ss.length() < eMaxDigitLen)
	{
		OmnString prefix;
		for(int i = 0;i < (eMaxDigitLen - ss.length());i++)
		{
			prefix << "0";
		}
		prefix << ss;
		ss = prefix;
	}
	*/
	return ss;
}


bool
AosQueryField::pickGTValue(AosQueryTesterUtil::CondDef &cond)
{
	cond.opr = eAosOpr_gt;
	mOpr = cond.opr;
	mCondValue1 = pickValue();

	cond.value1 = mCondValue1;
	cond.value2 = 0;
	if (cond.value1 >= cond.min)
		cond.min = cond.value1 + 1;

	cond.iilname = getFieldIILName();
	return true;
}


bool
AosQueryField::pickLTValue(AosQueryTesterUtil::CondDef &cond)
{
	// This function creates a condition:
	// 		field < value
	// where 'value' is a randomly picked value, which can be in 
	// the range or out of the range.
	cond.opr = eAosOpr_lt;
	cond.iilname = getFieldIILName();

	mCondValue1 = pickValue();
	mOpr = cond.opr;
	cond.value1 = mCondValue1;
	cond.value2 = 0;

	if (cond.value1 <= cond.max)
		cond.max = cond.value1 - 1;

	return true;
}


bool
AosQueryField::pickLEValue(AosQueryTesterUtil::CondDef &cond)
{
	// This function creates a condition:
	// 		field <= value
	// where 'value' is a randomly picked value, which can be in 
	// the range or out of the range.
	cond.opr = eAosOpr_le;
	cond.iilname = getFieldIILName();

	mCondValue1 = pickValue();
	mOpr = cond.opr;
	cond.value1 = mCondValue1;
	cond.value2 = 0;
	if (cond.value1 <= cond.max)
		cond.max = cond.value1;

	return true;
}


bool
AosQueryField::pickEQValue(AosQueryTesterUtil::CondDef &cond)
{
	// This function creates a condition:
	// 		field <= value
	// where 'value' is a randomly picked value, which can be in 
	// the range or out of the range.
	cond.opr = eAosOpr_eq;
	cond.iilname = getFieldIILName();

	mCondValue1 = pickValue();
	mOpr = cond.opr;
	cond.value1 = mCondValue1;
	cond.value2 = 0;

	return true;
}


bool
AosQueryField::pickNEValue(AosQueryTesterUtil::CondDef &cond)
{
	// This function creates a condition:
	// 		field <= value
	// where 'value' is a randomly picked value, which can be in 
	// the range or out of the range.
	cond.opr = eAosOpr_ne;
	cond.iilname = getFieldIILName();

	mCondValue1 = pickValue();
	mOpr = cond.opr;
	cond.value1 = mCondValue1;
	cond.value2 = 0;
	return true;
}


bool
AosQueryField::pickANValue(AosQueryTesterUtil::CondDef &cond)
{
	// This function creates a condition:
	// 		field <= value
	// where 'value' is a randomly picked value, which can be in 
	// the range or out of the range.
	cond.opr = eAosOpr_an;
	cond.iilname = getFieldIILName();

	mCondValue1 = pickValue();
	mOpr = cond.opr;
	cond.value1 = mCondValue1;
	cond.value2 = 0;
	return true;
}


bool
AosQueryField::pickR1Value(AosQueryTesterUtil::CondDef &cond)
{
	// This function creates a condition:
	// 		field <= value
	// where 'value' is a randomly picked value, which can be in 
	// the range or out of the range.
	cond.opr = eAosOpr_range_ge_le;
	mOpr = cond.opr;
	cond.iilname = getFieldIILName();

	mCondValue1 = pickValue();
	mCondValue2 = pickValue();
	if (mCondValue1 > mCondValue2)
	{
		u64 vv = mCondValue1;
		mCondValue1 = mCondValue2;
		mCondValue2 = vv;
	}

	cond.value1 = mCondValue1;
	cond.value2 = mCondValue2;
	if (cond.value2 <= cond.max)
		cond.max = cond.value2;

	if (cond.value1 >= cond.min)
		cond.min = cond.value1;

	return true;
}


bool
AosQueryField::pickR2Value(AosQueryTesterUtil::CondDef &cond)
{
	// This function creates a condition:
	// 		field <= value
	// where 'value' is a randomly picked value, which can be in 
	// the range or out of the range.
	cond.opr = eAosOpr_range_ge_lt;
	mOpr = cond.opr;
	cond.iilname = getFieldIILName();

	mCondValue1 = pickValue();
	mCondValue2 = pickValue();
	if (mCondValue1 > mCondValue2)
	{
		u64 vv = mCondValue1;
		mCondValue1 = mCondValue2;
		mCondValue2 = vv;
	}

	cond.value1 = mCondValue1;
	cond.value2 = mCondValue2;
	if (cond.value2 <= cond.max)
		cond.max = cond.value2 - 1;

	if (cond.value1 >= cond.min)
		cond.min = cond.value1;

	return true;
}


bool
AosQueryField::pickR3Value(AosQueryTesterUtil::CondDef &cond)
{
	// This function creates a condition:
	// 		field <= value
	// where 'value' is a randomly picked value, which can be in 
	// the range or out of the range.
	cond.opr = eAosOpr_range_gt_le;
	mOpr = cond.opr;
	cond.iilname = getFieldIILName();

	mCondValue1 = pickValue();
	mCondValue2 = pickValue();
	if (mCondValue1 > mCondValue2)
	{
		u64 vv = mCondValue1;
		mCondValue1 = mCondValue2;
		mCondValue2 = vv;
	}

	cond.value1 = mCondValue1;
	cond.value2 = mCondValue2;
	if (cond.value2 <= cond.max)
		cond.max = cond.value2;

	if (cond.value1 >= cond.min)
		cond.min = cond.value1 + 1;

	return true;
}


bool
AosQueryField::pickR4Value(AosQueryTesterUtil::CondDef &cond)
{
	// This function creates a condition:
	// 		field <= value
	// where 'value' is a randomly picked value, which can be in 
	// the range or out of the range.
	cond.opr = eAosOpr_range_gt_lt;
	mOpr = cond.opr;
	cond.iilname = getFieldIILName();

	mCondValue1 = pickValue();
	mCondValue2 = pickValue();
	if (mCondValue1 > mCondValue2)
	{
		u64 vv = mCondValue1;
		mCondValue1 = mCondValue2;
		mCondValue2 = vv;
	}

	cond.value1 = mCondValue1;
	cond.value2 = mCondValue2;
	if (cond.value2 <= cond.max)
		cond.max = cond.value2 - 1;

	if (cond.value1 >= cond.min)
		cond.min = cond.value1 + 1;

	return true;
}

//any cond is not suitable for num fields
//keep it for future expansion
bool
AosQueryField::createAnyCond(AosQueryTesterUtil::CondDef &cond)
{
	// It create a condition for the field 
	cond.reverse = false;	
	cond.field_idx = mFieldIdx;
	cond.opr = eAosOpr_an;
	cond.value1 = 0;
	cond.value2 = 0;
	cond.iilname = getFieldIILName();

	return true;
}


int
AosQueryField::getNumDigits(const u64 value)
{
	// It determines the number of digits.
	u64 vv = 10;
	int num_digits = 1;
	while (num_digits < 20 && value > (vv-1))
	{
		vv *= 10;
		num_digits++;
	}

	return num_digits;
}


bool
AosQueryField::initField(const AosQueryField::FieldInfo &info)
{
	mFieldIdx = info.mFieldIdx;
	mMinValue = info.mMinValue;
	mMaxValue = info.mMaxValue;
	return true;
}


bool
AosQueryField::FieldInfo::serializeTo(const AosBuffPtr &buff)
{
	aos_assert_r(buff, false);
	buff->setU64(123456789102634ULL);

	buff->setInt(mFieldIdx);
	buff->setU64(mMinValue);
	buff->setU64(mMaxValue);
	buff->setU8(mIsNumAlpha);
	buff->setU64(mStartDocid);

	buff->setU64(123456789102634ULL);
	return true;
}


bool
AosQueryField::FieldInfo::serializeFrom(const AosBuffPtr &buff)
{
	aos_assert_r(buff, false);
	u64 poison = buff->getU64(0);
	aos_assert_r(poison == 123456789102634ULL, false);

	mFieldIdx   = buff->getInt(-1);
	mMinValue   = buff->getU64(0);
	mMaxValue   = buff->getU64(0);
	mIsNumAlpha = buff->getU8(false);
	mStartDocid = buff->getU64(0);

	aos_assert_r(mFieldIdx >= 0, false);
	aos_assert_r(mStartDocid > 0, false);

	poison = buff->getU64(0);
	aos_assert_r(poison == 123456789102634ULL, false);
	return true;
}


AosQueryField::FieldInfo
AosQueryField::getFieldInfo()
{
	FieldInfo field_info;

	field_info.mFieldIdx = mFieldIdx;
	field_info.mMinValue = mMinValue;
	field_info.mMaxValue = mMaxValue;
	return field_info;
}


AosQueryFieldPtr
AosQueryField::serializeFromBuffStatic(const AosBuffPtr &buff)
{
	int type = buff->getInt(-1);
	aos_assert_r(type > 0, 0);
	AosQueryFieldPtr field;
	switch (type)
	{
	case 1:
		 field = OmnNew AosQueryFieldStr(-1, false);
		 break;

	case 2:
		 // field = OmnNew AosQueryFieldU64(-1);
		 OmnNotImplementedYet;
		 return 0;

	default:
		 OmnAlarm << "Unrecognized field type: " << type << enderr;
		 return 0;
	}

	bool rslt = field->serializeFromBuff(buff);
	aos_assert_r(rslt, 0);
	return field;
}


bool
AosQueryField::serializeToBuff(const AosBuffPtr &buff)
{
	buff->setInt(mFieldIdx);
	buff->setU64(mMinValue);
	buff->setU64(mMaxValue);
	buff->setU64(mMinDocid);
	buff->setU64(mMaxDocid);
	buff->setU8(mIsSortingField);
	buff->setU8(mOpr);
	buff->setU64(mCondValue1);
	buff->setU64(mCondValue2);
	buff->setU8(mIsNumAlpha);
	buff->setU64(mGroupSeedSize);
	buff->setChar(mIsTimeField);

	if (mGroupSeedSize > 0)
	{
		buff->setU32(mGroupSeeds.size());
		for (u32 i=0; i<mGroupSeeds.size(); i++)
		{
			buff->setU32(mGroupSeeds[i]);
		}
	}

	// Chen Ding, 2013/10/26
	buff->setU32(mResultFileIdMap.size());
	ritr_t itr = mResultFileIdMap.begin();
	while (itr != mResultFileIdMap.end())
	{
		buff->setOmnStr(itr->first);
		buff->setU64(itr->second.value);
		buff->setU64(itr->second.docid);
		itr++;
	}

	buff->setU32(mFileIdMap.size());
	eitr_t iditr = mFileIdMap.begin();
	while (iditr != mFileIdMap.end())
	{
		buff->setOmnStr(iditr->first);
		vector<Entry> &values = iditr->second;
		buff->setU32(values.size());
		for (u32 i=0; i<values.size(); i++)
		{
			buff->setU64(values[i].value);
			buff->setU64(values[i].docid);
		}
		iditr++;
	}

	aos_assert_r(mPatterns.size() > 0, false);
	buff->setU32(mPatterns.size());
	for (u32 i=0; i<mPatterns.size(); i++)
	{
		buff->setInt(mPatterns[i].right);
		buff->setU32(mPatterns[i].pattern);
		buff->setInt(mPatterns[i].left);
		buff->setInt(mPatterns[i].max);
	}
	return true;
}


bool
AosQueryField::serializeFromBuff(const AosBuffPtr &buff)
{
	mGroupSeeds.clear();
	mFieldIdx = 		buff->getInt(-1);
	mMinValue = 		buff->getU64(0);
	mMaxValue = 		buff->getU64(0);
	mMinDocid = 		buff->getU64(0);
	mMaxDocid = 		buff->getU64(0);
	mIsSortingField = 	buff->getU8(false);
	mOpr = 	   (AosOpr) buff->getU8(0);
	mCondValue1 =    	buff->getU64(0);
	mCondValue2 =    	buff->getU64(0);
	mIsNumAlpha = 		buff->getU8(false);
	mGroupSeedSize =    buff->getU64(0);
	mIsTimeField = 		buff->getChar(false);

	if (mGroupSeedSize > 0)
	{
		u32 nn = buff->getU32(0);
		for (u32 i=0; i<nn; i++)
		{
			u32 seed = buff->getU32(0);
			aos_assert_r(seed > 0, false);
			mGroupSeeds.push_back(seed);
		}
	}

	// Chen Ding, 2013/10/26
	u32 nn = buff->getU32(0);
	mResultFileIdMap.clear();
	for (u32 i=0; i<nn; i++)
	{
		OmnString iilname = buff->getOmnStr("");
		aos_assert_r(iilname != "", false);
		u64 fileid = buff->getU64(0);
		u64 size = buff->getU64(0);
		mResultFileIdMap[iilname] = Entry(fileid, size);
	}

	mFileIdMap.clear();
	u32 num_iilnames = buff->getU32(0);
	for (u32 i=0; i<num_iilnames; i++)
	{
		OmnString iilname = buff->getOmnStr("");
		aos_assert_r(iilname != "", false);
		u32 nn = buff->getU32(0);
		aos_assert_r(nn > 0, false);

		vector<Entry> values;
		for (u32 k=0; k<nn; k++)
		{
			u64 file_id = buff->getU64(0);
			u64 size = buff->getU64(0);
			values.push_back(Entry(file_id, size));
		}
		mFileIdMap[iilname] = values;
	}

	nn = buff->getU32(0);
	aos_assert_r(nn > 0, false);
	mPatterns.clear();
	for (u32 i=0; i<nn; i++)
	{
		Pattern pattern;
		pattern.right = buff->getInt(-1);
		pattern.pattern = buff->getU32(0);
		pattern.left = buff->getInt(-1);
		pattern.max = buff->getInt(-1);
		mPatterns.push_back(pattern);
	}	
	return true;
}


u64 
AosQueryField::getValueByHash(const u64 docid) const
{
	// Check whether the field values are 'grouped'. 
	// If it is grouped, 'docid' will be converted to
	// one of the group seeds (defined in mGroupSeeds).
	// This means that 'GroupSeeds' divide the docid space
	// into a number of docid sets, one for each group seed. 
	// Docids in each docid set are converted to the 
	// corresponding seed. 
	//
	u64 new_docid = docid;
	if (mGroupSeedSize > 0)
	{
		u64 dd = docid % mGroupSeedSize;
		new_docid = 0;
		for (u32 i=0; i<mGroupSeeds.size(); i++)
		{
			new_docid += mGroupSeeds[i];
			if (dd <= new_docid)
			{
				break;
			}
		}
		return new_docid;
	}

	u64 value = 5236;
	value += 123 * (mFieldIdx << (mFieldIdx+1));

	u8 *dd = (u8*)&new_docid;
	for (int i=0; i<8; i++)
	{
		value += (dd[i] << (15-i));
	}
	return value;
}


u64 
AosQueryField::getFieldValueBySwapping(const u64 docid) const
{
	aos_assert_r(mFieldIdx >= 0 && (u32)mFieldIdx < smFieldSwaps.size(), 0);
	int idx = smFieldSwaps.size() - mFieldIdx - 1;

	u64 new_docid = docid;
	new_docid = new_docid | (((docid >> smFieldSwaps[idx].from[0]) & 0x01) << smFieldSwaps[idx].to[0]);
	new_docid = new_docid | (((docid >> smFieldSwaps[idx].from[1]) & 0x01) << smFieldSwaps[idx].to[1]);
	new_docid = new_docid | (((docid >> smFieldSwaps[idx].from[2]) & 0x01) << smFieldSwaps[idx].to[2]);
	return new_docid;
}


u64 
AosQueryField::getFieldValueByBitPattern(const u64 docid) const
{
	// It calculates a new value by re-arrange bits:
	// 		A: [right, pattern, left]
	// 		B: [right, pattern, left]
	// 		C: [right, pattern, left]
	aos_assert_r(mPatterns.size() == 4, 0);
	u64 value = 
		(((docid >> mPatterns[0].right) & mPatterns[0].pattern) << mPatterns[0].left) + 
		(((docid >> mPatterns[1].right) & mPatterns[1].pattern) << mPatterns[1].left) + 
		(((docid >> mPatterns[2].right) & mPatterns[2].pattern) << mPatterns[2].left) +
		(((docid >> mPatterns[3].right) & mPatterns[3].pattern) << mPatterns[3].left);
	return value;
}


u64
AosQueryField::docid2U64Value(const u64 docid) const
{
	// This function calculates the field value based on docids. 
	// There are several methods of converting a docid to its value. 

	return getFieldValueByBitPattern(docid);
}


OmnString
AosQueryField::toString() const
{
	OmnString ss;
	ss << "FieldIdx: " << mFieldIdx
		<< ", min: " << mMinValue << ":" << mMinDocid
		<< ", max: " << mMaxValue << ":" << mMaxDocid
		<< ", sorting: " << mIsSortingField
		<< ", opr: " << AosOpr_toStr(mOpr)
		<< ", cond min value : " << mCondValue1
		<< ", cond max value : " << mCondValue2
		<< ", is_alpha: " << mIsNumAlpha
		<< ", start docid: " << mStartDocid
		<< ", group seed_size: " << mGroupSeedSize;

	if (mGroupSeeds.size() > 0)
	{
		ss << ", group seeds: ";
		for (u32 i=0; i<mGroupSeeds.size(); i++)
		{
			if (i > 0) ss << ", ";
			ss << mGroupSeeds[i];
		}
	}
	
	if (mKnownDocids.size() > 0)
	{
		ss << ". Known Docids: ";

		for (u32 i=0; i<mKnownDocids.size(); i++)
		{
			if (i > 0) ss << ", ";
			ss << mKnownDocids[i];
		}
	}
	return ss;
}


int
AosQueryField::docid2EpochDay(const u64 docid)
{
	return docid % (smTester->getMaxTimeField() - smTester->getMinTimeField()+1) 
		+ smTester->getMinTimeField();
}


bool
AosQueryField::convertIILName(
				const AosRundataPtr &rdata,
				const int epoch_day,
				const OmnString &iilname,
				bool &converted,
				OmnString &new_iilname)
{
/*	if(iilname == "_zt44_str_index_0")
	{
		if(epoch_day%1 == 0)
		{
			new_iilname = "_zt44_str_index_0_0";
		}
		else
		{
			new_iilname = "_zt44_str_index_0_1";
		}
		converted = true;
	}
	else if(iilname == "_zt44_str_index_2")
	{
		if(epoch_day%1 == 0)
		{
			new_iilname = "_zt44_str_index_2_0";
		}
		else
		{
			new_iilname = "_zt44_str_index_2_1";
		}
		converted = true;
	}
	else
	{
		converted = false;
	}
*/
	converted = false;
	return true;
}



bool
AosQueryField::addFileId(
		const OmnString &iilname, 
		const u64 fileid, 
		const u64 size)
{
	mLock->lock();
	eitr_t itr = mFileIdMap.find(iilname);
	if (itr == mFileIdMap.end())
	{
		vector<Entry> values;
		values.push_back(Entry(fileid, size));
		mFileIdMap[iilname] = values;
	}
	else
	{
		itr->second.push_back(Entry(fileid, size));
	}
	mLock->unlock();
	return true;
}


bool
AosQueryField::setResultFile(
		const OmnString &iilname, 
		const u64 fileid, 
		const u64 filesize)
{
	ritr_t itr = mResultFileIdMap.find(iilname);
	aos_assert_r(itr == mResultFileIdMap.end(), false);

	mResultFileIdMap[iilname] = Entry(fileid, filesize);
	return true;
}


u64
AosQueryField::pickValue()
{
	// It randomly picks a value in one of the following cases:
	// 		< mMinValue
	// 		[mMinValue, mMaxValue]
	// 		> mMaxValue
	aos_assert_r(mMaxValue >= mMinValue, 0);
	int vv = rand() % 100;
	if (vv < 5)
	{
		if (mMinValue <= 1) return 0;
		return rand() % mMinValue;
	}

	if (vv < 10)
	{
		return mMaxValue + (rand() % 10000);
	}

	u64 size = mMaxValue - mMinValue;
	return rand() % size + mMinValue;
}


u64
AosQueryField::getNextValue(const u64 value) const
{
	return value+1;
}


bool		
AosQueryField::isDocValid(const u64 &docid)const
{
	
	u64 u64_value = getFieldValueByBitPattern(docid);
	
	return AosIILUtil::valueMatch(u64_value, mOpr, mCondValue1, mCondValue2); 
}


u64
AosQueryField::value2Docid(const u64 value) const
{
	aos_assert_r(mPatterns.size() == 4, 0);
	u32 v1 = ((value >> mPatterns[0].left) & mPatterns[0].pattern);
	u32 v2 = ((value >> mPatterns[1].left) & mPatterns[1].pattern);
	u32 v3 = ((value >> mPatterns[2].left) & mPatterns[2].pattern);
	u32 v4 = ((value >> mPatterns[3].left) & mPatterns[3].pattern);

	u64 docid = 
		(v1 << mPatterns[0].right) + 
		(v2 << mPatterns[1].right) + 
		(v3 << mPatterns[2].right) +
		(v4 << mPatterns[3].right);
	return docid;
}


OmnString
AosQueryField::u64Value2StrValue(const u64 value) const
{
	OmnString ss;
	ss << value;
	if(ss.length() < eMaxDigitLen)
	{
		OmnString prefix;
		for(int i = 0;i < (eMaxDigitLen - ss.length());i++)
		{
			prefix << "0";
		}
		prefix << ss;
		ss = prefix;
	}
	return ss;
}


u64
AosQueryField::strValue2U64Value(const OmnString &value) const
{
	u64 v = atoll(value.data());
	return v;
}


OmnString
AosQueryField::getNextValue(const OmnString &crt_value, u64 &docid) const
{
	u64 value = strValue2U64Value(crt_value);
	u64 next_value = getNextValue(value);
	if (next_value < mMinValue || next_value > mMaxValue) return "";
	docid = value2Docid(next_value);
	return u64Value2StrValue(next_value);
}


OmnString
AosQueryField::convertFieldValue(const u64 docid) const
{
	u64 value = docid2U64Value(docid);
	return u64Value2StrValue(value);
}

