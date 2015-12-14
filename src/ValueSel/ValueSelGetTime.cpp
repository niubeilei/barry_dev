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
// 09/27/2011	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#if 0
#include "ValueSel/ValueSelGetTime.h"

#include "Debug/Except.h"
#include "Debug/Debug.h"
#include "Util/OmnNew.h"
#include "Alarm/Alarm.h"
#include "Util/DynArray.h"
#include "Util/UtUtil.h"
#include "XmlUtil/XmlDoc.h"
#include "XmlUtil/XmlTag.h"

AosValueSelGetTime::AosValueSelGetTime(const bool reg)
:
AosValueSel(AOSACTOPRID_GETTIME, AosValueSelType::eGetTime, reg)
{
}


AosValueSelGetTime::~AosValueSelGetTime()
{
}


bool
AosValueSelGetTime::run(
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
	AosValueRslt::Type datatype = AosValueRslt::toEnum(item->getAttrStr(AOSTAG_DATA_TYPE));
	if (!AosValueRslt::isValid(datatype))
	{
		datatype = AosValueRslt::eString;
	}

	if (datatype == AosValueRslt::eXmlDoc)
	{
		rdata->setError() << "Datatype incorrect: " << datatype;
		return false;
	}

	int format = item->getAttrInt("format", 2);
	OmnString year; 
	switch(format)
	{
	case 1:
		 year << atoll(AosGetSystemYear())%100;
		 break;

	case 2:
	default:
		 //The four digits, such as 2011.
		 year = AosGetSystemYear();
		 break;
	}
	return valueRslt.setValue(datatype, year, rdata);
}


AosValueSelPtr
AosValueSelGetTime::clone()
{
	AosValueSel *pp = (AosValueSel*) OmnNew AosValueSelGetTime(false);
	return pp;
}

bool 
AosValueSelGetTime::getValue(
		AosValueRslt &valueRslt, 
		const AosXmlTagPtr &obj,
		const AosRundataPtr &rdata)
{
	OmnNotImplementedYet;
	return false;
}

AosValueSelPtr 
AosValueSelGetTime::createValueSelector(
				const AosXmlTagPtr &def, 
				const AosRundataPtr &rdata)
{
	OmnNotImplementedYet;
	return 0;
}
#endif
