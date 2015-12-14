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
// 1. It may define ranges:
// 		[min, max, weight]
// 		[min, max, weight]
// 		...
// 		[min, max, weight]
//
// Modification History:
// 2014/08/21 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "RandomGen/RandomGenU64.h"
#include "Rundata/Rundata.h"

extern "C"
{ 
	AosJimoPtr AosCreateJimoFunc_AosRandomGenU64_1(const AosRundataPtr &rdata, const int version)
	{
		try
		{
			OmnScreen << "To create Jimo: " << endl;
			AosJimoPtr jimo = OmnNew AosRandomGenU64(version);
			aos_assert_r(jimo, 0);
			return jimo;
		} 
		catch (...)
		{
			AosSetErrorU(rdata, "Failed creating jimo") << enderr;
			return 0;
		}
				    
		OmnShouldNeverComeHere;
		return 0;
	}
}
AosRandomGenU64::AosRandomGenU64(const int version)
:
AosRandomGen(version)
{
}


AosRandomGenU64::AosRandomGenU64(
		const AosRundataPtr &rdata, 
		const AosXmlTagPtr &conf)// confirm the config is right
:
AosRandomGen(1)
{
	if (!config(rdata, conf))
	{
		OmnThrowException(rdata->getErrmsg());
		return;
	}
}


AosRandomGenU64::~AosRandomGenU64()
{
}


bool
AosRandomGenU64::config(
		const AosRundataPtr &rdata, 
		const AosXmlTagPtr &conf)//get the info min and max
{
	// 	<def min="0" max="100"/>

	mMin = conf->getAttrInt("min", -1);
	aos_assert_rr(mMin >= 0, rdata, false);

	mMax = conf->getAttrInt("max", -1);
	aos_assert_rr(mMin <= mMax, rdata, false);

	return true;
}


bool 
AosRandomGenU64::generateData(
		AosRundata *rdata, 
		AosValueRslt &value_rslt)
{
	//int number;
	//number=OmnRandom::intByRange(&mMin, &mMax,&mWeight,mLen);
	//value_rslt.setCharStr1(str.data(), str.length(), false);
	u64 number;
	number=OmnRandom::nextU64()%mMax+mMin;
	value_rslt.setValue(number);
	return true;
}

AosJimoPtr
AosRandomGenU64::cloneJimo() const
 {
	     return OmnNew AosRandomGenU64(*this);
 }
