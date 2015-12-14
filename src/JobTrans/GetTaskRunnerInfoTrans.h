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
#ifndef Aos_JobTrans_GetTaskRunnerInfoTrans_h
#define Aos_JobTrans_GetTaskRunnerInfoTrans_h

#include "TransUtil/TaskTrans.h"
#include "TaskUtil/Ptrs.h"

class AosGetTaskRunnerInfoTrans : virtual public AosTaskTrans
{
	u64					mJobDocid;
	u32					mNumSlots;
	int					mCrtJobSvrId;
	int					mTaskSvrId;

public:
	AosGetTaskRunnerInfoTrans(const bool regflag);
	AosGetTaskRunnerInfoTrans(
			const u64 &job_docid,
			const u32 num_slots,
			const int crt_job_svrid,
			const int server_id);
	~AosGetTaskRunnerInfoTrans();

	// Trans Interface	
	virtual bool serializeFrom(const AosBuffPtr &buff);
	virtual bool serializeTo(const AosBuffPtr &buff);
	virtual AosTransPtr clone();
	virtual bool proc();
	virtual bool respCallBack();

};
#endif

