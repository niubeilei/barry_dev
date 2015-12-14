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
// 07/23/2012 Created by Felicia
////////////////////////////////////////////////////////////////////////////
#include "SEInterfaces/DataSamplerObj.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "Rundata/Rundata.h"
#include "SEInterfaces/DataSamplerCreatorObj.h"
#include "XmlUtil/XmlTag.h"


AosDataSamplerCreatorObjPtr AosDataSamplerObj::smCreator;


AosDataSamplerObjPtr 
AosDataSamplerObj::createDataSampler(const AosXmlTagPtr &def, const AosRundataPtr &rdata)
{
	aos_assert_rr(smCreator, rdata, NULL);
	return smCreator->createDataSampler(def, rdata);
}
	

AosDataSamplerObjPtr 
AosDataSamplerObj::serializeFromStatic(const AosBuffPtr &def, const AosRundataPtr &rdata)
{
	aos_assert_rr(smCreator, rdata, NULL);
	return smCreator->serializeFrom(def, rdata);
}


bool
AosDataSamplerObj::registerDataSampler(const OmnString &name, const AosDataSamplerObjPtr &cacher)
{
	aos_assert_r(smCreator, false);
	return smCreator->registerDataSampler(name, cacher);
}

bool
AosDataSamplerObj::distributeData(
		vector<AosBuffArrayPtr> &buckets,
		const AosBuffArrayPtr &orig_array,
		const AosBuffArrayPtr &result_array,
		const AosRundataPtr &rdata)
{
	u32 num = result_array->getNumEntries();
	u32 num_section = num + 1;
	for(u32 i=0; i<num_section; i++)
	{
		AosBuffArrayPtr b = orig_array->clone();
		buckets.push_back(b);
	}
	u64 num_records = orig_array->getNumEntries();
	int rcd_len = orig_array->getRecordLen();
	u64 idx = 0;
	while(idx < num_records)
	{
		char* rcd = orig_array->getEntry(idx);
		int i = result_array->findBoundary(rcd, false);
		if (i<0)
		{
			i = num;
		}
		AosBuffArrayPtr b = buckets.at(i);
		b->appendEntry(rcd, rcd_len, rdata.getPtr());
		idx++;
	}
	return true;
}




