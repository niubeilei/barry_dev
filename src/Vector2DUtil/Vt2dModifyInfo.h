////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// Modification History:
// 2014/07/25 Created by Ketty 
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_Vector2DUtil_Vt2dModifyInfo_h
#define AOS_Vector2DUtil_Vt2dModifyInfo_h

#include "Util/String.h"
#include "Vector2DUtil/MeasureValueMapper.h"
#include "Vector2DUtil/Vt2dInfo.h"

struct AosVt2dModifyInfo
{
	u32					mVt2dIdx;
	u64					mRecordLen;
	int					mSdocidPos;
	int					mTimeFieldPos;
	
	AosMeasureValueMapper	mMeasureMapper;

	AosDataRecordType::E mRecordType;

	OmnString mOpr;

	//yang,2015/09/07
	enum E
	{
		NORMAL,
		MINMAX,
		DIST_COUNT,

		INVALID
	};
	E mMeasureType;

	AosVt2dModifyInfo();
	~AosVt2dModifyInfo();

	//yang
	bool	initOutputDef(
			const u32 vt2d_idx,
			AosVt2dInfo &vt2d_info);

	bool 	init(
				const u32 vt2d_idx,
				int	sdocid_pos,
				int	time_pos,
				const u64 rcd_len,
				AosVt2dInfo &vt2d_info);
				//AosMeasureValueDef &vt2d_measure_def);

	bool 	addMeasure(
				const AosRundataPtr &rdata,
				const OmnString &name,
				const int input_pos,
				const u32 input_datalen,
				AosVt2dInfo &vt2d_info);

	//yang
	bool 	addMeasure(
				const AosRundataPtr &rdata,
				const int idx,
				const int input_pos,
				const u32 input_datalen,
				AosVt2dInfo &vt2d_info);
	//yang
	bool 	addBuff2Measure(
				const AosRundataPtr &rdata,
				const int idx,
				AosVt2dInfo &vt2d_info);

	bool 	serializeTo(
				const AosRundataPtr &rdata,
				const AosBuffPtr &buff);

	bool 	serializeFrom(
				const AosRundataPtr &rdata,
				const AosBuffPtr &buff);

};

#endif
