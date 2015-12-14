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

#include "Vector2DUtil/Vt2dModifyInfo.h"
#include "Debug/Debug.h"

AosVt2dModifyInfo::AosVt2dModifyInfo()
:
mRecordLen(0),
mSdocidPos(0),
mTimeFieldPos(0)
{
}


AosVt2dModifyInfo::~AosVt2dModifyInfo()
{
}

//yang
bool
AosVt2dModifyInfo::initOutputDef(
		const u32 vt2d_idx,
		AosVt2dInfo &vt2d_info)
{
	mVt2dIdx = vt2d_idx;
	//yang
	mMeasureMapper.mRecordType = mRecordType;

	vt2d_info.initVt2dValueDef(mMeasureMapper.mOutputDef);
	//mMeasureMapper.mOutputDef = vt2d_measure_def;
	OmnTagFuncInfo << endl;
	return true;
}

bool
AosVt2dModifyInfo::init(
		const u32 vt2d_idx,
		int	sdocid_pos,
		int	time_pos,
		const u64 rcd_len,
		AosVt2dInfo &vt2d_info)
		//AosMeasureValueDef &vt2d_measure_def)
{
	mVt2dIdx = vt2d_idx;
	mSdocidPos = sdocid_pos;
	mTimeFieldPos = time_pos;
	mRecordLen = rcd_len;
	
	mMeasureMapper.mInputDef.mValueSize = mRecordLen;
	mMeasureMapper.mInputDef.mHasValidFlag = false;

	//yang
	mMeasureMapper.mRecordType = mRecordType;

	vt2d_info.initVt2dValueDef(mMeasureMapper.mOutputDef);
	//mMeasureMapper.mOutputDef = vt2d_measure_def;
	OmnTagFuncInfo << endl;
	return true;
}


bool
AosVt2dModifyInfo::addMeasure(
		const AosRundataPtr &rdata,
		const OmnString &name,
		const int input_pos,
		const u32 input_datalen,
		AosVt2dInfo &vt2d_info)
{
	// this measure_map is from modify measure to Vt2d_measure.
	// m_idx is output def
	int m_idx = vt2d_info.getMeasureIdx(name);
	aos_assert_r(m_idx >=0 && (u32)m_idx < vt2d_info.mMeasures.size(), false);

	MeasureInfo & measure = vt2d_info.mMeasures[m_idx];
	u32 measure_data_len = AosDataType::getValueSize(measure.mDataType);
	//aos_assert_r(input_datalen <= measure_data_len, false);
	aos_assert_r(input_datalen == measure_data_len, false);
	
	AosAggrFuncObjPtr agr_func;
	agr_func = AosAggrFuncObj::pickAggrFuncStatic(rdata, measure.mAggrFuncStr);
	aos_assert_r(agr_func, false);
	
	int input_idx = mMeasureMapper.mInputDef.getValueNum();
	mMeasureMapper.mInputDef.add(measure.mDataType, input_pos, input_datalen);
	mMeasureMapper.add(input_idx, m_idx, agr_func);
	OmnTagFuncInfo << endl;
	return true;
}

//yang
bool
AosVt2dModifyInfo::addMeasure(
			const AosRundataPtr &rdata,
			const int idx,
			const int input_pos,
			const u32 input_datalen,
			AosVt2dInfo &vt2d_info)
{
	// this measure_map is from modify measure to Vt2d_measure.

	int m_idx = idx;
	aos_assert_r(m_idx >=0 && (u32)m_idx < vt2d_info.mMeasures.size(), false);

	MeasureInfo & measure = vt2d_info.mMeasures[m_idx];
	u32 measure_data_len = AosDataType::getValueSize(measure.mDataType);
	//aos_assert_r(input_datalen <= measure_data_len, false);
	aos_assert_r(input_datalen == measure_data_len, false);

	AosAggrFuncObjPtr agr_func;
	agr_func = AosAggrFuncObj::pickAggrFuncStatic(rdata, measure.mAggrFuncStr);
	aos_assert_r(agr_func, false);

	int input_idx = mMeasureMapper.mInputDef.getValueNum();
	mMeasureMapper.mInputDef.add(measure.mDataType, input_pos, input_datalen);
	mMeasureMapper.add(input_idx, m_idx, agr_func);
	OmnTagFuncInfo << endl;
	return true;
}


bool
AosVt2dModifyInfo::addBuff2Measure(
			const AosRundataPtr &rdata,
			const int idx,
			AosVt2dInfo &vt2d_info)
{
	int m_idx = idx;
	aos_assert_r(m_idx >=0 && (u32)m_idx < vt2d_info.mMeasures.size(), false);

	MeasureInfo & measure = vt2d_info.mMeasures[m_idx];

	AosAggrFuncObjPtr agr_func;
	agr_func = AosAggrFuncObj::pickAggrFuncStatic(rdata, measure.mAggrFuncStr);
	aos_assert_r(agr_func, false);

	int input_idx = mMeasureMapper.mInputDef.getValueNum();
	mMeasureMapper.mInputDef.add2();
	mMeasureMapper.add(input_idx, m_idx, agr_func);
	OmnTagFuncInfo << endl;
	return true;
}


bool
AosVt2dModifyInfo::serializeTo(
		const AosRundataPtr &rdata,
		const AosBuffPtr &buff)
{
	if(mRecordType != AosDataRecordType::eBuff)
	{
		buff->setU32(mVt2dIdx);
		buff->setU64(mRecordLen);
		buff->setInt(mSdocidPos);
		buff->setInt(mTimeFieldPos);

		buff->setOmnStr(mOpr);

		mMeasureMapper.serializeTo(rdata, buff);
		OmnTagFuncInfo << endl;
	}
	else
	{
		buff->setOmnStr(mOpr);

		buff->setU32(mVt2dIdx);
		mMeasureMapper.serializeTo(rdata, buff);
		OmnTagFuncInfo << endl;
	}
	return true;
}

bool
AosVt2dModifyInfo::serializeFrom(
		const AosRundataPtr &rdata,
		const AosBuffPtr &buff)
{
	if(mRecordType != AosDataRecordType::eBuff)
	{
		mVt2dIdx = buff->getU32(0);
		mRecordLen = buff->getU64(0);
		mSdocidPos = buff->getInt(-1);
		mTimeFieldPos = buff->getInt(-1);

		mOpr = buff->getOmnStr("");
		mMeasureMapper.mRecordType = mRecordType;
		mMeasureMapper.mOpr = mOpr;

		mMeasureMapper.serializeFrom(rdata, buff);
		OmnTagFuncInfo << endl;
	}
	else
	{
		mOpr = buff->getOmnStr("");

		mVt2dIdx = buff->getU32(0);
		mMeasureMapper.mRecordType = mRecordType;
		mMeasureMapper.mOpr = mOpr;

		mMeasureMapper.serializeFrom(rdata, buff);
		OmnTagFuncInfo << endl;
	}
	return true;
}

