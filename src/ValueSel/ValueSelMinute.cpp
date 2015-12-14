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
#include "ValueSel/ValueSelMinute.h"

#include "Debug/Except.h"
#include "Debug/Debug.h"
#include "Util/OmnNew.h"
#include "Alarm/Alarm.h"
#include "Util/DynArray.h"
#include "Util/UtUtil.h"
#include "XmlUtil/XmlTag.h"
#include "XmlUtil/XmlDoc.h"
#if 0

AosValueSelMinute::AosValueSelMinute(const bool reg)
:
AosValueSel(AOSACTOPRID_MINUTE, AosValueSelType::eMinute, reg)
{
}


AosValueSelMinute::AosValueSelMinute(
        const AosXmlTagPtr &sdoc,
		const AosRundataPtr &rdata)
:
AosValueSel(AOSACTOPRID_MINUTE, AosValueSelType::eMinute, false)
{
}


AosValueSelMinute::~AosValueSelMinute()
{
}


bool
AosValueSelMinute::run(
		AosValueRslt    &valueRslt,
		const AosXmlTagPtr &item,
		const AosRundataPtr &rdata)
{
	//	<name AOSTAG_VALUE_TYPE="minute" 
	//		format="xxx" 
	//		timezone="xxx" 
	//		AOSTAG_DATA_TYPE ="xxx"/>
	// If timezone is not specified, it defaults to the local time.
	
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

	OmnString min;
	OmnString value;
	//min: 0,1...11,13

	time_t tt = time(0);
	struct tm *theTime = localtime(&tt);
	min<< theTime->tm_min;
	aos_assert_r(min != "", false);

	int format = item->getAttrInt("format", 1);
	aos_assert_r(format != 0, false);
	switch (format)
	{
	case 2:
		 //The minute with leading zeros as needed, starting from 0.
		 if (atoll(min.data())/10 == 0)
			value << "0" <<min;
		 else
			value << min;
		 break;

	case 1:
	default:
		 //The minute with no leading zeros, starting from 0.
		 value <<min;
		 break;
	}
	return valueRslt.setStr(value);
}


OmnString 
AosValueSelMinute::getXmlStr(
		const OmnString &tagname, 
		const int level,
		const AosRandomRulePtr &rule, 
		const AosRundataPtr &rdata)
{
	OmnNotImplementedYet;
	return "";
}


AosValueSelObjPtr
AosValueSelMinute::clone(const AosXmlTagPtr &sdoc, const AosRundataPtr &rdata)
{
	try
	{
		return OmnNew AosValueSelMinute(sdoc, rdata);
	}

	catch (...)
	{
		AosSetErrorUser(rdata, "failed_creating_obj") << enderr;
		return 0;
	}
}
#endif
