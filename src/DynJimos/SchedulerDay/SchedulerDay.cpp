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
// 07/27/2011	Created by Michael 
////////////////////////////////////////////////////////////////////////////
#include "DynJimos/SchedulerDay/SchedulerDay.h"

#include "DbQuery/Query.h"
#include "SEInterfaces/DocClientObj.h"
#include "SEUtil/IILName.h"
#include "Security/SecurityMgr.h"
#include "UtilTime/TimeInfo.h"
#include "Util/Opr.h"



AosSchedulerDay::AosSchedulerDay(
		const AosRundataPtr &rdata, 
		const AosXmlTagPtr &worker_doc,
		const AosXmlTagPtr &jimo_doc)
:
AosSchedulerImpl(rdata, worker_doc, jimo_doc)
{
	if (!config(rdata, worker_doc, jimo_doc))
	{
		OmnThrowException(rdata->getErrmsg());
		return;
	}
}


AosSchedulerDay::~AosSchedulerDay()
{
}


bool
AosSchedulerDay::addSchedule(
		const AosXmlTagPtr &sdoc, 
		const AosRundataPtr &rdata) 
{
	// This smart doc adds a schedule to the system. 
	// container. Note that it copies, not moves docs. 
	// 	<sdoc type="xxx" is_public="true|false" cid_required="true|false">
	// 		<orig_ctnr .../>
	// 		<target_ctnr .../>
	// 	</sdoc>
	//
	// The input to this function is a 'schedule_doc' that defines the new
	// schedule to be added. The scheduled resource is managed through 
	// an XML doc ('rsc_doc'). 
	
	aos_assert_rr(sdoc, rdata, false);
	AosXmlTagPtr schedule_doc, rsc_doc, ctnr_doc;
	bool rslt = retrieveScheduleInfo(sdoc, schedule_doc, rsc_doc, ctnr_doc, rdata);
	if (!rslt) return false;
	aos_assert_rr(schedule_doc, rdata, false);
	aos_assert_rr(rsc_doc, rdata, false);
	aos_assert_rr(ctnr_doc, rdata, false);
	OmnString ctnr_objid = rsc_doc->getAttrStr(AOSTAG_OBJID, 0);

	// Check whether the user has the right to schedule it.
	if (!AosSecurityMgr::getSelf()->checkAddMember1(ctnr_doc, rdata))
	{
		// Access denied
		return false;
	}

	// This function checks whether the schedule [startinfo, endinfo] can be 
	// added to the schedule 'start_scheduler' and 'end_scheduler'. Scheduled
	// resources are resources that are managed based on time. Each scheduled
	// resource may have 1 or more instances. A schedule can be added only when 
	// there is an idle slot for [startinfo, endinfo]. 
	//
	// Schedules that are managed by SchedulerDay have one XML doc for each 
	// day. The XML doc objid is determined by 
	// 		AosObjid::composeSchedulerObjid(siteid, ctnr_objid, starttime)
	// This is called Scheduler Doc. Each Scheduler Doc records the scheduled
	// information for a given day. This is used to determine whether a schedule
	// conflicts with others. 

	// Check whether the requested schedule conflicts with the existing ones. 
	// If yes, it is an error. 
	rslt = checkConflicts(schedule_doc, sdoc, rsc_doc, ctnr_doc, rdata);
	if (!rslt)
	{
		// It conflicts with the existing ones. 
		return false;
	}

	// The schedule is ok. Ready to create the schedule
	schedule_doc->setAttr(AOSTAG_PARENTC, ctnr_doc->getAttrStr(AOSTAG_OBJID));
	AosXmlTagPtr created_doc = AosDocClientObj::getDocClient()->createDocSafe3(
			rdata, schedule_doc, rdata->getCid(), "", true, true, false, 
			false, false, true, true);
	if (!created_doc)
	{
		OmnAlarm << "Failed creating the schedule doc: " << rdata->getErrmsg() << enderr;
		return false;
	}

	OmnString contents = "<Contents>";
	contents << schedule_doc->toString() << "</Contents>";
	rdata->setOk();
	return true;
}

	
bool
AosSchedulerDay::checkSchedule(
		const AosXmlTagPtr &sdoc, 
		const AosRundataPtr &rdata)
{
	return true;
}


bool
AosSchedulerDay::removeSchedule(
		const AosXmlTagPtr &sdoc, 
		const AosRundataPtr &rdata)
{
	return true;
}


