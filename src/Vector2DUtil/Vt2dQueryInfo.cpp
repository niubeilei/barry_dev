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

/*
#include "Vector2DUtil/Vt2dQueryInfo.h"

AosVt2dQueryInfo::AosVt2dQueryInfo()
{
}


AosVt2dQueryInfo::~AosVt2dQueryInfo()
{
}


bool
AosVt2dQueryInfo::init(
		vector<TimeArea> &time_areas,
		AosStatTimeUnit::E &grpby_time_unit,
		AosStatTimeUnit::E &vt2d_time_unit,
		MeasureValueDef &vt2d_measure_def)
{
	bool rslt = resolveQryTime(time_areas,
			vt2d_time_unit, grpby_time_unit);
	aos_assert_r(rslt, false);
	
	mGrpbyTimeUnit = grpby_time_unit;

	mMeasureMapper.mInputputDef = vt2d_measure_def;
	mMeasureMapper.mOutputDef.mHasValidFlag = false;
	mMeasureProcer.mValueDef.mHasValidFlag = false;
	
	mQryRsltProc = AosVt2dQryRsltProc::createQryRsltProc(
				mGrpbyTimeUnit, vt2d_time_unit);
	aos_assert_r(mQryRsltProc, false);

	return true;
}


bool
AosVt2dQueryInfo::addMeasure(
		const OmnString &name,
		Vt2dInfo &vt2d_info)
{
	// this measure_map is from Vt2d_measure to query_measure.
	
	int m_idx = vt2d_info.getMeasureIdx(name);
	aos_assert_r(m_idx >=0, false);
	aos_assert_r(m_idx < vt2d_info.mMeasures.size(), false);

	MeasureInfo & measure = vt2d_info.mMeasures[m_idx];
	
	int output_idx = mMeasureMapper.mOutputDef.getValueNum();
	AosAggrFuncObjPtr agr_func;
	agr_func = AosVector2DUtil::pickStructProc(rdata, measure.mAgrType);
	aos_assert_r(agr_func, false);
	
	mMeasureMapper.mOutputDef.add(measure.mDataType);
	mMeasureMapper.add(rdata, m_idx, output_idx, agr_func);
	mMeasureProcer.mValueDef.add(measure.mDataType);
	mMeasureProcer.add(rdata, output_idx, agr_func);
	return true;
}


//AosVt2dQryRsltPtr
//AosVt2dQueryInfo::createQryRslt()
//{
//	AosVt2dQryRsltPtr grpby_rslt = mQryRsltProc->createQryRslt();
//	return grpby_rslt;
//}


bool
AosVt2dQueryInfo::resolveQryTime(
		vector<TimeArea> &orig_time_areas,
		AosStatTimeUnit::E &vt2d_time_unit,
		AosStatTimeUnit::E &grpby_time_unit)
{
	int64_t start_time = -1, end_time = -1;
	for(u32 i=0; i<orig_time_areas.size(); i++)
	{
		start_time = AosStatTimeUnit::parseTimeValue(orig_time_areas[i].start_time,
				orig_time_areas[i].time_unit, vt2d_time_unit);
		aos_assert_r(start_time >=0, false);

		end_time = AosStatTimeUnit::parseTimeValue(orig_time_areas[i].end_time,
				orig_time_areas[i].time_unit, vt2d_time_unit);
		aos_assert_r(end_time >=0, false);
		
		AosTimeArea new_time_area(start_time, end_time, vt2d_time_unit);
		mQryTimeArea.push_back(new_time_area);
	}
	
	if(mQryTimeArea.size() == 0 && grpby_time_unit != AosStatTimeUnit::eInvalid)
	{
		// need retrieve each value by each time.
		AosTimeArea crt_time_area(-1, -1, vt2d_time_unit); 
		mQryTimeArea.push_back(crt_time_area);
	}
	
	return true;	
}

*/
