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
#ifndef Aos_JobTrans_UpdateTaskStatusTrans_h
#define Aos_JobTrans_UpdateTaskStatusTrans_h

#include "TaskUtil/Ptrs.h"
#include "TransUtil/TaskTrans.h"
#include "TaskUtil/TaskStatus.h"
#include "TaskMgr/TaskErrorType.h"


class AosUpdateTaskStatusTrans : virtual public AosTaskTrans
{
private:
	AosTaskStatus::E 		mStatus;
	AosTaskErrorType::E		mErrorType;
	OmnString				mErrorMsg;
	u64						mTaskDocid;

public:
	AosUpdateTaskStatusTrans(const bool regflag);
	AosUpdateTaskStatusTrans(
			const AosTaskStatus::E &status,
			const AosTaskErrorType::E &error_type,
			const OmnString &msg,
			const u64 &task_docid,
			const int server_id);
	~AosUpdateTaskStatusTrans();

	// Trans Interface	
	virtual bool serializeFrom(const AosBuffPtr &buff);
	virtual bool serializeTo(const AosBuffPtr &buff);
	virtual AosTransPtr clone();
	virtual bool proc();
	virtual OmnString toString();

};
#endif

