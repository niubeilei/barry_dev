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
// 07/28/2011	Created by Linda
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_JobTrans_ScheduleTaskTrans_h
#define Aos_JobTrans_ScheduleTaskTrans_h

#include "TransUtil/TaskTrans.h"

class AosScheduleTaskTrans : virtual public AosTaskTrans
{
	u64			mJobDocid;
	OmnString	mLogicId;
	OmnString	mTaskType;
	int 		mCrtJobSvrId;
	u64			mTaskDocid;
	u64			mStartStamp;

public:
	AosScheduleTaskTrans(const bool regflag);
	AosScheduleTaskTrans(
			const u64 &job_docid,
			const OmnString &logic_id,
			const OmnString &task_type,
			const int crt_jobsvrid,
			const u64 &task_docid,
			const int svr_id,
			const u64 &timestamp);
	~AosScheduleTaskTrans();

	// Trans Interface	
	virtual bool serializeFrom(const AosBuffPtr &buff);
	virtual bool serializeTo(const AosBuffPtr &buff);
	virtual AosTransPtr clone();
	virtual bool proc();
	virtual OmnString toString();

};
#endif

