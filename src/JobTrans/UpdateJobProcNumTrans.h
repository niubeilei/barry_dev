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
#ifndef Aos_JobTrans_UpdateJobProcNumTrans_h
#define Aos_JobTrans_UpdateJobProcNumTrans_h

#include "TransUtil/TaskTrans.h"


class AosUpdateJobProcNumTrans : virtual public AosTaskTrans
{
	u64 			mTaskDocid;
	AosXmlTagPtr	mXml;

public:
	AosUpdateJobProcNumTrans(const bool regflag);
	AosUpdateJobProcNumTrans(
			const u64 task_docid,
			const AosXmlTagPtr &xml,
			const int svr_id);
	~AosUpdateJobProcNumTrans();

	// Trans Interface	
	virtual bool serializeFrom(const AosBuffPtr &buff);
	virtual bool serializeTo(const AosBuffPtr &buff);
	virtual AosTransPtr clone();
	virtual bool proc(); 

};
#endif

