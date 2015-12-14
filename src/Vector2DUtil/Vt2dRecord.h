////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: Statistics.h
// Description:
//   
//
// Modification History:
// 2014/01/22 Created by Ketty
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Vector2DUtil_Vt2dRecord_h
#define Vector2DUtil_Vt2dRecord_h

#include "Rundata/Ptrs.h"
#include "SEInterfaces/Ptrs.h"
#include "Util/String.h"
#include "Util/Ptrs.h"
#include "StatUtil/StatTimeUnit.h"
#include "StatUtil/StatTimeArea.h"
#include "StatUtil/StatRecord.h"
#include "IILUtil/IILUtil.h"
#include "Vector2DUtil/MeasureValueMapper.h"

#include <vector>
using namespace std;

class AosVt2dRecord
{
	OmnDefineRCObject;

	//enum
	//{
	//	eStatDocIdOff = 0,
	//	eEntryNumOff = eStatDocIdOff + sizeof(u64),
	//	eEntryStartOff = eEntryNumOff + sizeof(u32),
	//};
public:
	enum E
	{
		NORMAL,
		MINMAX,
		DIST_COUNT,

		INVALID	
	};

	E 			mMeasureType;

private:
	u32			mMeasureValueSize;
	bool		mHasValidFlag;
	u32			mEntryDataSize;

	u64			mStatDocid;
	u32			mCrtEntryNum;

	AosBuffPtr	mDataHandler;
	char *		mData;

	AosStatTimeUnit::E	mTimeUnit;
	u32			mCrtIndex;

	vector<MeasureInfo> *mMeasureInfoList;
	vector<i64> mGrpbyTimeValueList;

	u32 		mVt2dIndex;

	OmnString	mDistValue;
	OmnString	mDistMeasure;

public:	
	AosVt2dRecord();
	AosVt2dRecord(const u64 stat_docid,
					const u32 measure_value_size,
					const u32 expect_max_value_num);
	virtual ~AosVt2dRecord();
	
	u64		getStatDocid(){ return mStatDocid; };
    void	setStatDocid(u64 sdocid) { mStatDocid = sdocid; }	

	void 	setHasValidFlag(bool hasValidFlag) { mHasValidFlag = hasValidFlag; }
	void 	setVt2dIndex(u32 vt2dIndex) { mVt2dIndex = vt2dIndex; }
	void	setMeasureValueSize(u32 len) 
	{ 
		mMeasureValueSize = len; 
		//if (mHasValidFlag)
		//	mMeasureValueSize--;

		mEntryDataSize = mMeasureValueSize + sizeof(u64); 
	}

	int64_t getLastTime();
	bool 	getLastValue(char * &value, u32 &value_len, const bool copy);
	bool	appendValue(i64 time_id, char * value, u32 value_len); 

	u32		getValueNum(){ return mCrtEntryNum; };
	bool	getValueByIdx(const u32 idx,
					i64 &time_value, 
					char * &value,
					u32 &value_len,
					const bool copy);

	bool	copyFrom(AosVt2dRecord * vt2d_rcd);
	bool	resetData(const u32 max_value_num);

	bool 	serializeTo(
				const AosRundataPtr &rdata,
				AosBuff *buff);

	bool 	serializeFrom(
				const AosRundataPtr &rdata,
				AosBuff * buff);

	//the following method used to traverse v2d values
	void	setTimeUnit(AosStatTimeUnit::E timeUnit) { mTimeUnit = timeUnit;}
	AosStatTimeUnit::E getTimeUnit() { return mTimeUnit; }
	int64_t	getTimeValue(const u32 value_idx);

	void	setMeasureInfo(vector<MeasureInfo> *info) { mMeasureInfoList = info; }

	void	resetIndex() { mCrtIndex = 0;}
	bool 	isIndexEnd() { return mCrtIndex >= mCrtEntryNum; }
	void	addIndex()	{mCrtIndex++;}
	bool 	skipOutOfRange(vector<AosStatTimeArea> *timeAreas);		
	bool	skipByTimeValue(AosRundata* rdata,
							i64 timeValue, 
							AosStatTimeUnit::E timeUnit);

	bool	statByTimeValue(AosRundata* rdata,
							AosStatRecord *statRcd, 
							i64 timeValue, 
							AosStatTimeUnit::E timeUnit, 
							vector<AosStatTimeArea> *timeAreas);
	
	bool	statByDistTimeValue(AosRundata* rdata,
							AosStatRecord *statRcd,
							i64 timeValue,
							AosStatTimeUnit::E timeUnit,
							vector<AosStatTimeArea> *timeAreas);

	bool 	inTimeRange(i64 timeValue, vector<AosStatTimeArea> *timeAreas);
	int 	getCrtTimeValue(AosStatTimeUnit::E timeUnit);
	i64 	getGrpbyTimeValue(i64 timeValue, AosStatTimeUnit::E timeUnit);
			
	void 	clear();

	void 	setDistValue(OmnString &value) { mDistValue = value; }
	void 	setDistMeasure(OmnString &measure) { mDistMeasure = measure; }


//yang
	bool appendDistValue(i64 time_id,char *value,u32 value_len);

	void setMeasureType(E measureType);

private:
	bool 	expandMemory(const u32 new_buff_size);

};

#endif
