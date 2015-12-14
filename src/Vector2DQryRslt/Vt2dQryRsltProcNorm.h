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

#ifndef Vector2DQryRslt_Vt2dQryRsltProcNorm_h
#define Vector2DQryRslt_Vt2dQryRsltProcNorm_h

#include "Vector2DQryRslt/Vt2dQryRsltProc.h"

class AosVt2dQryRsltProcNorm: public AosVt2dQryRsltProc 
{

private:
	
public:
	AosVt2dQryRsltProcNorm();
	AosVt2dQryRsltProcNorm(
				const u32 vt2d_idx,
				const AosMeasureValueMapper &value_mapper);
	~AosVt2dQryRsltProcNorm();
	
	virtual AosVt2dQryRsltPtr createQryRslt();

	virtual OmnString getType(){ return "QryRsltProcNorm"; };
	
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
