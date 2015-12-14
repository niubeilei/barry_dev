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
#include "Worker/Worker.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "Debug/Except.h"
#include "Rundata/Rundata.h"
#include "Util/Buff.h"


AosWorker::AosWorker(
		const AosRundataPtr &rdata, 
		const AosXmlTagPtr &worker_doc)
{
}


AosWorker::~AosWorker()
{
}


bool
AosWorker::serializeFrom(
		const AosRundataPtr &rdata, 
		const AosBuffPtr &buff)
{
	aos_assert_rr(buff, rdata, false);
	return true;
}


bool
AosWorker::serializeTo(
		const AosRundataPtr &rdata, 
		const AosBuffPtr &buff) const
{
	aos_assert_rr(buff, rdata, false);
	return true;
}


AosWorkerPtr 
AosWorker::clone() const 
{
	OmnShouldNeverComeHere;
	return 0;
}


bool 
AosWorker::run(		
		const AosRundataPtr &rdata, 
		const AosXmlTagPtr &worker_doc)
{
	OmnShouldNeverComeHere;
	return false;
}


bool 
AosWorker::config(		
		const AosRundataPtr &rdata, 
		const AosXmlTagPtr &worker_doc)
{
	// The worker_doc should be in the following format:
	// 	<worker 
	// 		AOSTAG_WORKER_TYPE="xxx">
	// 		<AOSTAG_INPUT_DATASETS .../>
	// 		<AOSTAG_JOB_SPLITTER .../>
	// 		<AOSTAG_JIMOS .../>
	// 		<AOSTAG_SCHEDULER .../>
	// 		<AOSTAG_INSTAANCE_NAMECREATOR .../>
	// 	</worker>
	aos_assert_rr(worker_doc, rdata, false);
	mWorkerType = worker_doc->getAttrStr(AOSTAG_WORKER_TYPE);
	if (!isValidWorkerType(mWorkerType))
	{
		AosSetErrorUser(rdata, "invalid_worker_type")
			<< mWorkerType
			<< ". " << AOSDICTERM("config_doc_is", rdata) << worker_doc->toString() << enderr;
		return false;
	}

	// Create the Input.
	AosXmlTagPtr tag = worker_doc->getFirstChild(AOSTAG_INPUT_DATASETS);
	if (!tag)
	{
		AosSetErrorUser(rdata, "missing_input_data")
			<< ". " << AOSDICTERM("config_doc_is", rdata) 
			<< worker_doc->toString() << enderr;
		return false;
	}
	mTaskData = AosCreateTaskData(rdata, tag);
	if (!mTaskData) return false;

	// Create the job splitter
	tag = worker_doc->getFirstChild(AOSTAG_JOB_SPLITTER);
	mJobSplitter = AosCreateDefaultJobSplitter(rdata, mTaskData, tag);
	if (!mJobSplitter)
	{
		AosSetErrorUser(rdata, "missing_job_splitter")
			<< ". " << AOSDICTERM("config_doc_is", rdata) 
			<< worker_doc->toString() << enderr;
		return false;
	}

	// Create jimos
	tag = worker_doc->getFirstChild(AOSTAG_JIMOS);
	if (!tag)
	{
		AosSetErrorUser(rdata, "missing_jimos")
			<< ". " << AOSDICTERM("config_doc_is", rdata) 
			<< worker_doc->toString() << enderr;
		return false;
	}
	bool rslt = createJimos(rdata, tag);
	aos_assert_r(rslt, false);

	// Create the scheduler
	tag = worker_doc->getFirstChild(AOSTAG_SCHEDULER);
	if (tag)
	{
		bool rslt = AosCreateScheduler(rdata, tag, mScheduler);
		aos_assert_r(rslt, false);
	}

	// Create the name creator
	tag = worker_doc->getFirstChild(AOSTAG_INSTAANCE_NAMECREATOR);
	if (tag)
	{
		// 'tag' should identify a jimo. 
		mNameCreator = AosCreateJimo(rdata, tag);
		if (!mNameCreator) return false;
	}

	return false;
}


OmnString 
AosWorker::toString() const
{
	OmnShouldNeverComeHere;
	return "";
}


