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

#ifndef Vector2DQryRslt_Vt2dQryRsltProc_h
#define Vector2DQryRslt_Vt2dQryRsltProc_h

#include "Rundata/Ptrs.h"
#include "SEInterfaces/Ptrs.h"
#include "Util/String.h"
#include "Util/Ptrs.h"
#include "StatUtil/StatTimeArea.h"
#include "StatUtil/StatTimeUnit.h"
#include "Vector2DUtil/Vt2dRecord.h"
#include "Vector2DUtil/MeasureValueMapper.h"
#include "Vector2DQryRslt/Ptrs.h"

class AosVt2dQryRsltProc: public OmnRCObject 
{
	OmnDefineRCObject;

protected:
	u32						mVt2dIdx;
	AosMeasureValueMapper	mValueMapper;
	char * 					mTmpOutputData;
	
public:
	AosVt2dQryRsltProc();
	AosVt2dQryRsltProc(
				const u32 vt2d_idx,
				const AosMeasureValueMapper &value_mapper);
	~AosVt2dQryRsltProc();

	static AosVt2dQryRsltProcPtr createQryRsltProc(
				const u32 vt2d_idx,
				const AosStatTimeUnit::E &grpby_time_unit,
				const AosStatTimeUnit::E &vt2d_time_unit,
				const AosMeasureValueMapper &value_mapper);

	static AosVt2dQryRsltProcPtr createQryRsltProc(
				const AosRundataPtr &rdata,
				const AosXmlTagPtr &conf);
	
	u32		getVt2dIdx(){ return mVt2dIdx; };
	AosMeasureValueMapper & getMeasureMapper(){ return mValueMapper; };

	virtual AosVt2dQryRsltPtr createQryRslt() = 0;
	virtual OmnString getType() = 0;
	
	virtual bool config(
				const AosRundataPtr &rdata,
				const AosXmlTagPtr &conf);
	
	virtual OmnString toXmlString(const AosRundataPtr &rdata);

	virtual bool appendVt2dRecords(
				AosRundata *rdata,
				AosVt2dQryRslt * qry_rslt,
				vector<u64> &stat_docids,
				vector<AosStatTimeArea> &time_areas) = 0;

	virtual bool appendValue(
				AosRundata *rdata,
				AosVt2dQryRslt * qry_rslt,
				const u32 stat_doc_idx,
				const u64 stat_docid,
				const u32 time_idx,
				const i64 time_value,
				char *value,
				const u32 value_len) = 0;
		
};

#endif
