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

#include "Vector2DQryRslt/Vt2dTimeGrpbyAll.h"

#include "Vector2DQryRslt/Vt2dQryRslt.h"

AosVt2dTimeGrpbyAll::AosVt2dTimeGrpbyAll()
{
}

AosVt2dTimeGrpbyAll::AosVt2dTimeGrpbyAll(
		const u32 vt2d_idx,
		const AosMeasureValueMapper &value_mapper)
:
AosVt2dTimeGrpby(vt2d_idx, value_mapper)
{
}


AosVt2dTimeGrpbyAll::~AosVt2dTimeGrpbyAll()
{
}


AosVt2dQryRsltPtr
AosVt2dTimeGrpbyAll::createQryRslt()
{
	u32 measure_value_size = mValueMapper.mOutputDef.mValueSize;
	return OmnNew AosVt2dQryRslt(measure_value_size);
}


bool
AosVt2dTimeGrpbyAll::config(
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &conf)
{
	aos_assert_r(conf, false);

	AosXmlTagPtr base_conf = conf->getFirstChild("vt2d_qry_rslt_base", false);
	aos_assert_r(base_conf, false);
	bool rslt = AosVt2dQryRsltProc::config(rdata, base_conf);
	aos_assert_r(rslt, false);

	return true;
}


OmnString
AosVt2dTimeGrpbyAll::toXmlString(const AosRundataPtr &rdata)
{
	OmnString str = "<vt2d_qry_rslt_proc type=\"time_grpby_all\" >";
	str << AosVt2dQryRsltProc::toXmlString(rdata);
	
	str << "</vt2d_qry_rslt_proc>"; 
	return str;
}


bool
AosVt2dTimeGrpbyAll::appendVt2dRecords(
		AosRundata *rdata,
		AosVt2dQryRslt * qry_rslt,
		vector<u64> &stat_docids,
		vector<AosStatTimeArea> &time_areas)
{
	qry_rslt->setMaxTimeValueNum(1);
	for(u32 i=0; i<stat_docids.size(); i++)
	{
		qry_rslt->appendVt2dRecord(i, stat_docids[i]);
	}
	
	return true;
}

bool
AosVt2dTimeGrpbyAll::appendValue(
		AosRundata *rdata,
		AosVt2dQryRslt * qry_rslt,
		const u32 stat_doc_idx,
		const u64 stat_docid,
		const u32 time_idx,
		const i64 time_value,
		char *value,
		const u32 value_len)
{
	aos_assert_r(qry_rslt, false);

	bool rslt;
	AosVt2dRecord *vt2d_rcd = qry_rslt->getVt2dRecordRaw(stat_doc_idx, stat_docid);
	aos_assert_r(vt2d_rcd, false);
	
	//if(agr_value.isInvalid())	return true;
	rslt = appendValueToRcd(rdata, vt2d_rcd, 0, value, value_len); 
	aos_assert_r(rslt, false);	
	
	return true;
}


