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
#ifndef Omn_TimeUtil_Calendar_h
#define Omn_TimeUtil_Calendar_h

#include "aosUtil/Types.h"
#include "Util/String.h"
#include "UtilTime/TimeInfo.h"
#include "Rundata/Rundata.h"
#include "Util/Ptrs.h"

class AosTime;

struct AosCalendar : virtual public OmnRCObject
{
	OmnDefineRCObject;

public:
	AosCalendar();
	~AosCalendar();

	bool nextWorkingDay(AosTime &crttime);
	bool isHoliday(const AosTime &crttime);
	bool isVacation(const AosTime &crttime);
};
#endif
