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
// This is a facility to generate a random string.
//
// 1. Randomly determine the string length.
// 2. There is a charset. Each allowed character in the charset has a
//    weight. It will randomly select a character from the charset 
//    based on the weights.
//
// Modification History:
// 11/01/2011	Created by Linda Lin 
////////////////////////////////////////////////////////////////////////////
#include "DataGenUtil/DataGenStringRand.h"

#include "Rundata/Rundata.h"
#include "Random/RandomUtil.h"
#include "Util/String.h"
#include "Util/StrSplit.h"
#include "ValueSel/ValueRslt.h"

AosDataGenStringRand::AosDataGenStringRand(const bool reg)
:
AosDataGenUtil(AOSDATGGENTYPE_STRING_RAND, AosDataGenUtilType::eStringRand, reg)
{
}

AosDataGenStringRand::AosDataGenStringRand(const AosXmlTagPtr &config, const AosRundataPtr &rdata)
:
AosDataGenUtil(AOSDATGGENTYPE_STRING_RAND, AosDataGenUtilType::eStringRand, false)
{
	aos_assert(parse(config, rdata));
}

AosDataGenStringRand::AosDataGenStringRand()
:
AosDataGenUtil(AOSDATGGENTYPE_STRING_RAND, AosDataGenUtilType::eStringRand, false)
{
}

AosDataGenStringRand::~AosDataGenStringRand()
{
}

bool
AosDataGenStringRand::nextValue(
		AosValueRslt &value,
		const AosXmlTagPtr &sdoc, 
		const AosRundataPtr &rdata)
{
	// This function randomly generates a string. The string is made from
	// a charset. The sdoc has a randomizer. It determines the string length.
	// 1. Use the randomizer to determine the string length.
	// 2. For each character, randomly pick one from the character set.
	//
	//	<sdoc zky_datagen_type = "xxx" 
	//		zky_repeatable ="true|false" 
	//		zky_charset = "a,b">	
	//		<length .../>
	//	</sdoc>
	//
	//	or 
	//	<sdoc zky_datagen_type = "xxx">
	//		<zky_length>
	//		...
	//		</zky_length>
	//		<zky_charselector>
	//		...
	//		</zky_charselector>
	//	</sdoc>	
	//
	// Note that this class generates ASCII strings only.
	//value.reset();	
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

	u64 len = sdoc->getAttrU64("zky_length", 0);
	if (len == 0)
	{
		AosXmlTagPtr tag = sdoc->getFirstChild("zky_length");
		AosValueRslt valueRslt;
		aos_assert_r(tag, false);
		if (AosDataGenUtil::nextValueStatic(valueRslt, tag, rdata))
		{
			if (!valueRslt.getU64()) return false;
		}
	}

	if (len == 0)
	{
		value.setStr("");
		return true;
	}
	aos_assert_rr(len, rdata, false);

	OmnString charset = sdoc->getAttrStr("zky_charset", "");
	if (charset != "")
	{
		// A character is defined as [a, b], where a and b are integers, a <= b. 
		// 'a' and 'b' are both ASCII code.
		AosStrSplit split;
		OmnString parts[2];
		split.splitStr(charset, ",", parts, 2);
		aos_assert_r(parts[0] != "" && parts[1] != "", false);
		int a = (int)parts[0].data()[0];
		int b = (int)parts[1].data()[0];
		if (a > b) 
		{
			AosSetErrorUser(rdata, "error") << "a > b!" << enderr;
			return false;
		}	
		char data[len+1];
		int setlen = (b - a + 1);
		for (u64 i=0; i<len; i++)
		{
			int idx = rand() % setlen;
			data[i] = a + idx;
		}
		data[len] = 0;
		OmnString vv(data);
		value.setStr(vv);
	}
	else
	{
		AosXmlTagPtr charselector = sdoc->getFirstChild("zky_charselector");
		if (!charselector)
		{
			AosSetErrorUser(rdata, "missing_charselector") << enderr;
			return false;
		}
		char data[len+1];
		for (u64 i = 0; i < len; i++)
		{
			AosValueRslt valueRslt;
			if (AosDataGenUtil::nextValueStatic(valueRslt, charselector, rdata))
			{
				data[i] = valueRslt.getChar();
			}
		}
		data[len] = 0;
		OmnString vv(data);
		value.setStr(vv);
	}
	return true;
}


bool
AosDataGenStringRand::nextValue(AosValueRslt &value, const AosRundataPtr &rdata)
{
	AosValueRslt vv;
	aos_assert_r(mLengthSelector, false);
	bool rslt = mLengthSelector->nextValue(vv, rdata);
	aos_assert_r(rslt, false);
	int len = vv.getI64();
	aos_assert_r(len >= 0, false);

	if (len == 0) 
	{
		value.setStr("");
		return true;
	}

	aos_assert_r(mCharSelector, false);
	vv.reset();
	char buff[len +1];
	for (int i = 0; i<len; i++)
	{
		rslt = mCharSelector->nextValue(vv, rdata);
		aos_assert_r(rslt, false);
		buff[i] = vv.getChar();
	}
	buff[len] = '\0';
	OmnString vvv(buff);
	value.setStr(vvv);
	return true;
}


AosDataGenUtilPtr 
AosDataGenStringRand::clone(const AosXmlTagPtr &config, const AosRundataPtr &rdata)
{
	return OmnNew AosDataGenStringRand(config, rdata);
}

bool
AosDataGenStringRand::parse(const AosXmlTagPtr &config, const AosRundataPtr &rdata)
{
	//<zky_datagen zky_datagen_type="stringrand">
	// <zky_length>
	//	 	<xxx zky_min = "xx" zky_max ="xxx" zky_weights="xxx"/>
	// 		<xxx zky_min = "xx" zky_max ="xxx" zky_weights="xxx"/>
	//	 	<xxx zky_min = "xx" zky_max ="xxx" zky_weights="xxx"/>
	//	 	...
	// </zky_length>
	// <zky_charselector>
	// 		<xxx zky_min = "xxx" zky_max ="xxx" zky_weights = "xxx"/>
	// 		...
	// </zky_charselector>
	// </zky_datagen>
	aos_assert_r(config, false);
	AosXmlTagPtr lengthselector = config->getFirstChild("zky_length");
	aos_assert_r(lengthselector, false);
	mLengthSelector = AosDataGenUtil::getDataGenUtilStatic(lengthselector, rdata);
	AosXmlTagPtr charselector = config->getFirstChild("zky_charselector"); 
	aos_assert_r(charselector, false);
	mCharSelector = AosDataGenUtil::getDataGenUtilStatic(charselector, rdata);
	return true;
}