bool
AosSchedulerDay::checkConflicts(
		const AosXmlTagPtr &schedule_doc,
		const AosXmlTagPtr &sdoc,
		const AosXmlTagPtr &rsc_doc,
		const AosXmlTagPtr &ctnr_doc,
		const AosRundataPtr &rdata)
{
	// 1. It queries the database for all the schedules that overlap with 
	// 	  [startime, endtime]. 
	// 2. Check whether there is any resource instance that can be scheduled
	//    based on [starttime, endtime].
	
	// Query the database. The container should have the following IILs:
	// 		IIL by the attribute AOSTAG_SCHEDULE_STARTTIME
	// 		IIL by the attribute AOSTAG_SCHEDULE_ENDTIME
	
	OmnString stime_aname = sdoc->getAttrStr("stime_aname", AOSTAG_STARTTIME);
	aos_assert_rr(stime_aname != "", rdata, false);
	OmnString etime_aname = sdoc->getAttrStr("etime_aname", AOSTAG_ENDTIME);
	aos_assert_rr(etime_aname != "", rdata, false);
	
	int64_t starttime = schedule_doc->getAttrInt(stime_aname, -1);
	int64_t endtime = schedule_doc->getAttrInt(etime_aname, -1);
	OmnString ctnr_objid = ctnr_doc->getAttrStr(AOSTAG_OBJID); 
	OmnString rsc_id = rsc_doc->getAttrStr(AOSTAG_RESOURCE_ID);
	aos_assert_rr(rsc_id != "", rdata, false);

	if (starttime < 0)
	{
		rdata->setError() << "Incorrect start time: " << starttime;
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}

	if (endtime < 0)
	{
		rdata->setError() << "Incorrect end time: " << endtime;
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}

	if (starttime >= endtime)
	{
		rdata->setError() << "Start time is not correct: " << starttime 
			<< ":" << endtime;
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}

	int num_resources = rsc_doc->getAttrInt("num_insts", 1);
	if (num_resources <= 0)
	{
		rdata->setError() << "Incorrect number of instances: " << num_resources
			<< ". Resource ID: " << rsc_doc->getAttrStr(AOSTAG_OBJID);
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}

	try
	{
		char *resources = OmnNew char[num_resources];
		memset(resources, 0, num_resources);

		bool rslt = setResourceFlags(resources, num_resources, 
				starttime, endtime, ctnr_objid, rsc_id, sdoc, rdata);
		if (!rslt)
		{
			OmnDelete [] resources;
			return false;
		}

		// Check whether any resource is found
		for (int i=0; i<num_resources; i++)
		{
			if (!resources[i])
			{
				// Found it. 
				schedule_doc->setAttr(AOSTAG_INSTANCE_ID, i);
				rdata->setOk();
				OmnDelete [] resources;
				return true;
			}
		}

		rdata->setError() << "No available schedule for the request";
		OmnAlarm << rdata->getErrmsg() << enderr;
		OmnDelete [] resources;
		return false;
	}

	catch (...)
	{
		rdata->setError() << "Failed allocating memory: " << num_resources;
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}

	OmnShouldNeverComeHere;
	rdata->setError() << "Internal error";
	return false;
}


bool
AosSchedulerDay::setResourceFlags(
		char *resources, 
		const int num_resources, 
		const int64_t &starttime,
		const int64_t &endtime,
		const OmnString &ctnr_objid,
		const OmnString &rsc_id,
		const AosXmlTagPtr &sdoc,
		const AosRundataPtr &rdata)
{
	// This function sets the resource flags based on [starttime, endtime]. 
	// Schedules are XML docs under a container. To set the resources, we need
	// to query:
	// 	Query 1:
	// 		resourceid_iil: value == rsc_id 
	// 		otype_iil: otype == "schedule"
	// 		starttime_iil: value >= starttime
	// 		starttime_iil: value <= endtime 
	//
	//  Query 2: 
	//  	resourceid_iil: value == resource
	//  	otype_iil: otype == "schedule"
	//  	endtime_iil: value >= starttime 
	//  	endtime_iil: value <= endtime
	u32 siteid = rdata->getSiteid();
	aos_assert_rr(siteid != 0, rdata, false);
	aos_assert_rr(starttime >= 0, rdata, false);
	aos_assert_rr(endtime >= 0, rdata, false);

	OmnString rsc_aname = sdoc->getAttrStr("rsc_aname", AOSTAG_SCHEDULE_RSC_OBJID);
	aos_assert_rr(rsc_aname != "", rdata, false);
	OmnString stime_aname = sdoc->getAttrStr("stime_aname", AOSTAG_STARTTIME);
	aos_assert_rr(stime_aname != "", rdata, false);
	OmnString etime_aname = sdoc->getAttrStr("etime_aname", AOSTAG_ENDTIME);
	aos_assert_rr(etime_aname != "", rdata, false);

	// Do Query 1
	bool rslt;
	rslt = doQuery(resources, num_resources, rsc_id, ctnr_objid, rsc_aname, 
			stime_aname, (u64)starttime, (u64)endtime, rdata);
	aos_assert_rr(rslt, rdata, false);

	// Do Query 2
	rslt = doQuery(resources, num_resources, rsc_id, ctnr_objid, rsc_aname, 
			etime_aname, (u64)starttime, (u64)endtime, rdata);
	aos_assert_rr(rslt, rdata, false);

	return true;
}


