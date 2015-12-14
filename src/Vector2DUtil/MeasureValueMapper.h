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
#ifndef AOS_Vector2DUtil_MeasureValueMapper_h
#define AOS_Vector2DUtil_MeasureValueMapper_h

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "aosUtil/Types.h"
#include "Rundata/Ptrs.h"
#include "Util/Ptrs.h"
#include "Util/OmnNew.h"
#include "Util/DataTypes.h"
#include "SEInterfaces/AggrFuncObj.h"
#include "Vector2DUtil/MeasureValueDef.h"

//yang
#include "SEInterfaces/DataRecordType.h"

#include <vector>
using namespace std;

struct AosMeasureValueMapper
{
	struct Entry
	{
		int					mInputIdx;
		int					mOutputIdx;
		
		AosAggrFuncObjPtr	mAggrFunc;
		AosAggrFuncObj*		mAggrFuncRaw;
	};

	AosMeasureValueDef	mInputDef;
	AosMeasureValueDef	mOutputDef;
	//has same number of entries with outputDef
	vector<Entry>	mMapEntrys;
	

	//yang
	AosDataRecordType::E mRecordType;
	vector<AosAggrFuncObjPtr> mAggrFuncs;

	OmnString mOpr;

	AosMeasureValueMapper();
	~AosMeasureValueMapper();
	
	OmnString toXmlString(const AosRundataPtr &rdata);
	
	bool 	config(
				const AosRundataPtr &rdata,
				const AosXmlTagPtr &conf);

	int	getInputIndex(u32 i) { return mMapEntrys[i].mInputIdx; }
	int	getOutputIndex(u32 i) { return mMapEntrys[i].mOutputIdx; }
	AosAggrFuncObj* getAggrFunc(u32 i) { return mMapEntrys[i].mAggrFuncRaw; }

	void clear() { mMapEntrys.clear(); }

	u32		getMeasureValueNum(){ return mMapEntrys.size(); };
	AosAggrFuncObjPtr getAggrFuncByIdx(const u32 idx)
	{
		aos_assert_r(idx < mMapEntrys.size(), 0);
		return mMapEntrys[idx].mAggrFunc;
	}
	
	bool serializeTo(
			const AosRundataPtr &rdata, 
			const AosBuffPtr &buff);
	
	bool serializeFrom(
			const AosRundataPtr &rdata, 
			const AosBuffPtr &buff);

	bool	add(
			const int input_idx,
			const int output_idx,
			const AosAggrFuncObjPtr &aggr_func);
	
	bool	addIndex(
			const int input_idx,
			const int output_idx,
			const AosAggrFuncObjPtr &aggr_func);
	
	bool	getOutputData(
			AosRundata *rdata,
			char * input_data,
			const u32 input_len,
			char * output_data,
			const u32 output_len,
			bool &valid);

	bool 	updateOutputData(
			AosRundata *rdata,
			char * input_data,
			const u32 input_len,
			char * output_data,
			const u32 output_len);
};


#endif
