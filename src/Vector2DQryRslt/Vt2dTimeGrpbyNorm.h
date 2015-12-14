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

#ifndef Vector2DQryRslt_Vt2dTimeGrpbyNorm_h
#define Vector2DQryRslt_Vt2dTimeGrpbyNorm_h

#include "Vector2DQryRslt/Vt2dTimeGrpby.h"
#include "StatUtil/StatTimeUnit.h"

#include <map>
#include <vector>
using namespace std;

class AosVt2dTimeGrpbyNorm: public AosVt2dTimeGrpby
{

private:
	AosStatTimeUnit::E mGrpbyTimeUnit;

public:
	AosVt2dTimeGrpbyNorm();
	AosVt2dTimeGrpbyNorm(
				const AosStatTimeUnit::E grpby_time_unit,
				const u32 vt2d_idx,
				const AosMeasureValueMapper &value_mapper);
	~AosVt2dTimeGrpbyNorm();
	
	virtual AosVt2dQryRsltPtr createQryRslt();
	
	virtual OmnString getType(){ return "GroupByNorm"; };
	
	virtual bool config(
				const AosRundataPtr &rdata,
				const AosXmlTagPtr &conf);
	
	virtual OmnString toXmlString(const AosRundataPtr &rdata);

	virtual bool appendVt2dRecords(
				AosRundata *rdata,
				AosVt2dQryRslt * qry_rslt,
				vector<u64> &stat_docids,
				vector<AosStatTimeArea> &time_areas);
	
	virtual bool appendValue(
				AosRundata *rdata,
				AosVt2dQryRslt * qry_rslt,
				const u32 stat_doc_idx,
				const u64 stat_docid,
				const u32 time_idx,
				const i64 time_value,
				char *value,
				const u32 value_len);

};

#endif
