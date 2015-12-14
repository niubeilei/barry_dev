////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// This class is used as the runtime for HTML code generation. 
//
// Modification History:
// 07/19/2010 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "HtmlModules/HtmlRuntime.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "XmlUtil/XmlTag.h"



AosHtmlRuntime::AosHtmlRuntime(const AosXmlTagPtr &vpd)
:
mVpd(vpd)
{
}


AosHtmlRuntime::~AosHtmlRuntime()
{
}


AosXmlTagPtr 
AosHtmlRuntime::getElemById(const OmnString &id)
{
	OmnNotImplementedYet;
	return 0;
}


OmnString
AosHtmlRuntime::getNewElemId()
{
	OmnNotImplementedYet;
	return "";
}

OmnString
AosHtmlRuntime::getElemId(const AosXmlTagPtr &elem)
{
	// This function checks whether the elemnt 'elem' already had an
	// ID. If yes, it returns that ID. Otherwise, it creates a new
	// ID and set the new ID to 'elem'.
	aos_assert_r(elem, "");
	OmnString id = elem->getAttrStr(AOSTAG_ELEMID);
	if (id != "") return id;

	id = getNewElemId();
	aos_assert_r(id != "", "");
	elem->setAttr(AOSTAG_ELEMID, id);
	return id;
}

