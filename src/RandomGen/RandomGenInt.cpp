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
//
// Modification History:
// 2014/08/21 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "RandomGen/RandomGenInt.h"
#include "Rundata/Rundata.h"

extern "C"
{ 
	AosJimoPtr AosCreateJimoFunc_AosRandomGenInt_1(const AosRundataPtr &rdata, const int version)
	{
		try
		{
			OmnScreen << "To create Jimo: " << endl;
			AosJimoPtr jimo = OmnNew AosRandomGenInt(version);
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
AosRandomGenInt::AosRandomGenInt(const int version)
:AosRandomGen(version)

{
}


AosRandomGenInt::AosRandomGenInt(
		const AosRundataPtr &rdata, 
		const AosXmlTagPtr &conf)// confirm the config is right
:AosRandomGen(1)
{
	if (!config(rdata.getPtr(), conf))
	{
		OmnThrowException(rdata->getErrmsg());
		return;
	}
}


AosRandomGenInt::~AosRandomGenInt()
{
}


bool
AosRandomGenInt::config(
		AosRundata *rdata, 
		const AosXmlTagPtr &conf)//get the info min and max
{
	// 	<def min="0" max="100" weight="100" len="20"/>
   // AosXmlTagPtr tag=conf->getFirstChild("def");
	mMin = conf->getAttrInt("min", -1);
	aos_assert_rr(mMin >= 0, rdata, false);

	mMax = conf->getAttrInt("max", -1);
	aos_assert_rr(mMin <= mMax, rdata, false);

	mWeight = conf->getAttrInt("weight",-1);
	aos_assert_rr(mWeight>=0,  rdata,  false);

	mLen =    conf->getAttrInt("len",-1);
	aos_assert_rr(mLen>=0,  rdata,  false);
	return true;
}


bool 
AosRandomGenInt::generateData(
		AosRundata *rdata, 
		AosValueRslt &value_rslt)
{
	int number;
	number=OmnRandom::intByRange(mMin, mMax,mWeight,0,0,0);
	value_rslt.setValue(number);
	return true;
}
AosJimoPtr 
AosRandomGenInt::cloneJimo() const
{
	return OmnNew AosRandomGenInt(*this);
}
