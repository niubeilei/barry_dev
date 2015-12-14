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
// 09/15/2010	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "ValueSel/ValueSelRandom.h"

#include "Debug/Except.h"
#include "Debug/Debug.h"
#include "DataGenUtil/DataGenUtil.h"
#include "Util/OmnNew.h"
#include "Alarm/Alarm.h"
#include "Util/DynArray.h"
#include "Util/UtUtil.h"
#include "XmlUtil/XmlTag.h"
#include "XmlUtil/XmlDoc.h"

AosValueSelRandom::AosValueSelRandom(const bool reg)
:
AosValueSel(AOSACTOPRID_RAND, AosValueSelType::eRandom, reg)
{
}


AosValueSelRandom::AosValueSelRandom(
        const AosXmlTagPtr &sdoc,
		const AosRundataPtr &rdata)
:
AosValueSel(AOSACTOPRID_RAND, AosValueSelType::eRandom, false)
{
}


AosValueSelRandom::~AosValueSelRandom()
{
}


bool
AosValueSelRandom::run(
		AosValueRslt    &valueRslt,
		const AosXmlTagPtr &item,
		const AosRundataPtr &rdata)
{
	//<name AOSTAG_VALUE_TYPE="rand" zky_datagen_type="simplerand|enumrand|section" zky_min="xxx" zky_max="xxx" charset="xxx" AOSTAG_DATA_TYPE="xxx"/>
	aos_assert_r(item, false);
	AosDataType::E datatype = AosDataType::toEnum(item->getAttrStr(AOSTAG_DATA_TYPE));
	if (!AosDataType::isValid(datatype))
	{
		datatype = AosDataType::eString;
	}

	if (datatype == AosDataType::eXmlDoc)
	{
		rdata->setError() << "Datatype incorrect: " << datatype;
		return false;
	}

	/*
	OmnString value;
	
	u64 minlen = item->getAttrU64("minlen", 0);

	u64 maxlen = item->getAttrU64("maxlen", 0);
	
	int len;
	if (maxlen <= minlen)len = maxlen;
	else len = rand()%(maxlen-minlen)+ minlen+1;

	char data[100];
	AosRandomLetterStr(len, data);
	value = data;
	return valueRslt.setValue(datatype, value, rdata);
	*/
	return AosDataGenUtil::nextValueStatic(valueRslt, item, rdata);
}


OmnString 
AosValueSelRandom::getXmlStr(
		const OmnString &tagname, 
		const int level,
		const AosRandomRulePtr &rule, 
		const AosRundataPtr &rdata)
{
	OmnNotImplementedYet;
	return "";
}


AosValueSelObjPtr
AosValueSelRandom::clone(const AosXmlTagPtr &sdoc, const AosRundataPtr &rdata)
{
	try
	{
		return OmnNew AosValueSelRandom(sdoc, rdata);
	}

	catch (...)
	{
		AosSetErrorUser(rdata, "failed_creating_obj") << enderr;
		return 0;
	}
}







