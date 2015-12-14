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
#ifndef AOS_Vector2DUtil_MeasureValueProcer_h
#define AOS_Vector2DUtil_MeasureValueProcer_h

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "aosUtil/Types.h"
#include "Rundata/Ptrs.h"
#include "Util/Ptrs.h"
#include "Util/OmnNew.h"
#include "Util/DataTypes.h"
#include "SEInterfaces/AggrFuncObj.h"
#include "Vector2DUtil/MeasureValueDef.h"

#include <vector>
using namespace std;

struct AosMeasureValueProcer
{
	struct Entry
	{
		int					mIdx;
		AosAggrFuncObjPtr	mAggrFunc;
		AosAggrFuncObj*		mAggrFuncRaw;
	};

	AosMeasureValueDef	mValueDef;
	vector<Entry>	mProcers;
	
	AosMeasureValueProcer();
	~AosMeasureValueProcer();
	
	OmnString toXmlString(const AosRundataPtr &rdata);
	
	bool 	config(
				const AosRundataPtr &rdata,
				const AosXmlTagPtr &conf);
	
	bool	add(
			const int idx,
			const AosAggrFuncObjPtr &aggr_func);
	
	bool 	updateOutputData(
			AosRundata *rdata,
			char * input_data,
			const u32 input_len,
			char * output_data,
			const u32 output_len);

	//bool	 serializeTo(
	//			const AosRundataPtr &rdata,
	//			AosBuff *buff);
	
	//bool 	serializeFrom(
	//			const AosRundataPtr &rdata,
	//			AosBuff *buff);

};


#endif
