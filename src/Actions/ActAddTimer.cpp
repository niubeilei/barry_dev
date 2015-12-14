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
#include "Actions/ActAddTimer.h"


#include "aosDLL/DllMgr.h"
#include "Alarm/Alarm.h"
#include "Debug/Except.h"
#include "Debug/Debug.h"
#include "SEInterfaces/DocClientObj.h"
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

AosActAddTimer::AosActAddTimer(const bool flag)
:
AosSdocAction(AOSACTTYPE_ADDTIMER, AosActionType::eAddTimer, flag)
{
}


AosActAddTimer::~AosActAddTimer()
{
}


bool	
AosActAddTimer::run(const AosXmlTagPtr &sdoc, const AosRundataPtr &rdata)
{
	// This action add an attribute to a doc. 
	// 	<action 
	// 		<sdocid .../>
	// 		<second .../>
	// 		<udata .../>
	// 	</action>
	
	if (!sdoc)
	{
		AosSetError(rdata, "eMissingSmartDoc");
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}

	// Retrieve the dll name
	AosValueRslt value;
	bool rslt = AosValueSel::getValueStatic(value, sdoc, "sobjid", rdata);
	aos_assert_rr(rslt, rdata, false);

	u64 sdocid = 0;
	OmnString sobjid = value.getStr();
	if (sobjid != "")
	{
		sdocid = AosDocClientObj::getDocClient()->getDocidByObjid(sobjid, rdata);
	}
	else
	{
		value.reset();
		rslt = AosValueSel::getValueStatic(value, sdoc, "sdocid", rdata);
		aos_assert_rr(rslt, rdata, false);

		if (!value.getU64()) return false;
	}
	
	if (!sdocid)
	{
		AosSetError(rdata, "eValueIncorrect");
		OmnAlarm << rdata->getErrmsg() << ". Sdoc: " << sdoc->toString() << enderr;
		return false;
	}

	value.reset();
	rslt = AosValueSel::getValueStatic(value, sdoc, "second", rdata);
	aos_assert_rr(rslt, rdata, false);

	u64 second;
	if (!value.getU64()) return false;
	if (!second)
	{
		AosSetError(rdata, "eValueIncorrect");
		OmnAlarm << rdata->getErrmsg() << ". Sdoc: " << sdoc->toString() << enderr;
		return false;
	}
	
	value.reset();
	rslt = AosValueSel::getValueStatic(value, sdoc, "udata", rdata);
	aos_assert_rr(rslt, rdata, false);

	OmnString udata = value.getStr();
	
	u64 timerid = 0;
	rslt = AosTimerMgr::getSelf()->addTimer(sdocid, second, timerid, udata, rdata);
	aos_assert_r(rslt, false);
	aos_assert_r(timerid, false);
	
	OmnString docstr = "";
	docstr << "<Contents timerid=\"" << timerid << "\" />";
	rdata->setResults(docstr);
	rdata->setArg1(AOSARG_TIMER_ID, timerid);
	rdata->setOk();
	return true;
}


AosActionObjPtr
AosActAddTimer::clone(const AosXmlTagPtr &def, const AosRundataPtr &rdata) const
{
	try
	{
		return OmnNew AosActAddTimer(false);
	}

	catch (const OmnExcept &e)
	{
		AosSetError(rdata, "failed_clone_object") << e.getErrmsg() << enderr;
		return 0;
	}

	OmnShouldNeverComeHere;
	return 0;
}


