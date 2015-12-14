////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//	
// Description:
//
// Modification History:
// 2013/04/30 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "BitmapTrans/TransBmpUpdated.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "Rundata/Rundata.h"
#include "Thread/ThreadPool.h"


AosTransBmpUpdated::AosTransBmpUpdated(const bool regflag)
:
AosBitmapTrans(AosTransType::eBitmapUpdated, regflag),
OmnThrdShellProc("bmp_updated"),
mExecutorID(0),
mSendCubeId(0)
{
}


AosTransBmpUpdated::AosTransBmpUpdated(
		const AosRundataPtr &rdata, 
		const u64 executor_id, 
		const int recv_cube_id, 
		const int send_cube_id, 
		const OmnString &errmsg)
:
AosBitmapTrans(AosTransType::eBitmapUpdated, recv_cube_id, true, false, false),
OmnThrdShellProc("bmp_updated"),
mExecutorID(executor_id),
mSendCubeId(send_cube_id),
mErrmsg(errmsg)
{
}


AosTransBmpUpdated::~AosTransBmpUpdated()
{
}


bool
AosTransBmpUpdated::serializeTo(const AosBuffPtr &buff)
{
	// Its format is:
	// 		executor_id		u64
	// 		send_cube_id	int
	// 		errmsg			OmnString
	bool rslt = AosBitmapTrans::serializeTo(buff);	
	aos_assert_r(rslt, false);

	buff->setU64(mExecutorID);
	buff->setInt(mSendCubeId);
	buff->setOmnStr(mErrmsg);
	return true;
}


bool
AosTransBmpUpdated::serializeFrom(const AosBuffPtr &buff)
{
	// Its format is:
	// 		executor_id		u64
	// 		send_cube_id	int
	// 		errmsg			OmnString
	bool rslt = AosBitmapTrans::serializeFrom(buff);	
	aos_assert_r(rslt, false);
	
	mExecutorID = buff->getU64(0);
	if (mExecutorID == 0)
	{
		OmnAlarm << "missing executor id" << enderr;
		return false;
	}

	mSendCubeId = buff->getInt(-1);
	if (mSendCubeId < 0)
	{
		OmnAlarm << "invalid cube id: " << mSendCubeId << enderr;
		return false;
	}

	mErrmsg = buff->getOmnStr("");
	return true;
}


AosTransPtr
AosTransBmpUpdated::clone()
{
	return OmnNew AosTransBmpUpdated(false);
}


bool 
AosTransBmpUpdated::proc()
{
	OmnThrdShellProcPtr thisptr(this, false);
	OmnThreadPool::runProcAsync(thisptr);
	return true;
}


bool		
AosTransBmpUpdated::run()
{
//	OmnNotImplementedYet;
//	return false;
	return true;
}


bool		
AosTransBmpUpdated::procFinished()
{
	return true;
}

