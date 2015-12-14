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
#ifndef Aos_JobTrans_StartServiceTrans_h
#define Aos_JobTrans_StartServiceTrans_h

#include "TransUtil/TaskTrans.h"
#include "TaskUtil/Ptrs.h"

class AosStartServiceTrans : virtual public AosTaskTrans
{
	u64					mServiceDocid;

public:
	AosStartServiceTrans(const bool regflag);
	AosStartServiceTrans(
			const u64 &service_docid,
			const int server_id,
			const u32 to_proc_id);
	~AosStartServiceTrans();

	// Trans Interface	
	virtual bool serializeFrom(const AosBuffPtr &buff);
	virtual bool serializeTo(const AosBuffPtr &buff);
	virtual AosTransPtr clone();
	virtual bool proc();
};
#endif

