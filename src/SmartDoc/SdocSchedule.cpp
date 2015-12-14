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
// 08/09/2011	Created by Ketty Guo 
////////////////////////////////////////////////////////////////////////////
#include "SmartDoc/SdocSchedule.h"

#include "API/AosApi.h"
#include "SmartDoc/ReservedMgr.h"
#include "Alarm/Alarm.h"
#include "SEUtil/Ptrs.h"
#include "SEUtil/DocTags.h"
#include "alarm_c/alarm.h"
#include "Rundata/Rundata.h"
#include "XmlUtil/XmlTag.h"

AosSdocSchedule::AosSdocSchedule(const bool flag)
:
AosSmartDoc(AOSOTYPE_SCHEDULE, AosSdocId::eSchedule, flag)
{
}

AosSdocSchedule::~AosSdocSchedule()
{
}


bool
AosSdocSchedule::run(
		const AosXmlTagPtr &worker_doc, 
		const AosRundataPtr &rdata)
{
	aos_assert_r(rdata, false);
	aos_assert_rr(worker_doc, rdata, false);
	return AosRunJimo(rdata.getPtr(), worker_doc);

	/*
	AosSchedulerPtr	schedule = AosGetScheduler(rdata, type);
	if(!schedule)
	{
		rdata->setError() << "schedule type is wrong!" << type;
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}

	OmnString opr = sdoc->getAttrStr("schedule_opr");

	if(opr == "add")	return schedule->addSchedule(sdoc, rdata); 
	
	return true;
	*/
}
