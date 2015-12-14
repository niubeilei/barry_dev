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
#include "DataGenUtil/DataGenEnumRand.h"
#include "Util/String.h"
#include "Rundata/Rundata.h"
#include "ValueSel/ValueSel.h"

AosDataGenEnumRand::AosDataGenEnumRand(const bool reg)
:
AosDataGenUtil(AOSDATGGENTYPE_ENUM_RAND, AosDataGenUtilType::eEnumRand, reg)
{
}

AosDataGenEnumRand::AosDataGenEnumRand(const AosXmlTagPtr &config, const AosRundataPtr &rdata)
:
AosDataGenUtil(AOSDATGGENTYPE_ENUM_RAND, AosDataGenUtilType::eEnumRand, false)
{
	aos_assert(parse(config, rdata));
}

AosDataGenEnumRand::AosDataGenEnumRand()
:
AosDataGenUtil(AOSDATGGENTYPE_ENUM_RAND, AosDataGenUtilType::eEnumRand, false)
{
}

AosDataGenEnumRand::~AosDataGenEnumRand()
{
}

bool
AosDataGenEnumRand::nextValue(
		AosValueRslt &value,
		const AosXmlTagPtr &sdoc, 
		const AosRundataPtr &rdata)
{
	//<xxx zky_datagen_type="xxx" zky_repeatable = "true|false" AOSTAG_DATA_TYPE="">	
	//<dataset>
	//      <entry …/>
	//      <entry …/>
	//      <entry …/>
	//      <entry …/>
	//</dataset>
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

	AosXmlTagPtr entries = sdoc->getFirstChild();
	if (!entries) return false; 
	
	int num = entries->getNumSubtags();
	aos_assert_r(num, false);
	int idx = rand() % num;
	AosXmlTagPtr entry = entries->getChild(idx);
	aos_assert_r(entry, false);

	return AosValueSel::getValueStatic(value, entry, rdata);
}


bool 
AosDataGenEnumRand::nextValue(AosValueRslt &value, const AosRundataPtr &rdata)
{
	aos_assert_r(mValue.size() >0, false);
	int idx = rand() % mValue.size();
	value.setStr(mValue[idx]);
	return true;
}

AosDataGenUtilPtr 
AosDataGenEnumRand::clone(const AosXmlTagPtr &config, const AosRundataPtr &rdata)
{
	return OmnNew AosDataGenEnumRand(config, rdata);
}

bool
AosDataGenEnumRand::parse(const AosXmlTagPtr &config, const AosRundataPtr &rdata)
{
	aos_assert_r(config, false);
	mValue.clear();
	AosXmlTagPtr entries = config->getFirstChild();
	if (!entries) return false;

	AosXmlTagPtr entry = entries ->getFirstChild();
	OmnString ss;
	while (entry)
	{
		AosValueRslt value;
		if (AosValueSel::getValueStatic(value, entry, rdata))
		{
			ss = value.getStr();
			mValue.push_back(ss);
		}
		entry = entries->getNextChild();
	}
	return true;
}
