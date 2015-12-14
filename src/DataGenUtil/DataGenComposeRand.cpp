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
// 12/01/2011	Created by Linda Lin 
////////////////////////////////////////////////////////////////////////////
#include "DataGenUtil/DataGenComposeRand.h"
#include "Util/String.h"
#include "Rundata/Rundata.h"
#include "ValueSel/ValueSel.h"

AosDataGenComposeRand::AosDataGenComposeRand(const bool reg)
:
AosDataGenUtil(AOSDATGGENTYPE_COMPOSE_RAND, AosDataGenUtilType::eComposeRand, reg)
{
}

AosDataGenComposeRand::AosDataGenComposeRand(const AosXmlTagPtr &config, const AosRundataPtr &rdata)
:
AosDataGenUtil(AOSDATGGENTYPE_COMPOSE_RAND, AosDataGenUtilType::eComposeRand, false)
{
	aos_assert(parse(config, rdata));
}

AosDataGenComposeRand::AosDataGenComposeRand()
:
AosDataGenUtil(AOSDATGGENTYPE_COMPOSE_RAND, AosDataGenUtilType::eComposeRand, false)
{
}

AosDataGenComposeRand::~AosDataGenComposeRand()
{
}

bool
AosDataGenComposeRand::nextValue(
		AosValueRslt &value,
		const AosXmlTagPtr &sdoc, 
		const AosRundataPtr &rdata)
{
	//<xxx zky_datagen_type="xxx"  AOSTAG_DATA_TYPE="">	
	//</datagens>
	//	<datagen/>
	//	<datagen/>
	//	<datagen/>
	//</datagens>
	//</xxx>
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

	OmnString vv = "";
	AosXmlTagPtr datagens = sdoc->getFirstChild();
	if (!datagens) return false; 
	AosXmlTagPtr datagen = datagens->getFirstChild();
	while(datagen)	
	{
		AosValueRslt valueRslt;
		if (AosDataGenUtil::nextValueStatic(valueRslt, datagen, rdata))
		{
			OmnString ss = valueRslt.getStr();
			vv << ss;
		}
		datagen = datagens->getNextChild();
	}

	value.setStr(vv);	
	return true;
}


bool 
AosDataGenComposeRand::nextValue(AosValueRslt &value, const AosRundataPtr &rdata)
{
	if (mDataGen.size() <= 0) 
	{
		value.setStr("");
		return true; 
	}
	OmnString vv = "";
	OmnString ss;
	for (u32 i = 0; i < mDataGen.size(); i++)
	{
		value.reset();
		mDataGen[i]->nextValue(value, rdata);
		ss = value.getStr();
		vv << ss;
	}
	value.setStr(vv);
	return true;
}

AosDataGenUtilPtr 
AosDataGenComposeRand::clone(const AosXmlTagPtr &config, const AosRundataPtr &rdata)
{
	return OmnNew AosDataGenComposeRand(config, rdata);
}

bool
AosDataGenComposeRand::parse(const AosXmlTagPtr &config, const AosRundataPtr &rdata)
{
	//<zky_datagen zky_datagen_type="compserand">
	//	<zky_datagen/>
	//	<zky_datagen/>
	//	<zky_datagen/>
	//	<zky_datagen/>
	//	<zky_datagen/>
	//</zky_datagen>
	aos_assert_r(config, false);
	mDataGen.clear();
	AosXmlTagPtr datagen = config->getFirstChild();
	if (!datagen) return false; 
	while(datagen)	
	{
		mDataGen.push_back(AosDataGenUtil::getDataGenUtilStatic(datagen, rdata));
		datagen = config->getNextChild();
	}
	return true;
}
