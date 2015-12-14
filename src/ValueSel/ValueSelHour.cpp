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
#include "ValueSel/ValueSelHour.h"

#include "Debug/Except.h"
#include "Debug/Debug.h"
#include "Util/OmnNew.h"
#include "Alarm/Alarm.h"
#include "Util/DynArray.h"
#include "Util/UtUtil.h"
#include "XmlUtil/XmlTag.h"
#include "XmlUtil/XmlDoc.h"
#if 0
AosValueSelHour::AosValueSelHour(const bool reg)
:
AosValueSel(AOSACTOPRID_HOUR, AosValueSelType::eHour, reg)
{
}


AosValueSelHour::AosValueSelHour(
        const AosXmlTagPtr &sdoc,
		const AosRundataPtr &rdata)
:
AosValueSel(AOSACTOPRID_HOUR, AosValueSelType::eHour, false)
{
}


AosValueSelHour::~AosValueSelHour()
{
}


bool
AosValueSelHour::run(
		AosValueRslt    &valueRslt,
		const AosXmlTagPtr &item,
		const AosRundataPtr &rdata)
{
	//	<valuesel AOSTAG_VALUE_TYPE="hour" 
	//		format="xxx" 
	//		timezone="xxx" 
	//		AOSTAG_DATA_TYPE="xxx"/>
	// If timezone not specified, it defaults to the system local time.
	
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
	OmnString hour;
	//hour: 0,1...11,13
	time_t tt = time(0);
	struct tm *theTime = localtime(&tt);
	hour<< theTime->tm_hour;
	aos_assert_r(hour != "",false); 

	int format = item->getAttrInt("format", 1);
	aos_assert_r(format !=0, false);
	switch (format)
	{
	case 2:
		 //The hour with leading zeros as needed, starting from 0.
		 if (atoll(hour.data())/10 == 0)
			value << "0" <<hour;
		 else
			value << hour;
		 break;

	case 1:
	default:
		 //The hour with no leading zeros, starting from 0.
		 value <<hour;
		 break;

	}
	return valueRslt.setValue(datatype, value, rdata.getPtrNoLock());
}


OmnString 
AosValueSelHour::getXmlStr(
		const OmnString &tagname, 
		const int level,
		const AosRandomRulePtr &rule, 
		const AosRundataPtr &rdata)
{
	OmnString xml = "<";
	xml << tagname << " ";

	XmlGenRuleValueIter itr;
	for (itr = rule->mAttrs_r.begin(); itr != rule->mAttrs_r.end(); ++itr)
	{
		xml << itr->first <<"=\"" << (itr->second).getRandomValue() << "\" ";
	}
	for (itr = rule->mAttrs_o.begin(); itr != rule->mAttrs_o.end(); ++itr)
	{
		if (rand()%10 < 8)
			xml << itr->first <<"=\"" << (itr->second).getRandomValue() << "\" ";
	}
	
	xml << "/>";
	return  xml;
}


AosValueSelObjPtr
AosValueSelHour::clone(const AosXmlTagPtr &sdoc, const AosRundataPtr &rdata)
{
	try
	{
		return OmnNew AosValueSelHour(sdoc, rdata);
	}

	catch (...)
	{
		AosSetErrorUser(rdata, "failed_creating_obj") << enderr;
		return 0;
	}
}
#endif