bool
AosSchedulerDay::doQuery(
		char *resources, 
		const int num_resources, 
		const OmnString &rsc_id,
		const OmnString &ctnr_objid,
		const OmnString &rsc_aname, 
		const OmnString &time_aname,
		const u64 &starttime,
		const u64 &endtime,
		const AosRundataPtr &rdata)
{
	u32 siteid = rdata->getSiteid();
	aos_assert_rr(siteid != 0, rdata, false);

	AosQueryRsltObjPtr query_rslt = 0;

	const int num_conds = 4;
	OmnString ctnr_objids[num_conds];
	OmnString anames[num_conds];
	AosValueRslt values[num_conds];
	AosOpr oprs[num_conds];
	bool reverse[num_conds];
	bool order[num_conds];

	OmnString valueStr;
	// Container 'rsc_aname' member listing
	ctnr_objids[0] = ctnr_objid;
	anames[0] = rsc_aname;
	oprs[0] = eAosOpr_eq;
	valueStr = rsc_id;
	values[0].setValue(valueStr);
	reverse[0] = false;
	order[0] = false;

	// Container 'otype' member listing
	ctnr_objids[1] = ctnr_objid;
	anames[1] = AOSTAG_OTYPE;
	oprs[1] = eAosOpr_eq;
	valueStr = AOSOTYPE_SCHEDULE;
	values[1].setValue(valueStr);
	reverse[1] = false;
	order[1] = false;

	// Container 'timeaname' member listing
	ctnr_objids[2] = ctnr_objid;
	anames[2] = time_aname;
	oprs[2] = eAosOpr_ge;
	valueStr = "";
	valueStr << starttime;
	values[2].setValue(valueStr);
	reverse[2] = false;
	order[2] = false;
	
	// Container 'timeaname' member listing
	ctnr_objids[3] = ctnr_objid;
	anames[3] = time_aname;
	oprs[3] = eAosOpr_le;
	valueStr = "";
	valueStr <<  endtime;
	values[3].setValue(valueStr);
	reverse[3] = false;
	order[3] = false;

	// Chen Ding, 03/23/2012
	// bool rslt = AosQuery::getSelf()->runQuery(ctnr_objids, anames, values, oprs, 
	// 					reverse, order, num_conds, query_rslt, rdata);
	// CHEN, WARN
	int startidx = 0;
	int psize = 20;
	bool rslt = AosQuery::getSelf()->runQuery(startidx, psize,
			ctnr_objids, anames, values, oprs, 
						reverse, order, num_conds, query_rslt, rdata);
	if (!rslt)
	{
		OmnAlarm << "Failed querying: " << rdata->getErrmsg() << enderr;
		return false;
	}

	// Found some results. Process the results.
	if (!setResourceFlags(resources, num_resources, query_rslt, rdata))
	{
		return false;
	}
	return true;
}


bool
AosSchedulerDay::setResourceFlags(
		char *resources, 
		const int num_resources, 
		const AosQueryRsltObjPtr query_rslt,
		const AosRundataPtr &rdata)
{
	// Check the resources
	AosXmlTagPtr ins_doc;
	int ins_id;	

	u64 docid = 0;
	bool finished = false;
	while(1)
	{
		query_rslt->nextDocid(docid, finished, rdata); 
		if (finished) break;
	
		ins_doc = AosDocClientObj::getDocClient()->getDocByDocid(docid, rdata);
		if (!ins_doc)
		{
			OmnAlarm << "Failed retrieving the doc: " << docid << enderr;
			continue;
		}
		
		ins_id = ins_doc->getAttrInt(AOSTAG_INSTANCE_ID , -1);

		if (ins_id < 0 || ins_id >= num_resources)
		{
			OmnAlarm << "Instance ID Invalid: " << ins_id << ":" << num_resources << enderr;
			continue;
		}
		
		resources[ins_id] = 1;
	}
	
	return true;
}

/*
bool
AosSchedulerDay::getBookingTime(
		const AosXmlTagPtr &rsc_doc, 
		const AosTimeInfo &timeinfo, 
		int &begintime,
		int &endtime,
		const AosRundataPtr)
{
	// It retrieves the begin time for the resource 'rsc_doc', based on 
	// the time 'timeinfo'. 
	//
	// In the current implementations, we support weekdays and weekends.
	AosTimeInfo::DayType type = timeinfo.getDayType();
	switch (type)
	{
	case AosTimeInfo::eWeekday:
		 return rsc_doc->getAttrInt("weekday_btime", -1);

	case AosTimeInfo::eWeekend:
		 return rsc_doc->getAttrInt("weekend_btime", -1);

	case AosTimeInfo::eHoliday:
		 return rsc_doc->getAttrInt("holidday", -1);

	case AosTimeInfo::eClosingDay:
		 return -1;

	default:
		 break;
	}

	rdata->setError() << "Unrecognized day type: " << type;
	OmnAlarm << rdata->getErrmsg() << enderr;
	return -1;
}
*/

