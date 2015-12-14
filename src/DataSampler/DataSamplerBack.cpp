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
#include "DataSampler/DataSamplerBack.h"
#include "XmlUtil/XmlTag.h"
#include "Rundata/Rundata.h"
#include "Util/BuffArray.h"


AosDataSamplerBack::AosDataSamplerBack(const bool flag)
:
AosDataSampler(AosDataSamplerType::eBack)
{
}


AosDataSamplerBack::AosDataSamplerBack(
		const AosXmlTagPtr &conf, 
		const AosRundataPtr &rdata)
:
AosDataSampler(AosDataSamplerType::eBack)
{
	aos_assert(config(conf, rdata));
}


AosDataSampler::~AosDataSampler()
{
}


bool
AosDataSamplerBack::config(const AosXmlTagPtr &def, const AosRundataPtr &rdata)
{
	//<sampler type="front" zky_num="10000">
	//...
	//</sampler>
	mNum = def->getAttrInt("zky_num", 0);
	aos_assert_rr(mNum != 0, rdata, false);
	return true;
}

bool 
AosDataSamplerBack::createSampler(
				const AosBuffArrayPtr &orig_array,
				AosBuffArrayPtr &result_array,
				const AosRundataPtr &rdata)
{
	u32 orig_num = orig_array->getNumEntries();
	aos_assert_rr(mNum <= orig_num, rdata, false);
	int rcd_len = orig_array->getRecordLen();
	result_array = orig_array->clone();
	for(u32 i=orig_num-mNum; i<orig_num; i++)
	{
		char *rcddata = orig_array->getRecord(i, rcd_len);
		result_array->addValue(rcddata, rcd_len, rdata);
	}
	result_array->sort();
	return true;
}


AosDataSamplerObjPtr 
AosDataSamplerBack::clone()
{
	OmnNotImplementedYet;
	return 0;
}

AosDataSamplerObjPtr 
AosDataSamplerBack::clone(
		const AosXmlTagPtr &conf,
		const AosRundataPtr &rdata)
{
	try
	{
		return OmnNew AosDataSamplerBack(conf, rdata);
	}
	catch (...)
	{
		OmnAlarm << "Failed createing object: " << conf->toString() << enderr;
		return 0;
	}
}

bool 
AosDataSamplerBack::serializeTo(
				const AosBuffPtr &buff,
				const AosRundataPtr &rdata)
{
	buff->setInt(mNum);
	return true;
}

bool 
AosDataSamplerBack::serializeFrom(
				const AosBuffPtr &buff,
				const AosRundataPtr &rdata)
{
	mNum = buff->getInt(0);
	return true;
}

