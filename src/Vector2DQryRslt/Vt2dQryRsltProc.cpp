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

#include "Vector2DQryRslt/Vt2dQryRsltProc.h"

#include "Vector2DQryRslt/Vt2dQryRslt.h"
#include "Vector2DQryRslt/Vt2dQryRsltProcNorm.h"
#include "Vector2DQryRslt/Vt2dTimeGrpbyAll.h"
#include "Vector2DQryRslt/Vt2dTimeGrpbyNorm.h"

AosVt2dQryRsltProc::AosVt2dQryRsltProc()
{
}

AosVt2dQryRsltProc::AosVt2dQryRsltProc(
		const u32 vt2d_idx,
		const AosMeasureValueMapper &value_mapper)
:
mVt2dIdx(vt2d_idx),
mValueMapper(value_mapper)
{
	mTmpOutputData = OmnNew char[mValueMapper.mOutputDef.mValueSize]; 
}


AosVt2dQryRsltProc::~AosVt2dQryRsltProc()
{
	OmnDelete[] mTmpOutputData;
}


AosVt2dQryRsltProcPtr
AosVt2dQryRsltProc::createQryRsltProc(
		const u32 vt2d_idx,
		const AosStatTimeUnit::E &grpby_time_unit,
		const AosStatTimeUnit::E &vt2d_time_unit,
		const AosMeasureValueMapper &value_mapper)
{
	if(grpby_time_unit == AosStatTimeUnit::eInvalid)
	{
		return OmnNew AosVt2dTimeGrpbyAll(vt2d_idx, value_mapper);
	}
	
	if(grpby_time_unit == vt2d_time_unit)
	{
		return OmnNew AosVt2dQryRsltProcNorm(vt2d_idx, value_mapper);
	}
	
	return OmnNew AosVt2dTimeGrpbyNorm(grpby_time_unit, vt2d_idx, value_mapper);
}


bool
AosVt2dQryRsltProc::config(
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &conf)
{
	aos_assert_r(conf, false);
	
	mVt2dIdx = conf->getAttrU32("vt2d_idx", 0);
	
	AosXmlTagPtr mapper_conf = conf->getFirstChild("m_value_mapper");
	aos_assert_r(mapper_conf, false);
	bool rslt = mValueMapper.config(rdata, mapper_conf);
	aos_assert_r(rslt, false);

	aos_assert_r(mValueMapper.mOutputDef.mValueSize, false);
	mTmpOutputData = OmnNew char[mValueMapper.mOutputDef.mValueSize]; 
	return true;
}


OmnString
AosVt2dQryRsltProc::toXmlString(const AosRundataPtr &rdata)
{
	OmnString str = "<vt2d_qry_rslt_base ";
	str << "vt2d_idx=\"" << mVt2dIdx << "\" >"
		<< mValueMapper.toXmlString(rdata)
		<< "></vt2d_qry_rslt_base>";
	return str;
}


AosVt2dQryRsltProcPtr
AosVt2dQryRsltProc::createQryRsltProc(
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &conf)
{
	aos_assert_r(conf, 0);
	OmnString type = conf->getAttrStr("type", "");	
	aos_assert_r(type != "", 0);

	AosVt2dQryRsltProcPtr qry_rslt_proc;
	if(type == "qry_norm")
	{
		qry_rslt_proc = OmnNew AosVt2dQryRsltProcNorm();
	}
	else if (type == "time_grpby_norm")
	{
		qry_rslt_proc = OmnNew AosVt2dTimeGrpbyNorm();
	}
	else if (type == "time_grpby_all")
	{
		qry_rslt_proc = OmnNew AosVt2dTimeGrpbyAll();
	}
	else
	{
		OmnAlarm << "Invalid proc type:" << type << enderr;
		return 0;
	}
	
	bool rslt = qry_rslt_proc->config(rdata, conf);
	aos_assert_r(rslt, 0);
	return qry_rslt_proc;
}


