////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
//
// Modification History:
// 2014/01/22 Created by Ketty
////////////////////////////////////////////////////////////////////////////

#include "Vector2DUtil/Vt2dRecord.h"

#include "Util/Buff.h"
#include "Debug/Debug.h"

AosVt2dRecord::AosVt2dRecord()
:
mMeasureValueSize(0),
mHasValidFlag(true),
mEntryDataSize(0),
mStatDocid(0),
mCrtEntryNum(0),
mCrtIndex(0),
mData(0)
{
	u32	buff_len = 64 * 2000;
	mDataHandler = OmnNew AosBuff(buff_len AosMemoryCheckerArgs);
	mData = mDataHandler->data();

	mGrpbyTimeValueList.clear();
}

AosVt2dRecord::AosVt2dRecord(
		const u64 stat_docid,
		const u32 measure_value_size,
		const u32 expect_max_value_num)
:
mMeasureValueSize(measure_value_size),
mEntryDataSize(0),
mStatDocid(stat_docid),
//mDataLen(0),
mCrtEntryNum(0),
mData(0)
{
	mEntryDataSize = sizeof(u64) + mMeasureValueSize;
	//mDataLen 
	//u32	buff_len = eEntryStartOff + mEntryDataSize * expect_max_value_num;
	u32	buff_len = mEntryDataSize * expect_max_value_num;
	mDataHandler = OmnNew AosBuff(buff_len AosMemoryCheckerArgs);
	mData = mDataHandler->data();
}

AosVt2dRecord::~AosVt2dRecord()
{
}


int64_t
AosVt2dRecord::getLastTime()
{
	if(mCrtEntryNum == 0)	return -1;

	//u32 entry_off = eEntryStartOff + (mCrtEntryNum - 1) * mEntryDataSize;
	u32 entry_off = (mCrtEntryNum - 1) * mEntryDataSize;
	aos_assert_r(entry_off + mEntryDataSize <= mDataHandler->buffLen(), -1);

	return *(int64_t *)(mData + entry_off);	
}


bool
AosVt2dRecord::getLastValue(char * &value, u32 &value_len, const bool copy)
{
	if(mCrtEntryNum == 0) return false;
	
	//u32 entry_off = eEntryStartOff + (mCrtEntryNum - 1) * mEntryDataSize;
	u32 entry_off = (mCrtEntryNum - 1) * mEntryDataSize;
	aos_assert_r(entry_off + mEntryDataSize <= mDataHandler->buffLen(), 0);
	
	value_len = mMeasureValueSize; 
	if(!copy)
	{
		value = mData + entry_off + sizeof(int64_t);
	}
	else
	{
		aos_assert_r(value, false);
		memcpy(value, mData + entry_off + sizeof(int64_t), value_len);
	}

	return true;
}


bool
AosVt2dRecord::appendValue(
		i64 time_id,
		char *value,
		u32 value_len)
{
	bool rslt;

	aos_assert_r(value, false);
	aos_assert_r(value_len == mMeasureValueSize, false);

	// filter out all empty values
	//
	if (mHasValidFlag)
	{
		u32 pos = 0;
		u32 dataLen = 8;
		bool hasValue = false;

		for (u32 i = 0; i < mMeasureInfoList->size(); i++)
		{
			if (value[pos] != 0)
			{
				hasValue = true;
				break;
			}

			//1 byte for valid flag
			pos += dataLen + 1;
		}

		//skip if no value
		if (!hasValue)
			return true;
	}

	//u32 new_entry_off = eEntryStartOff + mCrtEntryNum * mEntryDataSize;
	u32 new_entry_off = mCrtEntryNum * mEntryDataSize;
	if(new_entry_off + mEntryDataSize > mDataHandler->buffLen())
	{
		rslt = expandMemory(new_entry_off *2);
		aos_assert_r(rslt, false);
	}
	
	char *new_entry = mData + new_entry_off; 
	*(int64_t *)(new_entry) = time_id;
	memcpy(new_entry + sizeof(int64_t), value, value_len);
	
	mCrtEntryNum++;
	return true;
}


//yang
bool
AosVt2dRecord::appendDistValue(i64 time_id,char *value,u32 value_len)
{
	bool rslt;

	aos_assert_r(value, false);

	//u32 new_entry_off = eEntryStartOff + mCrtEntryNum * mEntryDataSize;
	u32 new_entry_off = mCrtEntryNum * mEntryDataSize;
	if(new_entry_off + mEntryDataSize > mDataHandler->buffLen())
	{
		rslt = expandMemory(new_entry_off * 2);
		aos_assert_r(rslt, false);
	}

	char *new_entry = mData + new_entry_off;
	*(int64_t *)(new_entry) = time_id;
	memcpy(new_entry + sizeof(int64_t), value, value_len);

	mCrtEntryNum++;
	return true;
}


