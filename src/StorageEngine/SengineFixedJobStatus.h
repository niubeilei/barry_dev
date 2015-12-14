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
// 07/09/2012 Created by Linda 
////////////////////////////////////////////////////////////////////////////
#if 0
#ifndef AOS_StorageEngine_SengineFixedJobStatus_h
#define AOS_StorageEngine_SengineFixedJobStatus_h

#include "DataScanner/DocScanner.h"
#include "QueryRslt/Ptrs.h"
#include "Thread/ThrdShellProc.h"
#include "SEInterfaces/DataCacherObj.h"
#include "Thread/ThreadedObj.h"
#include "Thread/ThrdShellProc.h"


class AosSengineFixedJobStatus : public OmnThrdShellProc
{
	OmnDefineRCObject;

public:
	u64 			job_id;
	u32				logic_taskid;
	int				job_serverid;

	u64				finished_client; 
	u64 			start_client;

	u64 			total_num_docs;
	u64 			num_docs; 

	u64 			num_call_back;
	u64 			num_finished_call_back;

	bool 			clean_data_flag;
	u64				total_num_reqs;
	u64 			num_reqs;

	AosRundataPtr 	rdata;
	int				pre_progress;

public:
	AosSengineFixedJobStatus()
	:
	OmnThrdShellProc("JobStatus"),
	job_id(0),
	logic_taskid(0),
	job_serverid(0),
	finished_client(0),
	start_client(0),
	total_num_docs(0),
	num_docs(0),
	num_call_back(0),
	num_finished_call_back(0),
	clean_data_flag(false),
	total_num_reqs(0),
	num_reqs(0),
	rdata(0),
	pre_progress(0)
	{
	}

	~AosSengineFixedJobStatus(){}

	virtual bool    run()
	{
		aos_assert_r(rdata, false);
		OmnScreen << "Sleep (180) " << num_finished_call_back << ":" << num_call_back << endl;
		OmnSleep(180);
		AosStorageEngineMgr::getSelf()->cleanData(job_id, logic_taskid, job_serverid, rdata);
		return true;
	}
	virtual bool    procFinished(){return true;}
	virtual bool    waitUntilFinished(){return true;}
};

#endif
#endif
