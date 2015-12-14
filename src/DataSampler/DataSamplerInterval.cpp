////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// Description:
//
// Modification History:
// 07/25/2012 Created by Jozhi
////////////////////////////////////////////////////////////////////////////
#include "DataSampler/DataSamplerInterval.h"
#include "XmlUtil/XmlTag.h"
#include "Rundata/Rundata.h"
#include "Util/BuffArray.h"


AosDataSamplerInterval::AosDataSamplerInterval(const bool flag)
:
AosDataSampler(AosDataSamplerType::eInterval),
mInterval(-1)
{
}


AosDataSamplerInterval::AosDataSamplerInterval(
		const AosXmlTagPtr &conf, 
		const AosRundataPtr &rdata)
:
AosDataSampler(AosDataSamplerType::eInterval),
mInterval(-1)
{
	aos_assert(config(conf, rdata));
}


AosDataSampler::~AosDataSampler()
{
}


bool
AosDataSamplerInterval::config(const AosXmlTagPtr &def, const AosRundataPtr &rdata)
{
	//<sampler type="interval" zky_interval="10000" zky_num="10000">
	//...
	//</sampler>
	mInterval = def->getAttrInt("zky_interval", 0);
	mNum = def->getAttrInt("zky_num", 0);
	return true;
}

bool 
AosDataSamplerInterval::createSampler(
				const AosBuffArrayPtr &orig_array,
				AosBuffArrayPtr &result_array,
				const AosRundataPtr &rdata)
{
	u32 orig_num = orig_array->getNumEntries();
	int rcd_len = orig_array->getRecordLen();
	result_array = orig_array->clone();
	if (mNum != 0)
	{
		u32 interval = orig_num / mNum;
		for(u32 i=0; i<mNum; i++)
		{
			u64 rcd_idx = i*interval;
			if (rcd_idx > orig_num)
			{
				break;
			}
			char *rcddata = orig_array->getRecord(rcd_idx, rcd_len);
			result_array->addValue(rcddata, rcd_len, rdata);
		}
	}
	else
	{
		u32 rslt_num = orig_num / mInterval;
		for(u32 i=0; i<rslt_num; i++)
		{
			u64 rcd_idx = i*mInterval;
			char *rcddata = orig_array->getRecord(rcd_idx, rcd_len);
			result_array->addValue(rcddata, rcd_len, rdata);
		}
	}
	result_array->sort();
	return true;
}


AosDataSamplerObjPtr 
AosDataSamplerInterval::clone()
{
	OmnNotImplementedYet;
	return 0;
}

AosDataSamplerObjPtr 
AosDataSamplerInterval::clone(
		const AosXmlTagPtr &conf,
		const AosRundataPtr &rdata)
{
	try
	{
		return OmnNew AosDataSamplerInterval(conf, rdata);
	}
	catch (...)
	{
		OmnAlarm << "Failed createing object: " << conf->toString() << enderr;
		return 0;
	}
}

bool 
AosDataSamplerInterval::serializeTo(
				const AosBuffPtr &buff,
				const AosRundataPtr &rdata)
{
	buff->setU32(mInterval);
	buff->setU32(mNum);
	return true;
}

bool 
AosDataSamplerInterval::serializeFrom(
				const AosBuffPtr &buff,
				const AosRundataPtr &rdata)
{
	mInterval = buff->getU32(0);
	mNum = buff->getU32(0);
	return true;
}