bool
AosVt2dRecord::expandMemory(const u32 new_buff_size)
{
	//JIMODB-1205 2015-11-19 wumeng 
	i64 dataLen = mCrtEntryNum * mEntryDataSize;
	mDataHandler->setDataLen(dataLen);
	bool rslt = mDataHandler->expandMemory1(new_buff_size);
	aos_assert_r(rslt, false);

	mData = mDataHandler->data();
	OmnTagFuncInfo << endl;
	return true;
}


bool
AosVt2dRecord::getValueByIdx(
		const u32 idx, 
		i64 &time_value, 
		char * &value,
		u32 &value_len,
		const bool copy)
{
	aos_assert_r(idx < mCrtEntryNum, false);
	
	u32 entry_off = idx * mEntryDataSize;
	aos_assert_r(entry_off + mEntryDataSize <= mDataHandler->buffLen(), 0);

	time_value = *(i64 *)(mData + entry_off);
	value_len = mMeasureValueSize; 
	if(!copy)
	{
		value = mData + entry_off + sizeof(int64_t);
	}
	else
	{
		aos_assert_r(value, false);
		memcpy(value, mData + entry_off + sizeof(int64_t), value_len);
	}

	return true;
}

int64_t
AosVt2dRecord::getTimeValue(const u32 idx)
{
	aos_assert_r(idx < mCrtEntryNum, false);
	
	u32 entry_off = idx * mEntryDataSize;
	aos_assert_r(entry_off + mEntryDataSize <= mDataHandler->buffLen(), 0);

	int64_t time_value = *(i64 *)(mData + entry_off);
	return time_value;	
}

bool
AosVt2dRecord::copyFrom(AosVt2dRecord *vt2d_rcd)
{
	// this is for vt2d merge.
	aos_assert_r(vt2d_rcd, false);
	bool rslt;

	mMeasureValueSize = vt2d_rcd->mMeasureValueSize;
	mEntryDataSize = vt2d_rcd->mEntryDataSize;
	mStatDocid = mStatDocid;
	mCrtEntryNum = vt2d_rcd->mCrtEntryNum; 
	
	u32 valid_size = (vt2d_rcd->mEntryDataSize * vt2d_rcd->mCrtEntryNum); 
	aos_assert_r(valid_size <= vt2d_rcd->mDataHandler->buffLen(), false);
	if(valid_size == 0)	return true;

	if(!mDataHandler)
	{
		mDataHandler = OmnNew AosBuff(valid_size AosMemoryCheckerArgs);
		mData = mDataHandler->data();	
	}
	else if(mDataHandler->buffLen() < valid_size)
	{
		rslt = expandMemory(valid_size);
		aos_assert_r(rslt, false);
	}
	aos_assert_r(mData && vt2d_rcd->mData, false);
	
	memcpy(mData, vt2d_rcd->mData, valid_size);

	OmnTagFuncInfo << endl;
	return true;
}


bool
AosVt2dRecord::resetData(const u32 max_value_num)
{
	// this is for vt2d merge.
	aos_assert_r(mMeasureValueSize && mEntryDataSize
			&& mStatDocid && mDataHandler, false);
	
	bool rslt;
	mCrtEntryNum = 0;

	u32 max_buff_len = max_value_num * mEntryDataSize;
	if(mDataHandler->buffLen() < max_buff_len)
	{
		rslt = expandMemory(max_buff_len);
		aos_assert_r(rslt, false);
	}
	
	return true;
}

	
bool
AosVt2dRecord::serializeTo(
		const AosRundataPtr &rdata,
		AosBuff *buff)
{
	buff->setU64(mStatDocid);
	buff->setU32(mMeasureValueSize);
	buff->setU32(mCrtEntryNum);	
	
	u32 data_len = mCrtEntryNum * mEntryDataSize;
	buff->setBuff(mData, data_len);
	return true;
}


bool
AosVt2dRecord::serializeFrom(
		const AosRundataPtr &rdata,
		AosBuff * buff)
{
	mStatDocid = buff->getU64(0);
	aos_assert_r(mStatDocid != 0, false);

	mMeasureValueSize = buff->getU32(0);
	aos_assert_r(mMeasureValueSize, false);
	mEntryDataSize = sizeof(u64) + mMeasureValueSize;
	
	mCrtEntryNum = buff->getU32(0);
	
	u32	data_len = mCrtEntryNum * mEntryDataSize;
	mDataHandler = buff->getBuff(data_len, true AosMemoryCheckerArgs); 
	mData = mDataHandler->data();
	return true;	
}

bool
AosVt2dRecord::skipOutOfRange(vector<AosStatTimeArea> *timeAreas)
{
	i64 timeValue;
	char* value;
	u32 vLen;
	
	if (!timeAreas)
		return true;
		
	while (!isIndexEnd())
	{
		getValueByIdx(mCrtIndex, timeValue, value, vLen, false);
	
		if (inTimeRange(timeValue, timeAreas))
			break;
			
		mCrtIndex++;
	}
	
	return true;
}	

