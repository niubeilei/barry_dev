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
//   
//
// Modification History:
// 09/03/2011	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "HtmlServer/HtmlOpr.h"

#include "alarm_c/alarm.h"
#include "Thread/Mutex.h"
#include "Util/HashUtil.h"

static AosStr2U32_t 		sgOprMap;

AosHtmlOpr gAosHtmlOpr;

AosHtmlOpr::AosHtmlOpr()
{
	addName("retrieveFullVpd", eRetrieveFullVpd);
	addName("retrieveVpd", eRetrieveVpd);
	addName("retrieveGic", eRetrieveGic);
	addName("retrieveCreator", eRetrieveCreator);
	addName("retrieveXmlCreator", eRetrieveXmlCreator);
	addName("simulate", eSimulate);
	addName("print", ePrint);
	addName("retrieveSite", eRetrieveSite);
	addName("serverError", eServerError);
}


AosHtmlOpr::E 
AosHtmlOpr::toEnum(const OmnString &id)
{
	if (id.length() < 1) return eInvalid;
	
	AosStr2U32Itr_t itr = sgOprMap.find(id);
	if (itr == sgOprMap.end()) return eInvalid;
	return (E) itr->second;
}


bool
AosHtmlOpr::addName(const OmnString &name, const E eid)
{
	aos_assert_r(name != "", false);

	E ee = toEnum(name);
	if (ee != eInvalid)
	{
		OmnAlarm << "SeRequest has already been added: " << enderr;
		return false;
	}

	sgOprMap[name] = eid;
	return true;
}

