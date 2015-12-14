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
#ifndef Aos_JobTrans_SetRunTimeValueByJobTrans_h
#define Aos_JobTrans_SetRunTimeValueByJobTrans_h

#include "TransUtil/TaskTrans.h"
#include "TaskUtil/Ptrs.h"


class AosSetRunTimeValueByJobTrans : virtual public AosTaskTrans
{
private:
	OmnString 		mKey;
	AosValueRslt	mValue;
	u64				mJobDocid;

public:
	AosSetRunTimeValueByJobTrans(const bool regflag);
	AosSetRunTimeValueByJobTrans(
			const OmnString &key,
			const AosValueRslt &value,
			const u64 &job_docid,
			const int svr_id);
	~AosSetRunTimeValueByJobTrans();

	// Trans Interface	
	virtual bool serializeFrom(const AosBuffPtr &buff);
	virtual bool serializeTo(const AosBuffPtr &buff);
	virtual AosTransPtr clone();
	virtual bool proc(); 

};
#endif
