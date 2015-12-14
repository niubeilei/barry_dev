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
#ifndef Aos_JobTrans_CreateDataCol_h
#define Aos_JobTrans_CreateDataCol_h

#include "TransUtil/TaskTrans.h"

class AosCreateDataCol : virtual public AosTaskTrans
{
private:
	u64				mJobDocid;
	OmnString 		mDataColName;
	OmnString 		mConfig;

public:
	AosCreateDataCol(const bool regflag);
	AosCreateDataCol(
			const OmnString &datacol_id,
			const u64 &job_id,
			const int svr_id,
			const OmnString &config);

	~AosCreateDataCol();

	// Trans Interface	
	virtual bool serializeFrom(const AosBuffPtr &buff);
	virtual bool serializeTo(const AosBuffPtr &buff);
	virtual AosTransPtr clone();
	virtual bool proc(); 

};
#endif

