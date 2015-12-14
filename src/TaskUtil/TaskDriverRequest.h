////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// This type of IIL maintains a list of (string, docid) and is sorted
// based on the string value. 
//
// Modification History:
// 04/28/2013 Created by Linda Lin 
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_TaskUtil_TaskDriverRequest_h
#define AOS_TaskUtil_TaskDriverRequest_h

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "Debug/Debug.h"
#include "TaskUtil/Ptrs.h"

class AosTaskDriverRequest : virtual public OmnRCObject   
{
	OmnDefineRCObject;

public:

	AosTaskDriver::EventTrigger			mEvent;
	AosRundataPtr  						mRdata;
	AosTaskRunnerInfoPtr				mRunnerinfo;
	int									mServerId;
	u64 								mJobDocid; 				

	AosTaskDriverRequest(){}

	AosTaskDriverRequest(
			const AosTaskDriver::EventTrigger &event,
			const AosTaskRunnerInfoPtr &run_info,
			const AosRundataPtr &rdata)
	:
	mEvent(event),
	mRdata(rdata),
	mRunnerinfo(run_info),
	mServerId(-1),
	mJobDocid(0)
	{
	}

	
	AosTaskDriverRequest(
			const AosTaskDriver::EventTrigger &event,
			const u64 &job_docid,
			const AosTaskRunnerInfoPtr &run_info,
			const AosRundataPtr &rdata)
	:
	mEvent(event),
	mRdata(rdata),
	mRunnerinfo(run_info),
	mServerId(-1),
	mJobDocid(job_docid)
	{
	}

	AosTaskDriverRequest(
			const AosTaskDriver::EventTrigger &event,
			const int svr_id)
	:
	mEvent(event),
	mRdata(0),
	mRunnerinfo(0),
	mServerId(svr_id),
	mJobDocid(0)
	{
	}

	AosTaskDriverRequest(
			const AosTaskDriver::EventTrigger &event,
			const int svr_id,
			const AosRundataPtr &rdata)
	:
	mEvent(event),
	mRdata(rdata),
	mRunnerinfo(0),
	mServerId(svr_id),
	mJobDocid(0)
	{
	}

	AosTaskDriverRequest(const AosTaskDriver::EventTrigger &event)
	:
	mEvent(event),
	mRdata(0),
	mRunnerinfo(0),
	mServerId(-1),
	mJobDocid(0)
	{
	}

	AosTaskDriverRequest(
			const AosTaskDriver::EventTrigger &event,
			const u64 &job_docid,
			const AosRundataPtr &rdata)
	:
	mEvent(event),
	mRdata(rdata),
	mRunnerinfo(0),
	mServerId(-1),
	mJobDocid(job_docid)
	{
	}

	~AosTaskDriverRequest()
	{
	}

};

#endif
