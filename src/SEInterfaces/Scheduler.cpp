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
// 2013/06/02 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#if 0
#include "SEInterfaces/Scheduler.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "API/AosApi.h"
#include "Debug/Except.h"
#include "Rundata/Rundata.h"
#include "Util/Buff.h"


AosScheduler::AosScheduler(
		const AosRundataPtr &rdata, 
		const AosXmlTagPtr &worker_doc, 
		const AosXmlTagPtr &jimo_doc)
:
AosJimo(rdata, worker_doc, jimo_doc, AOS_JIMOTYPE_SCHEDULER)
{
}


AosScheduler::~AosScheduler()
{
}


bool
AosScheduler::serializeFrom(
		const AosRundataPtr &rdata, 
		const AosBuffPtr &buff)
{
	aos_assert_rr(buff, rdata, false);
	return true;
}


bool
AosScheduler::serializeTo(
		const AosRundataPtr &rdata, 
		const AosBuffPtr &buff)
{
	aos_assert_rr(buff, rdata, false);
	return true;
}


bool 
AosScheduler::config(		
		const AosRundataPtr &rdata, 
		const AosXmlTagPtr &worker_doc) 
{
	return AosJimo::config(rdata, worker_doc);
}


OmnString 
AosScheduler::toString() const
{
	OmnString ss = AosJimo::toString();
	return ss;
}


bool 
AosScheduler::addSchedule(
		const AosXmlTagPtr &sdoc, 
		const AosRundataPtr &rdata)
{
	OmnShouldNeverComeHere;
	return false;
}


bool 
AosScheduler::checkSchedule(
		const AosXmlTagPtr &sdoc, 
		const AosRundataPtr &rdata)
{
	OmnShouldNeverComeHere;
	return false;
}


bool 
AosScheduler::removeSchedule(
		const AosXmlTagPtr &sdoc, 
		const AosRundataPtr &rdata)
{
	OmnShouldNeverComeHere;
	return false;
}	


bool 
AosScheduler::retrieveScheduleInfo(
		const AosXmlTagPtr &sdoc, 
		AosXmlTagPtr &schedule_doc,
		AosXmlTagPtr &rsc_doc,
		AosXmlTagPtr &ctnr_doc,
		const AosRundataPtr &rdata)
{
	// This function retrieves three docs: 'schedule_doc', 'rsc_doc', 
	// and 'ctnr_doc'.
	// Retrieve the schedule
	
	AosXmlTagPtr xmldoc = rdata->getReceivedDoc();
	aos_assert_r(xmldoc, false);
	xmldoc = xmldoc->getFirstChild("objdef");
	aos_assert_r(xmldoc, false);
	
	schedule_doc = xmldoc->getFirstChild(); 

	if (!schedule_doc)
	{
		AosSetErrorUser(rdata, "missing_schedule_doc") << xmldoc->toString() << enderr;
		return false;
	}

	// Retrieve the container
	OmnString ctnr_objid = sdoc->getAttrStr("ctnr_objid");
	if (ctnr_objid == "")
	{
		AosSetErrorUser(rdata, "missing_container") << xmldoc->toString() << enderr;
		return false;
		return false;
	}

	// Retrieve the container doc
	ctnr_doc = AosGetDocByObjid(ctnr_objid, rdata);
	if (!ctnr_doc)
	{
		rdata->setError() << "Failed retrieving the container doc!";
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}

	// The container manages a set of resources. Each resource has a number of 
	// instances. Each instance may be reserved by at most one schedule. 
	OmnString rsc_aname = sdoc->getAttrStr("rsc_aname", AOSTAG_SCHEDULE_RSC_OBJID);
	aos_assert_rr(rsc_aname != "", rdata, 0);
	OmnString rsc_objid = schedule_doc->getAttrStr(rsc_aname);
	if (rsc_objid == "")
	{
		rdata->setError() << "Missing resourc id";
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}

	rsc_doc = AosGetDocByObjid(rsc_objid, rdata);
	if (!rsc_doc)
	{
		rdata->setError() << "Missing resource doc: " << rsc_objid;
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}

	rdata->setOk();
	return true;
}


