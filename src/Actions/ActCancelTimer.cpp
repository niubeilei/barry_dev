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
#include "Actions/ActCancelTimer.h"


#include "aosDLL/DllMgr.h"
#include "Alarm/Alarm.h"
#include "Debug/Except.h"
#include "Debug/Debug.h"
#include "DocSelector/DocSelector.h"
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

AosActCancelTimer::AosActCancelTimer(const bool flag)
:
AosSdocAction(AOSACTTYPE_CANCELTIMER, AosActionType::eCancelTimer, flag)
{
}


AosActCancelTimer::~AosActCancelTimer()
{
}


bool	
AosActCancelTimer::run(const AosXmlTagPtr &sdoc, const AosRundataPtr &rdata)
{
	// This action add an attribute to a doc. 
	// 	<action 
	// 		<timerid .../>
	// 	</action>
	
	if (!sdoc)
	{
		AosSetError(rdata, "eMissingSmartDoc");
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}

	// Retrieve the dll name
	AosValueRslt value;
	bool rslt = AosValueSel::getValueStatic(value, sdoc, "timerid", rdata);
	aos_assert_rr(rslt, rdata, false);

	u64 timerid;
	if (!value.getU64()) return false;
	if (!timerid)
	{
		AosSetError(rdata, "eValueIncorrect");
		OmnAlarm << rdata->getErrmsg() << ". Sdoc: " << sdoc->toString() << enderr;
		return false;
	}

	rslt = AosTimerMgr::getSelf()->cancelTimer(timerid, rdata);
	aos_assert_r(rslt, false);
	
	rdata->setOk();
	return true;
}


AosActionObjPtr
AosActCancelTimer::clone(const AosXmlTagPtr &def, const AosRundataPtr &rdata) const
{
	try
	{
		return OmnNew AosActCancelTimer(false);
	}

	catch (const OmnExcept &e)
	{
		AosSetError(rdata, "failed_clone_object") << e.getErrmsg() << enderr;
		return 0;
	}

	OmnShouldNeverComeHere;
	return 0;
}



