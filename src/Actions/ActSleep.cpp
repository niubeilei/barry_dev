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
// This action get a function from a dll lib,
// then run the function and get the return value.
//
// Modification History:
// 2012/02/23	Created by Ken Lee
////////////////////////////////////////////////////////////////////////////
#include "Actions/ActSleep.h"


#include "aosDLL/DllMgr.h"
#include "Alarm/Alarm.h"
#include "Debug/Except.h"
#include "Debug/Debug.h"
#include "DocSelector/DocSelector.h"
#include "Porting/Sleep.h" 
#include "Rundata/Rundata.h"
#include "SEInterfaces/DocSelObj.h"
#include "SmartDoc/SmartDoc.h"
#include "Timer/TimerMgr.h"
#include "Util/OmnNew.h"
#include "ValueSel/ValueSel.h"
#include "XmlUtil/XmlTag.h"
#include "XmlUtil/XmlDoc.h"
#include <string>
using namespace std;

AosActSleep::AosActSleep(const bool flag)
:
AosSdocAction(AOSACTTYPE_SLEEP, AosActionType::eSleep, flag)
{
}


AosActSleep::~AosActSleep()
{
}


bool	
AosActSleep::run(const AosXmlTagPtr &sdoc, const AosRundataPtr &rdata)
{
	OmnNotImplementedYet;
	return false;
}


bool
AosActSleep::run(
		const AosTaskObjPtr &task, 
		const AosXmlTagPtr &sdoc,
		const AosRundataPtr &rdata)
{
	aos_assert_r(sdoc, false);
	u32 all_sleep_time = sdoc->getAttrU32("sleep_time", 0);
	if (all_sleep_time <= 0) all_sleep_time = 1;

	u32 start_sec = OmnGetSecond();
	while(1)
	{
		OmnSleep(1);
		u32 sec = OmnGetSecond();
		if (sec - start_sec >= all_sleep_time) break;

		u32 percent = (sec - start_sec) * 100 / all_sleep_time;
		task->actionProgressed(percent, rdata);
	}
	
	AosActionObjPtr thisptr(this, true);
	task->actionFinished(thisptr, rdata);
	return true;
}


AosActionObjPtr
AosActSleep::clone(const AosXmlTagPtr &def, const AosRundataPtr &rdata) const
{
	try
	{
		return OmnNew AosActSleep(false);
	}

	catch (const OmnExcept &e)
	{
		AosSetError(rdata, "failed_clone_object") << e.getErrmsg() << enderr;
		return 0;
	}

	OmnShouldNeverComeHere;
	return 0;
}


