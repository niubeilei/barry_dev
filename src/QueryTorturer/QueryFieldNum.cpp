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
#include "QueryTorturer/QueryFieldNum.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "API/AosApi.h"
#include "QueryTorturer/SmartQueryTester.h"
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

AosQueryFieldNum::AosQueryFieldNum(
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


AosQueryFieldNum::~AosQueryFieldNum()
{
}


bool
AosQueryFieldNum::init()
{
	AosQueryField::init();

	mIsNumAlpha = determineNumAlpha();
	mMinValue = 0x99999;
	mMaxValue = 0;

	return true;
}


bool 
AosQueryFieldNum::determineNumAlpha()
{
	return false;
}


/*
bool
AosQueryFieldNum::createFieldContents(const AosRundataPtr &rdata)
{
	// Conceptually, it creates a number of docs in the following format:
	// 	  <doc zky_docid="ddd" 
	// 	  	gruop_id="ddd" 	// u64
	// 	  	field_str_00="random_str"
	// 	  	...
	// 	  	field_str_nn="random_str"
	// 	  	field_u64_00="random_str"
	// 	  	...
	// 	  	field_u64_nn="random_str"/>
	//
	// Field values are calcuated based on its docid.
	// This function creates one string field. 
	//
	// Chen Ding, 2014/02/03
	// If the field is a parallel IIL, it uses the docid to calculate
	// the epoch day an entry is supposed to be. It then uses the 
	// epoch day to determine the IIL name.
	int64_t num_docs_created = 0;
	u64 docid = mStartDocid;

	emap_t value_map;
	lmap_t log_map;

	OmnString log;
	int min_block_size = smTester->getMinBlockSize();
	int max_block_size = smTester->getMaxBlockSize();
	int64_t num_docs_to_create = smTester->getNumDocsToCreate();
	while (num_docs_created < num_docs_to_create)
	{
		// Fields are created in blocks in case there are too many 
		// entries. It randomly determines the block size.
		values.clear();
		u32 size = (u32)OmnRandom::nextInt(min_block_size, max_block_size);
		if (num_docs_created + size > num_docs_to_create)
		{
			size = num_docs_to_create - num_docs_created;
		}

		if (size <= 0) return true;

		OmnString iilname;
		for (u32 i=0; i<size; i++)
		{
			// Field values are calculated based on docids. Please refer to 
			// getFieldValue(docid) for more information about how values
			// are calculated. 
			u64 vv = getFieldValue(docid+i);
			int epoch_day = docid2EpochDay(docid+i);
			rslt = AosIndexMgrObj::convertIILNameStatic(rdata, epoch_day, iilname);
			aos_assert_r(rslt, false);
			aos_assert_r(iilname != "", false);
			AosQueryFieldNumItr_t itr = value_map.find(iilname);
			if (itr == value_map.end())
			{
				vector<Entry> values;
				Entry entry(vv, docid+i);
				values.push_back(entry);
				value_map[iilname] = values;
			}
			else
			{
				itr->second.push_back(Entry(vv, docid+i));
			}

			log = "";
			log << vv << ":" << docid+i << "\n";
			AosQueryFieldNumLItr_t logitr = log_map.find(iilname);
			if (logitr == log_map.end())
			{
				value_map[iilname] = log;
			}
			else
			{
				logitr->second << log;
			}

			if (sgShowLog)
			{
				OmnScreen << "Add value for str field: " << size 
					<< ":" << i << ":" << mFieldIdx << ":"
					<< docid+i << ":" << vv << endl;
			}
		}

		// Add the contents
		eitr_t itr = value_map.begin();
		while (itr != value_map.end())
		{
			OmnString iilname = itr->first;
			addContents(rdata, iilname, smTester->getRecordLen(), itr->second);
			itr++;
		}

		litr_t logitr = log_map.begin();
		while (logitr != log_map.end())
		{
			OmnString iilname = logitr->first;
			OmnString fname = "str_field_";
			fname << mFieldIdx << "_" << iilname;
			OmnFile ff(fname, OmnFile::eCreate AosMemoryCheckerArgs);
			aos_assert_f(ff.isGood(), false);
			ff.append(logitr->second, logitr->second.length(), false);
			logitr++;
		}

		log = "";
		docid += size;
		num_docs_created += size;
	}
	return true;
}
*/


int
AosQueryFieldNum::compareTwoNumbers(u64 value1, u64 value2)
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
AosQueryFieldNum::setMinMax(const u64 docid)
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
AosQueryFieldNum::setRandomMinMax()
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
AosQueryFieldNum::getFieldIILName()
{
	OmnString iilname = "_zt44_str_index_";
	iilname << mFieldIdx;
	return iilname;
}


bool 
AosQueryFieldNum::createCond(AosQueryTesterUtil::CondDef &cond)
{
	AosQueryField::createCond(cond);
	cond.is_str_field = false;
	return true;
}


bool 
AosQueryFieldNum::createAnyCond(AosQueryTesterUtil::CondDef &cond)
{
	AosQueryField::createAnyCond(cond);
	cond.is_str_field = true;
	return true;
}

AosQueryFieldPtr
AosQueryFieldNum::clone()
{
	return OmnNew AosQueryFieldNum(*this);
}


bool 
AosQueryFieldNum::initField(const FieldInfo &info)
{
	AosQueryField::initField(info);

	mIsNumAlpha = info.mIsNumAlpha;
	mStartDocid = info.mStartDocid;
	return true;
}


AosQueryField::FieldInfo
AosQueryFieldNum::getFieldInfo()
{
	FieldInfo field_info = AosQueryField::getFieldInfo();
	field_info.mIsNumAlpha = mIsNumAlpha;
	// field_info.mSeeds = mSeeds;
	field_info.mStartDocid = mStartDocid;
	return field_info;
}


bool
AosQueryFieldNum::serializeFromBuff(const AosBuffPtr &buff)
{
	// It assumes the field type has been read.
	bool rslt = AosQueryField::serializeFromBuff(buff);
	aos_assert_r(rslt, false);
	return true;
}


bool
AosQueryFieldNum::serializeToBuff(const AosBuffPtr &buff)
{
	// 	FieldType(1)		int
	// 	(refer to parent)
	buff->setInt(1);
	bool rslt = AosQueryField::serializeToBuff(buff);
	aos_assert_r(rslt, false);
	return true;
}


OmnString 
AosQueryFieldNum::getMinValue() const
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
AosQueryFieldNum::getMaxValue() const
{
	// Chen Ding, 2013/10/10
	// u64 value = getFieldValue(mMaxValue);
	OmnString ss;
	ss << mMaxValue;
	return ss;
}


OmnString
AosQueryFieldNum::getFieldname() const
{
	return mFieldName;
}