/*
bool
AosSchedulerImpl::getTimes(
		const AosXmlTagPtr &rsc_doc,
		const AosXmlTagPtr &ctnr_doc,
		const AosXmlTagPtr &schedule_doc, 
		AosTimeInfo &starttime, 
		AosTimeInfo &endtime,
		const AosRundataPtr &rdata)
{
	// This function retrieves the starttime and endtime from the schedule_doc.
	// Time is defined in 'schedule_doc'. The time format is either defined 
	// by 'schedule_doc' or 'ctnr_doc'. If none of them defines the time format, 
	// it defaults to AOSTIMEFORMAT_YYYYMMDD.
	//
	// SchedulerImpl time is determined by 'rsc_doc'. A schedule is defined by
	// a number of 'slots'. Each slot represents a time duration. As an example,
	// for hourly managed schedules, each slot is one hour. Each schedule has
	// a start slot and end slot. These define the time slots that can be 
	// scheduled. All the information is defined by 'rsc_doc'.
	aos_assert_rr(rsc_doc, rdata, false);
	aos_assert_rr(ctnr_doc, rdata, false);
	aos_assert_rr(schedule_doc, rdata, false);

	// Retrieve the time format
	OmnString timeformat = schedule_doc->getAttrStr("timeformat");
	if (timeformat == "") timeformat = ctnr_doc->getAttrStr("timeformat");
	if (timeformat == "") timeformat = AOSTIMEFORMAT_YYYYMMDD;

	// Retrieve the start and end slots
	// int start_time = rsc_doc->getAttrInt("start_time", 0);
	// int end_time = rsc_doc->getAttrInt("end_time", 23);
	// int max_slots = rsc_doc->getAttrInt("max_slots", -1);
	// if (max_slots < 0)
	// {
	// 	rdata->setError() << "Missing max slots: " << rsc_doc->getAttrStr(AOSTAG_OBJID);
	// 	OmnAlarm << rdata->getErrmsg() << enderr;
	// 	return false;
	// }

	// if (end_slot > max_slots)
	// {
	// 	rdata->setError() << "End slot is incorrect: " << end_slot 
	// 		<< ". The maximum slot allowed is: " << max_slots
	// 		<< ". The resource doc objid is: " << rsc_doc->getAttrStr(AOSTAG_OBJID);
	// 	OmnAlarm << rdata->getErrmsg() << enderr;
	// 	return false;
	// }

	// Retrieve the calendar, which is used to determine the scheduleable days.
	OmnString calendar_objid = ctnr_doc->getAttrStr("schedule_calendar");
	AosCalendarPtr calendar;
	if (calendar_objid != "")
	{
		calendar = AosCalendarMgr::getSelf()->getCalendar(calendar_objid);
		if (!calendar)
		{
			OmnAlarm << "Failed retrieving the calendar: " << calendar_objid << enderr;
		}
	}

	if (!calendar)
	{
		calendar = AosCalendarMgr::getSelf()->getDefaultCalendar(rdata);
		aos_assert_rr(calendar, rdata, false);
	}

	// The scheduler objid is determined by the scheduled starting day
	OmnString starttimestr = schedule_doc->getAttrStr("starttime");
	if (starttimestr == "")
	{
		rdata->setError() << "Missing the start time";
		OmnAlarm << rdata->getErrmsg() << enderr;
		return "";
	}

	OmnString endtimestr = schedule_doc->getAttrStr("endtime");
	if (endtimestr == "")
	{
		rdata->setError() << "Missing end time";
		OmnAlarm << rdata->getErrmsg() << enderr;
		return "";
	}

	// Create the start time and end time. 
	bool rslt = AosTimeUtil::getTime(starttimestr, timeformat, starttime);
	aos_assert_rr(rslt, rdata, false);
	rslt = AosTimeUtil::getTime(endtimestr, timeformat, endtime);
	aos_assert_rr(rslt, rdata, false);

	return true;
}
*/

#endif
