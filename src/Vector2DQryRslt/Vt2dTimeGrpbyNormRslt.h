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
// 2014/08/06 Created by Ketty
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Vector2DQryRslt_Vt2dTimeGrpbyNormRslt_h
#define Vector2DQryRslt_Vt2dTimeGrpbyNormRslt_h

#include "Vector2DUtil/Vt2dRecord.h"
#include "Vector2DQryRslt/Vt2dQryRslt.h"

using namespace std;

class AosVt2dTimeGrpbyNormRslt: public AosVt2dQryRslt 
{
	OmnDefineRCObject;
	
	typedef pair<i64, u32>		Value2ValueIdxPair;

private:
	vector<i64>					mGrpbyTimeValues;
	vector<Value2ValueIdxPair>	mTimeValueToGrpbyValueIdx;

public:
	AosVt2dTimeGrpbyNormRslt(const u32 measure_value_size)
	:
	AosVt2dQryRslt(measure_value_size)
	{
	}

	bool	appendGrpbyTimeValue(
			const u32 timeidx, 
			const i64 time_value,
			const i64 grpby_time_value)
	{
		// assume this time_area's is seqence.
		//
		if(mGrpbyTimeValues.size() == 0 ||
				mGrpbyTimeValues[mGrpbyTimeValues.size() -1] != grpby_time_value)
		{
			mGrpbyTimeValues.push_back(grpby_time_value);
		}
		
		aos_assert_r(mTimeValueToGrpbyValueIdx.size() == timeidx, false); 
		u32 grpby_value_idx = mGrpbyTimeValues.size() - 1;
		mTimeValueToGrpbyValueIdx.push_back(make_pair(time_value, grpby_value_idx));	
		return true;
	}
	
	u32	getValueNums(){ return mGrpbyTimeValues.size(); };
	
	bool	getGrpbyTimeValue(
			const u32 timeidx,
			const i64 time_value,
			i64 &grpby_time_value)
	{
		aos_assert_r(timeidx < mTimeValueToGrpbyValueIdx.size(), false);
		aos_assert_r(mTimeValueToGrpbyValueIdx[timeidx].first == time_value, false);
		
		u32 idx = mTimeValueToGrpbyValueIdx[timeidx].second;
		aos_assert_r(idx < mGrpbyTimeValues.size(), false);
		grpby_time_value = mGrpbyTimeValues[idx];
		return true;
	}

};

#endif
