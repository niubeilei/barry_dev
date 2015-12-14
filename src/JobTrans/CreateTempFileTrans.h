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
// 2014/08/14	Created by Ken Lee
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_JobTrans_CreateTempFileTrans_h
#define AOS_JobTrans_CreateTempFileTrans_h

#include "TransUtil/TaskTrans.h"


class AosCreateTempFileTrans : virtual public AosTaskTrans
{
	OmnString	mFnamePrefix;
	u64			mRequestedSpace;
	bool		mReserveFlag;

public:
	AosCreateTempFileTrans(const bool regflag);
	AosCreateTempFileTrans(
			const int svr_id,
			const OmnString &fname_prefix,
			const u64 &requested_space,
			const bool reserve_flag);
	~AosCreateTempFileTrans();

	// Trans Interface	
	virtual bool serializeFrom(const AosBuffPtr &buff);
	virtual bool serializeTo(const AosBuffPtr &buff);
	virtual AosTransPtr clone();
	virtual bool proc();

};
#endif
