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
#ifndef Aos_JobTrans_UpdateRDDStatusTrans_h
#define Aos_JobTrans_UpdateRDDStatusTrans_h

#include "TaskUtil/Ptrs.h"
#include "TransUtil/TaskTrans.h"
#include "TaskUtil/TaskStatus.h"


class AosUpdateRDDStatusTrans : virtual public AosTaskTrans
{
private:
	AosTaskStatus::E 		mStatus;
	OmnString				mServiceId;
	OmnString				mDataId;
	OmnString				mDataProcName;
	u64						mRDDId;
	int						mRemain;

public:
	AosUpdateRDDStatusTrans(const bool regflag);
	AosUpdateRDDStatusTrans(
			const AosTaskStatus::E &status,
			const OmnString &serviceid,
			const OmnString &dataid,
			const OmnString &dp_name,
			const u64 &rddid,
			const int remain,
			const int server_id);
	~AosUpdateRDDStatusTrans();

	// Trans Interface	
	virtual bool serializeFrom(const AosBuffPtr &buff);
	virtual bool serializeTo(const AosBuffPtr &buff);
	virtual AosTransPtr clone();
	virtual bool proc();
	virtual OmnString toString();

};
#endif

