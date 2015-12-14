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
#ifndef Aos_JobTrans_AbortTaskTrans_h
#define Aos_JobTrans_AbortTaskTrans_h

#include "TransUtil/TaskTrans.h"


class AosAbortTaskTrans : virtual public AosTaskTrans
{
	u64				mJobDocid;	
	u64				mTaskDocid;

public:
	AosAbortTaskTrans(const bool regflag);
	AosAbortTaskTrans(
			const int server_id,
			const u64 &job_docid,
			const u64 &task_docid);
	~AosAbortTaskTrans();

	// Trans Interface	
	virtual bool serializeFrom(const AosBuffPtr &buff);
	virtual bool serializeTo(const AosBuffPtr &buff);
	virtual AosTransPtr clone();
	virtual bool proc();

};
#endif

