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
#ifndef Aos_JobTrans_StopTaskProcessTrans_h
#define Aos_JobTrans_StopTaskProcessTrans_h

#include "TransUtil/TaskTrans.h"
#include "TaskUtil/Ptrs.h"


class AosStopTaskProcessTrans : virtual public AosTaskTrans
{
	u64				mTaskDocid;

public:
	AosStopTaskProcessTrans(const bool regflag);

	AosStopTaskProcessTrans(
			const int server_id,
			const u64 &task_docid);

	~AosStopTaskProcessTrans();

	// Trans Interface	
	virtual bool serializeFrom(const AosBuffPtr &buff);
	virtual bool serializeTo(const AosBuffPtr &buff);
	virtual AosTransPtr clone();
	virtual bool proc(); 

};
#endif

