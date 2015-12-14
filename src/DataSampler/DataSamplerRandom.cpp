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
#include "DataSampler/DataSamplerRandom.h"
#include "XmlUtil/XmlTag.h"
#include "Rundata/Rundata.h"
#include "Util/BuffArray.h"


AosDataSamplerRandom::AosDataSamplerRandom(const bool flag)
:
AosDataSampler(AosDataSamplerType::eRandom)
{
}


AosDataSamplerRandom::AosDataSamplerRandom(
		const AosXmlTagPtr &conf, 
		const AosRundataPtr &rdata)
:
AosDataSampler(AosDataSamplerType::eRandom)
{
	aos_assert(config(conf, rdata));
}


AosDataSampler::~AosDataSampler()
{
}


bool
AosDataSamplerRandom::config(const AosXmlTagPtr &def, const AosRundataPtr &rdata)
{
	//<sampler type="random">
	//...
	//</sampler>
	//mNum = def->getAttrInt("zky_num", 0);
	//aos_assert_rr(mNum != 0, rdata, false);
	return true;
}

bool 
AosDataSamplerRandom::createSampler(
				const AosBuffArrayPtr &orig_array,
				AosBuffArrayPtr &result_array,
				const AosRundataPtr &rdata)
{
	/*
	u32 orig_num = orig_array->getNumEntries();
	aos_assert_rr(mNum <= orig_num, rdata, false);
	int rcd_len = orig_array->getRecordLen();
	result_array = orig_array;
	for(u32 i=0; i<mNum; i++)
	{
		char *rcddata = orig_array->getRecord(i, rcd_len);
		result_array->addValue(rcddata, rcd_len, rdata);
	}
	result_array->sort();
	*/
	return true;
}


AosDataSamplerObjPtr 
AosDataSamplerRandom::clone()
{
	OmnNotImplementedYet;
	return 0;
}

AosDataSamplerObjPtr 
AosDataSamplerRandom::clone(
		const AosXmlTagPtr &conf,
		const AosRundataPtr &rdata)
{
	try
	{
		return OmnNew AosDataSamplerRandom(conf, rdata);
	}
	catch (...)
	{
		OmnAlarm << "Failed createing object: " << conf->toString() << enderr;
		return 0;
	}
}

bool 
AosDataSamplerRandom::serializeTo(
				const AosBuffPtr &buff,
				const AosRundataPtr &rdata)
{
	return true;
}

bool 
AosDataSamplerRandom::serializeFrom(
				const AosBuffPtr &buff,
				const AosRundataPtr &rdata)
{
	return true;
}

