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
#include "UtilTime/Calendar.h"

#include "XmlUtil/XmlTag.h"

AosCalendar::AosCalendar()
{
}


AosCalendar::~AosCalendar()
{
}

bool 
AosCalendar::nextWorkingDay(AosTime &crttime)
{
	return true;
}


bool 
AosCalendar::isHoliday(const AosTime &crttime)
{
	return true;
}


bool 
AosCalendar::isVacation(const AosTime &crttime)
{
	return true;
}
