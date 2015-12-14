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

#include "Vector2DQryRslt/Vt2dQryRsltProcNorm.h"

#include "Vector2DQryRslt/Vt2dQryRslt.h"

AosVt2dQryRsltProcNorm::AosVt2dQryRsltProcNorm()
{
}

AosVt2dQryRsltProcNorm::AosVt2dQryRsltProcNorm(
		const u32 vt2d_idx,
		const AosMeasureValueMapper &value_mapper)
:
AosVt2dQryRsltProc(vt2d_idx, value_mapper)
{
}


AosVt2dQryRsltProcNorm::~AosVt2dQryRsltProcNorm()
{
}


AosVt2dQryRsltPtr
AosVt2dQryRsltProcNorm::createQryRslt()
{
	u32 measure_value_size = mValueMapper.mOutputDef.mValueSize;
	return OmnNew AosVt2dQryRslt(measure_value_size);
}


bool
AosVt2dQryRsltProcNorm::config(
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
AosVt2dQryRsltProcNorm::toXmlString(const AosRundataPtr &rdata)
{
	OmnString str = "<vt2d_qry_rslt_proc type=\"qry_norm\" >";
	str << AosVt2dQryRsltProc::toXmlString(rdata);
	
	str << "</vt2d_qry_rslt_proc>"; 
	return str;
}


bool
AosVt2dQryRsltProcNorm::appendVt2dRecords(
		AosRundata *rdata,
		AosVt2dQryRslt * qry_rslt,
		vector<u64> &stat_docids,
		vector<AosStatTimeArea> &time_areas)
{
	u32 time_value_num = 0;
	i64 crt_time = -1;
	for(u32 i=0; i <time_areas.size(); i++)
	{
		crt_time = time_areas[i].start_time;	
		aos_assert_r(crt_time >=0 && time_areas[i].end_time >=0, false);
		for(; crt_time <= time_areas[i].end_time; crt_time++)
		{
			time_value_num++;
		}
	}

	qry_rslt->setMaxTimeValueNum(time_value_num);
	for(u32 i=0; i<stat_docids.size(); i++)
	{
		qry_rslt->appendVt2dRecord(i, stat_docids[i]);
	}
	
	return true;
}


bool
AosVt2dQryRsltProcNorm::appendValue(
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
	aos_assert_r(value_len == mValueMapper.mInputDef.mValueSize, false);
	
	bool rslt;
	AosVt2dRecord *vt2d_rcd = qry_rslt->getVt2dRecordRaw(stat_doc_idx, stat_docid);	
	aos_assert_r(vt2d_rcd, false);
	
	bool is_valid;
	rslt = mValueMapper.getOutputData(rdata, value, value_len,
			mTmpOutputData, mValueMapper.mOutputDef.mValueSize, is_valid);
	aos_assert_r(rslt, false);
	if(!is_valid)	return true;
	
	vt2d_rcd->appendValue(time_value, mTmpOutputData,
				mValueMapper.mOutputDef.mValueSize);
	
	return true;
}


/*
bool
AosVt2dQryRsltProcNorm::appendValue(
		const AosRundataPtr &rdata,
		AosVt2dQryRslt * qry_rslt,
		const u64 stat_docid,
		const AosStatTimeArea &time_area,
		char *bucket,
		const u64 bucket_len,
		const AosMeasureValueMapper &value_mapper,
		const AosMeasureValueProcer &value_procer)
{
	aos_assert_r(qry_rslt, false);
	aos_assert_r(time_area.start_time >=0 && time_area.end_time >=0, false);

	bool rslt;
	AosVt2dRecord *rcd = qry_rslt->getRcdRawByStatId(stat_docid, true);	
	aos_assert_r(rcd, false);

	char * output_data = OmnNew char[value_mapper.mOutputDef.mValueSize]; 

	int entry_off = -1;
	u64 crt_idx = 0;
	u64 crt_time = time_area.start_time;
	for(; crt_time <= (u64)time_area.end_time; crt_time++, crt_idx++)
	{
		entry_off = crt_idx * value_mapper.mInputDef.mValueSize;
	
		rslt = value_mapper.getOutputData(rdata,
				bucket + entry_off, value_mapper.mInputDef.mValueSize,
				output_data, value_mapper.mOutputDef.mValueSize);
		aos_assert_rr(rslt, rdata, false);
		
		rcd->appendValue(crt_time_value, output_data,
				value_mapper.mOutputDef.mValueSize);
	}
	
	delete output_data[];
	return true;
}
*/

