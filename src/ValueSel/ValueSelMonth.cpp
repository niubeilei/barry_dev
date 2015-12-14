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
#include "ValueSel/ValueSelMonth.h"

#include "Debug/Except.h"
#include "Debug/Debug.h"
#include "Util/OmnNew.h"
#include "Alarm/Alarm.h"
#include "Util/DynArray.h"
#include "Util/UtUtil.h"
#include "XmlUtil/XmlTag.h"
#include "XmlUtil/XmlDoc.h"

AosValueSelMonth::AosValueSelMonth(const bool reg)
:
AosValueSel(AOSACTOPRID_MONTH, AosValueSelType::eMonth, reg)
{
}


AosValueSelMonth::AosValueSelMonth(
        const AosXmlTagPtr &sdoc,
		const AosRundataPtr &rdata)
:
AosValueSel(AOSACTOPRID_MONTH, AosValueSelType::eMonth, false)
{
}


AosValueSelMonth::~AosValueSelMonth()
{
}


bool
AosValueSelMonth::run(
		AosValueRslt    &valueRslt,
		const AosXmlTagPtr &item,
		const AosRundataPtr &rdata)
{
	//	<valuesel AOSTAG_VALUE_TYPE="month" 
	//		format="xxx" 
	//		timezone="xxx"/>
	// If timezone not specified, it defaults to the sytem local time.
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

	OmnString value;
	OmnString month = AosGetSystemMonth();//month:1,2...11
	aos_assert_r(month != "", false);

	int format = item->getAttrInt("format", 1);
	aos_assert_r(format !=0 , false);
	switch (format)
	{
	case 2:
		 //The month number with leading zeros as needed, starting from 1.
		 if (atoll(month.data())/10 == 0)
			value << "0" <<month;
		 else
			value << month;
		 break;

	case 3:
		 //Short form month names, such as Jan., Feb., March, April, ... 
		 //The language is determined by the user's locale.
		 value = "";
		 break;

	case 4:
		 //Long form month names, such as January, February, 
		 //March, April, May, etc. The language is determined by the user's locale. 
		 //If there are no long form for the language, it is the same as 3 (such as Chinese)
		 value="";
		 break;

	case 1:
	default:
		 //The month number with leading zeros as needed, starting from 1.
		 value << month;
		 break;
	}
	valueRslt.setStr(value);
	return true;
}


OmnString 
AosValueSelMonth::getXmlStr(
		const OmnString &tagname, 
		const int level,
		const AosRandomRulePtr &rule, 
		const AosRundataPtr &rdata)
{
	OmnNotImplementedYet;
	return "";
}


AosValueSelObjPtr
AosValueSelMonth::clone(const AosXmlTagPtr &sdoc, const AosRundataPtr &rdata)
{
	try
	{
		return OmnNew AosValueSelMonth(sdoc, rdata);
	}

	catch (...)
	{
		AosSetErrorUser(rdata, "failed_creating_obj") << enderr;
		return 0;
	}
}

