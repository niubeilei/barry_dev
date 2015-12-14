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
#include "ValueSel/ValueSelSecond.h"

#include "Debug/Except.h"
#include "Debug/Debug.h"
#include "Util/OmnNew.h"
#include "Alarm/Alarm.h"
#include "Util/DynArray.h"
#include "Util/UtUtil.h"
#include "XmlUtil/XmlTag.h"
#include "XmlUtil/XmlDoc.h"
#if 0

AosValueSelSecond::AosValueSelSecond(const bool reg)
:
AosValueSel(AOSACTOPRID_SECOND, AosValueSelType::eSecond, reg)
{
}


AosValueSelSecond::AosValueSelSecond(
        const AosXmlTagPtr &sdoc,
		const AosRundataPtr &rdata)
:
AosValueSel(AOSACTOPRID_SECOND, AosValueSelType::eSecond, false)
{
}


AosValueSelSecond::~AosValueSelSecond()
{
}


bool
AosValueSelSecond::run(
		AosValueRslt    &valueRslt,
		const AosXmlTagPtr &item,
		const AosRundataPtr &rdata)
{

	//	<valuesel AOSTAG_VALUE_TYPE="second" 
	//		format="xxx" 
	//		timezone="xxx" 
	//		AOSTAG_DATA_TYPE="xxx"/>
	// If timezone not specified, If not specified, the server's local time is used.
	
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
	OmnString sec;
	//sec: 0,1...11,13
	
	time_t tt = time(0);
	struct tm *theTime = localtime(&tt);
	sec<< theTime->tm_sec;
	aos_assert_r(sec!="", false);

	int format = item->getAttrInt("format", 1);
	aos_assert_r(format != 0, false);
	switch (format)
	{
	case 2:
		 //The second with leading zeros as needed, starting from 0.
		 if (atoll(sec.data())/10 == 0)
			value << "0" <<sec;
		 else
			value << sec;
		 break;

	case 1:
	default:
		 //The second with no leading zeros, starting from 0.
		 value << sec;
		 break;
	}
	return valueRslt.setStr(value);
}


OmnString 
AosValueSelSecond::getXmlStr(
		const OmnString &tagname, 
		const int level,
		const AosRandomRulePtr &rule, 
		const AosRundataPtr &rdata)
{
	OmnNotImplementedYet;
	return "";
}


AosValueSelObjPtr
AosValueSelSecond::clone(const AosXmlTagPtr &sdoc, const AosRundataPtr &rdata)
{
	try
	{
		return OmnNew AosValueSelSecond(sdoc, rdata);
	}

	catch (...)
	{
		AosSetErrorU(rdata, "failed_creating_obj") << enderr;
		return 0;
	}
}






#endif
