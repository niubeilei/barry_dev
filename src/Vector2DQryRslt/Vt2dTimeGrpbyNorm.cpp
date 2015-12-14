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

#include "Vector2DQryRslt/Vt2dTimeGrpbyNorm.h"

#include "Vector2DQryRslt/Vt2dTimeGrpbyNormRslt.h"

AosVt2dTimeGrpbyNorm::AosVt2dTimeGrpbyNorm()
{
}

AosVt2dTimeGrpbyNorm::AosVt2dTimeGrpbyNorm(
		const AosStatTimeUnit::E grpby_time_unit,
		const u32 vt2d_idx,
		const AosMeasureValueMapper &value_mapper)
:
AosVt2dTimeGrpby(vt2d_idx, value_mapper),
mGrpbyTimeUnit(grpby_time_unit)
{
}


AosVt2dTimeGrpbyNorm::~AosVt2dTimeGrpbyNorm()
{
}


AosVt2dQryRsltPtr
AosVt2dTimeGrpbyNorm::createQryRslt()
{
	u32 measure_value_size = mValueMapper.mOutputDef.mValueSize;
	return OmnNew AosVt2dTimeGrpbyNormRslt(measure_value_size);
}


bool
AosVt2dTimeGrpbyNorm::config(
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &conf)
{
	aos_assert_r(conf, false);

	AosXmlTagPtr base_conf = conf->getFirstChild("vt2d_qry_rslt_base", false);
	aos_assert_r(base_conf, false);
	bool rslt = AosVt2dQryRsltProc::config(rdata, base_conf);
	aos_assert_r(rslt, false);

	AosXmlTagPtr time_unit_conf = conf->getFirstChild("grpby_time_unit");
	aos_assert_r(time_unit_conf, false);
	OmnString unit_str = time_unit_conf->getAttrStr("unit", "");
	mGrpbyTimeUnit = AosStatTimeUnit::getTimeUnit(unit_str);

	return true;
}


OmnString
AosVt2dTimeGrpbyNorm::toXmlString(const AosRundataPtr &rdata)
{
	OmnString str = "<vt2d_qry_rslt_proc type=\"time_grpby_norm\" >";
	str << AosVt2dQryRsltProc::toXmlString(rdata);
	str << "<grpby_time_unit unit=\"" << AosStatTimeUnit::toStr(mGrpbyTimeUnit) << "\" />";

	str << "</vt2d_qry_rslt_proc>"; 
	return str;
}


bool
AosVt2dTimeGrpbyNorm::appendVt2dRecords(
		AosRundata *rdata,
		AosVt2dQryRslt * qry_rslt,
		vector<u64> &stat_docids,
		vector<AosStatTimeArea> &time_areas)
{
	AosVt2dTimeGrpbyNormRslt * grpby_norm_rslt = 0;
	grpby_norm_rslt = dynamic_cast<AosVt2dTimeGrpbyNormRslt *>(qry_rslt);
	aos_assert_r(grpby_norm_rslt, false);

	u32 crt_timeidx = 0;
	i64 crt_time = -1, crt_grpby_time = -1;
	for(u32 i=0; i <time_areas.size(); i++)
	{
		crt_time = time_areas[i].start_time;	
		aos_assert_r(crt_time >=0 && time_areas[i].end_time >=0, false);

		for(; crt_time <= time_areas[i].end_time; crt_time++, crt_timeidx++)
		{
			crt_grpby_time = AosStatTimeUnit::parseTimeValue(crt_time,
				time_areas[i].time_unit, mGrpbyTimeUnit);
			aos_assert_r(crt_grpby_time >=0, false);
			
			grpby_norm_rslt->appendGrpbyTimeValue(crt_timeidx, crt_time, crt_grpby_time);
		}
	}
	
	qry_rslt->setMaxTimeValueNum(grpby_norm_rslt->getValueNums());
	for(u32 i=0; i<stat_docids.size(); i++)
	{
		grpby_norm_rslt->appendVt2dRecord(i, stat_docids[i]);
	}
	return true;
}


bool
AosVt2dTimeGrpbyNorm::appendValue(
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
	
	AosVt2dRecord *vt2d_rcd = qry_rslt->getVt2dRecordRaw(stat_doc_idx, stat_docid);
	aos_assert_r(vt2d_rcd, false);

	AosVt2dTimeGrpbyNormRslt * grpby_norm_rslt = 0;
	grpby_norm_rslt = dynamic_cast<AosVt2dTimeGrpbyNormRslt *>(qry_rslt);
	aos_assert_r(grpby_norm_rslt, false);

	i64 grpby_time_value;
	bool rslt = grpby_norm_rslt->getGrpbyTimeValue(time_idx, time_value, grpby_time_value);
	aos_assert_r(rslt && grpby_time_value >=0, false);

	rslt = appendValueToRcd(rdata, vt2d_rcd, grpby_time_value, value, value_len);
	aos_assert_r(rslt, false);

	return true;
}


