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
#ifndef AOS_StatUtil_StatModifyInfo_h
#define AOS_StatUtil_StatModifyInfo_h

#include "Util/String.h"
#include "Util/Ptrs.h"
#include "Util/Buff.h"
#include "Vector2DUtil/Vt2dModifyInfo.h"

#include <vector>

using namespace std;

class AosStatModifyInfo
{
public:
	enum E
	{
		NORMAL,
		MINMAX,
		DIST_COUNT,

		INVALID
	};
public:
	E 							mMeasureType;
	u64							mRecordLen;
	int							mSdocidPos;
	int							mKeyPos;
	int							mIsNewFlagPos;	
	OmnString 					mOpr;
	AosDataRecordType::E 		mRecordType;
	
	map<u32, u32>				mVt2dIdMap;
	
	vector<AosVt2dModifyInfo>	mVt2dMdfInfos;

public:
	AosStatModifyInfo();

	bool addVt2dMdfInfo(
			const AosRundataPtr &rdata,
			const u32 vt2d_idx,
			AosVt2dInfo& vt2d_info);

	bool addBuff2Measure(
			const AosRundataPtr &rdata,
			const int idx,
			const u32 vt2d_idx,
			AosVt2dInfo &vt2d_info);

	bool addMeasure(
			const AosRundataPtr &rdata,
			const OmnString name,
			const int value_input_pos,
			const u32 value_data_len,
			const int time_input_pos,
			const u32 vt2d_idx,
			AosVt2dInfo &vt2d_info);

	//yang
	bool addMeasure(
				const AosRundataPtr &rdata,
				const int idx,
				const int value_input_pos,
				const u32 value_data_len,
				const int time_input_pos,
				const u32 vt2d_idx,
				AosVt2dInfo &vt2d_info);

	vector<AosVt2dModifyInfo> & getVt2dMdfInfos();

	bool serializeTo(
			const AosRundataPtr &rdata,
			const AosBuffPtr &buff);
	
	bool serializeFrom(
			const AosRundataPtr &rdata,
			const AosBuffPtr &buff);
	
	void clear();

};

#endif