bool
AosVt2dRecord::skipByTimeValue(AosRundata* rdata,
							   i64 timeValue, 
							   AosStatTimeUnit::E timeUnit)
{
	//skip value doesn't need procers
	statByTimeValue(rdata, NULL, timeValue, timeUnit, NULL);
	return true;
}

//merge values into given statRcd based on timeValue
bool
AosVt2dRecord::statByTimeValue(AosRundata* rdata,
							AosStatRecord *statRcd,
							i64 timeValue, 
							AosStatTimeUnit::E timeUnit, 
							vector<AosStatTimeArea> *timeAreas)
{
	char *value;
	u32 vLen;
	i64 localTimeValue;
	i64 newTimeValue;
	
	if (mMeasureType == DIST_COUNT)
		return statByDistTimeValue(rdata, statRcd, timeValue, timeUnit, timeAreas);
	
	while (!isIndexEnd())
	{
		getValueByIdx(mCrtIndex, localTimeValue, value, vLen, false);
	
		if (timeUnit != AosStatTimeUnit::eAllTime &&
			timeUnit != AosStatTimeUnit::eInvalid)
		{
			newTimeValue = getGrpbyTimeValue(localTimeValue, timeUnit);
			if (newTimeValue != timeValue)
				break;
		}
			
		//if no statRcd, skip only
		//check time range
		if (statRcd && inTimeRange(localTimeValue, timeAreas))
		{
			//count stat value
			statRcd->addValue(rdata, mVt2dIndex, value, vLen);
		}
						
		mCrtIndex++;
	}
	
	return true;
}

//merge values into given statRcd based on timeValue
bool
AosVt2dRecord::statByDistTimeValue(AosRundata* rdata,
							AosStatRecord *statRcd,
							i64 timeValue,
							AosStatTimeUnit::E timeUnit,
							vector<AosStatTimeArea> *timeAreas)
{
	char *value;
	u32 vLen;
	i64 localTimeValue;
	i64 newTimeValue;
	bool added = false;

	while (!isIndexEnd())
	{
		getValueByIdx(mCrtIndex, localTimeValue, value, vLen, false);

		if (timeUnit != AosStatTimeUnit::eAllTime &&
			timeUnit != AosStatTimeUnit::eInvalid)
		{
			newTimeValue = getGrpbyTimeValue(localTimeValue, timeUnit);
			if (newTimeValue != timeValue)
				break;
		}

		//if already added, no need to add distinct value any more
		//if no statRcd, skip only
		//check time range
		if (statRcd  && inTimeRange(localTimeValue, timeAreas))
		{
			//count stat value
			if (*(i64 *)value)
			{
				statRcd->addDistValue(rdata, value);
			}
		}

		mCrtIndex++;
	}

	return true;
}

bool
AosVt2dRecord::inTimeRange(i64 timeValue, vector<AosStatTimeArea> *timeAreas)
{
	AosStatTimeArea *timeArea;
	i64 newTimeValue;	

	//so far, assume that value into vt2drecord are already in range
	//in reading time
	return true;

	if (!timeAreas)
		return true;
		
	for (u32 i = 0; i < timeAreas->size(); i++)
	{
		timeArea = &((*timeAreas)[i]);
		newTimeValue = AosStatTimeUnit::parseTimeValue(
				 				timeValue, mTimeUnit, timeArea->time_unit);

		if (newTimeValue >= timeArea->start_time && newTimeValue <= timeArea->end_time)
			return true; 
	}	

	return false;
}

int
AosVt2dRecord::getCrtTimeValue(AosStatTimeUnit::E timeUnit)
{
	i64 timeValue;
	char *value;
	u32 vLen;

	getValueByIdx(mCrtIndex, timeValue,	value, vLen, false);
	return getGrpbyTimeValue(timeValue, timeUnit);	
}

void 
AosVt2dRecord::setMeasureType(E measureType)
{
    mMeasureType = measureType;
}

void
AosVt2dRecord::clear()
{
	mMeasureValueSize = 0;
	mEntryDataSize = 0;
	mStatDocid = 0;
	mCrtEntryNum = 0;

	mDataHandler->setDataLen(0);
	mDataHandler->setCrtIdx(0);

	mTimeUnit = AosStatTimeUnit::eInvalid;
	mCrtIndex = 0;

	//*mMeasureInfoList;
}

i64
AosVt2dRecord::getGrpbyTimeValue(i64 timeValue, AosStatTimeUnit::E timeUnit)
{
	i64 newTimeValue;

	//if (mCrtIndex < mGrpbyTimeValueList.size())
	//	return mGrpbyTimeValueList[mCrtIndex];

	newTimeValue = AosStatTimeUnit::parseTimeValue(
			timeValue, mTimeUnit, timeUnit);			

	//mGrpbyTimeValueList.push_back(newTimeValue);
	return newTimeValue;
}
