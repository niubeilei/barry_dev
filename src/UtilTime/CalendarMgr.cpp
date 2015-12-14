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
// Modification History:
//	07/29/2011 Created by Chen Ding 
////////////////////////////////////////////////////////////////////////////
#include "UtilTime/CalendarMgr.h"

#include "SingletonClass/SingletonImpl.cpp"
#include "XmlUtil/XmlTag.h"

OmnSingletonImpl(AosCalendarMgrSingleton,
		  	AosCalendarMgr,
		  	AosCalendarMgrSelf,
			"AosCalengarMgr");


AosCalendarMgr::AosCalendarMgr()
{
}


AosCalendarMgr::~AosCalendarMgr()
{
}


bool        
AosCalendarMgr::start()
{
	return true;
}


bool        
AosCalendarMgr::stop()
{
	return true;
}


bool
AosCalendarMgr::config(const AosXmlTagPtr &def)
{
	return true;
}


AosCalendarPtr 
AosCalendarMgr::getCalendar(const OmnString &objid)
{
	// This function retrieves the calendar identified by 'objid'. Note that
	// this class caches some of the calendars. It first checks whether the
	// calendar is already in memory. If yes, it returns it.
	OmnNotImplementedYet;
	return 0;
}


AosCalendarPtr 
AosCalendarMgr::getDefaultCalendar(const AosRundataPtr &rdata)
{
	// This function retrieves the default calendar for the locale 'locale'. 
	// If no default calendar is defined for the locale, it retrieves the
	// system default calendar. 
	OmnNotImplementedYet;
	return 0;
}


