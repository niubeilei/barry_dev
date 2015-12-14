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
// 07/26/2011	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "SmartDoc/SdocAddSchedule.h"

#include "API/AosApi.h"
#include "Actions/ActSeqno.h"
#include "ValueSel/ValueSel.h"
#include "ValueSel/ValueRslt.h"
#include "SEUtil/IILName.h"
#include "Rundata/Rundata.h"
#include "UtilComm/TcpClient.h"
#include "XmlInterface/WebRequest.h"
#include "XmlUtil/XmlTag.h"
#include "XmlUtil/SeXmlParser.h"

AosSdocAddSchedule::AosSdocAddSchedule(const bool flag)
:
AosSmartDoc(AOSSDOCTYPE_ADDSCHEDULE, AosSdocId::eAddSchedule, flag)
{
}


AosSdocAddSchedule::~AosSdocAddSchedule()
{
}

bool
AosSdocAddSchedule::run(
		const AosXmlTagPtr &worker_doc, 
		const AosRundataPtr &rdata) 
{
	// This smart doc adds a schedule to the system. 
	// 	<worker type="xxx"
	// 		AOSTAG_USE_JIMO="xxx"
	// 		AOSTAG_METHOD="xxx"
	// 		version="xxx"
	// 		...
	// 	</worker>
	
	return AosRunJimo(rdata.getPtr(), worker_doc);
	/*
	aos_assert_rr(worker_doc, rdata, false);
	OmnString schedule_type = sdoc->getAttrStr("schedule_type", "normal");
	AosSchedulerPtr schedule = AosGetScheduler(rdata, schedule_type);
	if (!schedule)
	{
		AosSetError(rdata, "schedule_type_not_fd") << ": " << schedule_type;
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}

	bool rslt = schedule->addSchedule(sdoc, rdata);
	return rslt;
	*/
}

