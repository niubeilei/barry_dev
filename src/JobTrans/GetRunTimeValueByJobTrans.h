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
// 2015/01/19	Created by Jozhi
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_JobTrans_GetRunTimeValueByJobTrans_h
#define Aos_JobTrans_GetRunTimeValueByJobTrans_h

#include "TransUtil/TaskTrans.h"
#include "TaskUtil/Ptrs.h"


class AosGetRunTimeValueByJobTrans : virtual public AosTaskTrans
{
private:
	OmnString 		mKey;
	u64				mJobDocid;

public:
	AosGetRunTimeValueByJobTrans(const bool regflag);
	AosGetRunTimeValueByJobTrans(
			const OmnString &key,
			const u64 &job_docid,
			const int svr_id);
	~AosGetRunTimeValueByJobTrans();

	// Trans Interface	
	virtual bool serializeFrom(const AosBuffPtr &buff);
	virtual bool serializeTo(const AosBuffPtr &buff);
	virtual AosTransPtr clone();
	virtual bool proc(); 

};
#endif
