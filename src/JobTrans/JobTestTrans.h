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
// 2015/01/05	Created by Andy Zhang
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_JobTrans_JobTestTrans_h
#define Aos_JobTrans_JobTestTrans_h

#include "TransUtil/TaskTrans.h"

class AosJobTestTrans : virtual public AosTaskTrans
{
private:
	AosBuffPtr 		mBuff;
	i64				mReqSize;
	i64				mRespSize;
	i64				mSleep;

public:
	AosJobTestTrans(const bool regflag);
	AosJobTestTrans(
			const int svr_id,
			const i64 req_size,
			const i64 resp_size, 
			const i64 sleep,
			const AosRundataPtr &rdata);

	~AosJobTestTrans();

	// Trans Interface	
	virtual bool serializeFrom(const AosBuffPtr &buff);
	virtual bool serializeTo(const AosBuffPtr &buff);
	virtual AosTransPtr clone();
	virtual bool proc(); 
	virtual bool respCallBack(); 

};
#endif

