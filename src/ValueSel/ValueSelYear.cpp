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
#include "ValueSel/ValueSelYear.h"

#include "Debug/Except.h"
#include "Debug/Debug.h"
#include "Util/OmnNew.h"
#include "Alarm/Alarm.h"
#include "Util/DynArray.h"
#include "Util/UtUtil.h"
#include "XmlUtil/XmlDoc.h"
#include "XmlUtil/XmlTag.h"

AosValueSelYear::AosValueSelYear(const bool reg)
:
AosValueSel(AOSACTOPRID_YEAR, AosValueSelType::eYear, reg)
{
}


AosValueSelYear::AosValueSelYear(
        const AosXmlTagPtr &sdoc,
		const AosRundataPtr &rdata)
:
AosValueSel(AOSACTOPRID_YEAR, AosValueSelType::eYear, false)
{
}


AosValueSelYear::~AosValueSelYear()
{
}


bool
AosValueSelYear::run(
		AosValueRslt    &valueRslt,
		const AosXmlTagPtr &item,
		const AosRundataPtr &rdata)
{
	//	<name AOSTAG_VALUE_TYPE="year" 
	//		format="xxx" 
	//		timezone="xxx" 
	//		AOSTAG_DATA_TYPE ="xxx"/>
	// If timezone not specified, it defaults to the system local time.
	aos_assert_rr(item, rdata, false);
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

	int format = item->getAttrInt("format", 2);
	aos_assert_r(format != 0, false);
	OmnString year; 
	switch(format)
	{
	case 1:
		 //Only the last two digits, such as 11 (for 2011).
		 year << atoll(AosGetSystemYear().data())%100;
		 break;

	case 2:
	default:
		 //The four digits, such as 2011.
		 year = AosGetSystemYear();
		 break;
	}
	valueRslt.setStr(year);
	return true;
}


OmnString 
AosValueSelYear::getXmlStr(
		const OmnString &tagname, 
		const int level,
		const AosRandomRulePtr &rule, 
		const AosRundataPtr &rdata)
{
	OmnNotImplementedYet;
	return "";
}


AosValueSelObjPtr
AosValueSelYear::clone(const AosXmlTagPtr &sdoc, const AosRundataPtr &rdata)
{
	try
	{
		return OmnNew AosValueSelYear(sdoc, rdata);
	}

	catch (...)
	{
		AosSetErrorU(rdata, "failed_creating_obj") << enderr;
		return 0;
	}
}

