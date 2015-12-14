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
// This is a utility to select docs.
//
// Modification History:
// 11/01/2011	Created by Linda Lin 
////////////////////////////////////////////////////////////////////////////
#include "DataGenUtil/DataGenSimpleRand.h"
#include "Util/String.h"
#include "Rundata/Rundata.h"
//#include "RandomUtil/RandomUtil.h"

AosDataGenSimpleRand::AosDataGenSimpleRand(const bool reg)
:
AosDataGenUtil(AOSDATGGENTYPE_SIMPLE_RAND, AosDataGenUtilType::eSimpleRand, reg)
{
}

AosDataGenSimpleRand::AosDataGenSimpleRand(const AosXmlTagPtr &config, const AosRundataPtr &rdata)
:
AosDataGenUtil(AOSDATGGENTYPE_SIMPLE_RAND, AosDataGenUtilType::eSimpleRand, false),
mMin(0),
mMax(0)
{
	aos_assert(parse(config, rdata));
}

AosDataGenSimpleRand::AosDataGenSimpleRand()
:
AosDataGenUtil(AOSDATGGENTYPE_SIMPLE_RAND, AosDataGenUtilType::eSimpleRand, false)
{
}


AosDataGenSimpleRand::~AosDataGenSimpleRand()
{
}

bool
AosDataGenSimpleRand::nextValue(
		AosValueRslt &value,
		const AosXmlTagPtr &sdoc, 
		const AosRundataPtr &rdata)
{
	//<xxx zky_datagen_type = "xxx" zky_min = "xxx" zky_max="xxx" zky_repeatable ="true|false" >	
	//value.reset();
	aos_assert_r(sdoc, false);
	AosDataType::E datatype = AosDataType::toEnum(sdoc->getAttrStr(AOSTAG_DATA_TYPE));
	if (!AosDataType::isValid(datatype))
	{
		datatype = AosDataType::eString;
	}

	if (datatype == AosDataType::eXmlDoc)
	{
		AosSetErrorUser(rdata, "data_type_incorrect") << datatype << enderr;
		return false;
	}

	int min = sdoc->getAttrInt("zky_min", 0);
	int max = sdoc->getAttrInt("zky_max", 0);

	OmnString vv = RandomInteger(min, max);
	value.setStr(vv);
	return true;
}

OmnString
AosDataGenSimpleRand::RandomInteger(int min, int max)
{
	if (min > max) return "";
	OmnString value;
	int range = max - min + 1;
	value << min + random() % range;
	return value;
}


bool
AosDataGenSimpleRand::nextValue(AosValueRslt &value, const AosRundataPtr &rdata)
{
	OmnString vv = RandomInteger(mMin, mMax);
	value.setStr(vv);
	return true;
}


AosDataGenUtilPtr 
AosDataGenSimpleRand::clone(const AosXmlTagPtr &config, const AosRundataPtr &rdata)
{
	return OmnNew AosDataGenSimpleRand(config, rdata);
}

bool
AosDataGenSimpleRand::parse(const AosXmlTagPtr &config, const AosRundataPtr &rdata)
{
	aos_assert_r(config, false);
	mMin = config->getAttrInt("zky_min", 0);
	mMax = config->getAttrInt("zky_max", 0);
	return true;
}
