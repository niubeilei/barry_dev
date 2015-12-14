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
// 07/28/2011	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_DynJimos_ScheduleDay_SCHEDULEDAY_h
#define AOS_DynJimos_ScheduleDay_SCHEDULEDAY_h

#include "Scheduler/SchedulerImpl.h"


#define AOSTAG_SCHEDULE_STARTTIME   "schedule_start_time"
#define AOSTAG_SCHEDULE_ENDTIME   	"schedule_end_time"
#define AOSTAG_INSTANCE_ID 			"instance_id"

class AosSchedulerDay : public AosSchedulerImpl
{
public:
	AosSchedulerDay(
			const AosRundataPtr &rdata, 
			const AosXmlTagPtr &worker_doc,
			const AosXmlTagPtr &jimo_doc);
	~AosSchedulerDay();

	virtual bool config(const AosRundataPtr &rdata, 
						const AosXmlTagPtr &worker_doc,
						const AosXmlTagPtr &jimo_doc);

	virtual bool addSchedule(
						const AosXmlTagPtr &sdoc, 
						const AosRundataPtr &rdata);

	virtual bool checkSchedule(
						const AosXmlTagPtr &sdoc, 
						const AosRundataPtr &rdata);

	virtual bool removeSchedule(
						const AosXmlTagPtr &sdoc, 
						const AosRundataPtr &rdata);

private:
	bool checkConflicts(const AosXmlTagPtr &schedule_doc,
						const AosXmlTagPtr &sdoc,
						const AosXmlTagPtr &rsc_doc,
						const AosXmlTagPtr &ctnr_doc,
						const AosRundataPtr &rdata);

	bool setResourceFlags(
						char *resources, 
						const int num_resources, 
						const int64_t &starttime,
						const int64_t &endtime,
						const OmnString &ctnr_objid,
						const OmnString &rsc_id,
						const AosXmlTagPtr &sdoc,
						const AosRundataPtr &rdata);

	bool doQuery( 		char *resources, 
						const int num_resources, 
						const OmnString &rsc_id,
						const OmnString &ctnr_objid,
						const OmnString &rsc_aname, 
						const OmnString &time_aname,
						const u64 &starttime,
						const u64 &endtime,
						const AosRundataPtr &rdata);

	bool setResourceFlags(
						char *resources, 
						const int num_resources, 
						const AosQueryRsltObjPtr query_rslt,
						const AosRundataPtr &rdata);
};
#endif
