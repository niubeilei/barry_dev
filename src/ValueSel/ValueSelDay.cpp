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
#include "ValueSel/ValueSelDay.h"

#include "Debug/Except.h"
#include "Debug/Debug.h"
#include "Util/OmnNew.h"
#include "Alarm/Alarm.h"
#include "Util/DynArray.h"
#include "Util/UtUtil.h"
#include "XmlUtil/XmlTag.h"
#include "XmlUtil/XmlDoc.h"
#if 0
AosValueSelDay::AosValueSelDay(const bool reg)
:
AosValueSel(AOSACTOPRID_DAY, AosValueSelType::eDay, reg)
{
}


AosValueSelDay::AosValueSelDay(
		const AosXmlTagPtr &sdoc,
		const AosRundataPtr &rdata)
:
AosValueSel(AOSACTOPRID_DAY, AosValueSelType::eDay, false)
{
}


AosValueSelDay::~AosValueSelDay()
{
}


bool
AosValueSelDay::run(
		AosValueRslt &valueRslt, 
		const AosXmlTagPtr &item,
		const AosRundataPtr &rdata)
{
	//	<valuesel AOSTAG_VALUE_TYPE="day" 
	//		format="xxx" 
	//		timezone="xxx"  
	//		AOSTAG_DATA_TYPE="xxx"/>
	// If timezone not specified, it defaults to the system local time.
	aos_assert_r(item, false);
	OmnString value;
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

	OmnString day = AosGetSystemDayOfMonth();
	//day:1,3.... 11,12
	aos_assert_r(day != "", false);

	int format = item->getAttrInt("format", 1);
	aos_assert_r(format != 0, false);
	switch (format)
	{
	case 2:
		 //The day number with leading zeros as needed, starting from 1.
		 if (atoll(day.data())/10 == 0)
			value << "0" <<day;
		 else
			value << day;
		 break;

	case 3:
		 //Short form day names, such as 1sd, 2cd, 3rd, 4th, ... 
		 //The language is determined by the user's locale.	
		 value = "";
		 break;

	case 1:
	default:
		 //The day number with no leading zeros, starting from 1.
		 value << day;
		 break;
	}
	return valueRslt.setStr(value);
}


OmnString 
AosValueSelDay::getXmlStr(
		const OmnString &tagname, 
		const int level,
		const AosRandomRulePtr &rule, 
		const AosRundataPtr &rdata)
{
	OmnNotImplementedYet;
	return "";
}


AosValueSelObjPtr
AosValueSelDay::clone(const AosXmlTagPtr &sdoc, const AosRundataPtr &rdata)
{
	try
	{
		return OmnNew AosValueSelDay(sdoc, rdata);
	}

	catch (...)
	{
		AosSetErrorUser(rdata, "failed_creating_obj") << enderr;
		return 0;
	}
}
#endif
