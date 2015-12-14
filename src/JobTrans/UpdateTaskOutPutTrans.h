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
#ifndef Aos_JobTrans_UpdateTaskOutPutTrans_h
#define Aos_JobTrans_UpdateTaskOutPutTrans_h

#include "TransUtil/TaskTrans.h"
#include "TaskUtil/Ptrs.h"


class AosUpdateTaskOutPutTrans : virtual public AosTaskTrans
{
	u64						mTaskDocid;
	AosXmlTagPtr 			mXml;

public:
	AosUpdateTaskOutPutTrans(const bool regflag);
	AosUpdateTaskOutPutTrans(
			const u64 &task_docid,
			const AosXmlTagPtr &xml,
			const int server_id);
	~AosUpdateTaskOutPutTrans();

	// Trans Interface	
	virtual bool serializeFrom(const AosBuffPtr &buff);
	virtual bool serializeTo(const AosBuffPtr &buff);
	virtual AosTransPtr clone();
	virtual bool proc(); 

};
#endif

