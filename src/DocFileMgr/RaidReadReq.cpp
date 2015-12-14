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
// 05/13/2013 Created by Linda Lin 
////////////////////////////////////////////////////////////////////////////
#include "DocFileMgr/RaidReadReq.h"

#include "Rundata/Rundata.h"
#include "StorageMgr/AioRequest.h"
#include "StorageMgr/StorageMgrAyscIo.h"
#include "Util/File.h"

AosRaidReadReq::AosRaidReadReq(
		char *buff,
		const int64_t &len,
		const OmnSemPtr &sem,
		int64_t &bytesread)
:
mSem(sem),
mBytesRead(bytesread),
mErrorCode(false),
mErrmsg("")
{	
	memset(buff, 0, len);
	mBuff = OmnNew AosBuff(buff, len, 0, false AosMemoryCheckerArgs);
}


static int64_t sgInit;
AosRaidReadReq::AosRaidReadReq()
:
mBuff(0),
mSem(0),
mBytesRead(sgInit),
mErrorCode(false),
mErrmsg("")
{
}


AosRaidReadReq::~AosRaidReadReq()
{
}


bool
AosRaidReadReq::addRequest(
		const u64 &reqId,
		const OmnFilePtr &ff,
		const int64_t &seek_pos,
		const int64_t &bytes_to_read)
{
	AosAioCallerPtr thisptr(this, true);
	AosAioRequestPtr req = OmnNew AosAioRequest(reqId, thisptr, ff.getPtr(), seek_pos, bytes_to_read, AosAioRequest::eAioReqRead);
	mRequest.push_back(req);
	return true;
}


bool
AosRaidReadReq::sendRequest()
{
	bool rslt = AosStorageMgrAyscIo::getSelf()->addRequest(mRequest);
	aos_assert_r(rslt, false);
	return true;
}


void
AosRaidReadReq::clear()
{
	mRequest.clear();
}

AosBuffPtr 
AosRaidReadReq::getIoBuff(const u64 &reqId, const int64_t &size AosMemoryCheckDecl)
{
	char *data_buff = mBuff->data();
	aos_assert_r(data_buff, 0);
	AosBuffPtr buff = OmnNew AosBuff(&data_buff[reqId], size, 0, false AosMemoryCheckerFileLine);
	return buff;
}


void
AosRaidReadReq::dataRead(const Parameter &parm, const AosBuffPtr &buff)
{
	aos_assert(parm.reqId < (u64)mBuff->buffLen());
	if (!parm.finished)
	{
		aos_assert(parm.expected_size == buff->dataLen());
	}

	// Chen Ding, 2013/05/26
	// if (buff->dataLen() > 0)
	if (buff->dataLen() >= 0)
	{
		//http://www.linuxidc.com/Linux/2011-06/37403.htm
		__sync_fetch_and_add(&mBytesRead,  buff->dataLen());
	}
	mSem->post();
}


void
AosRaidReadReq::readError(u64 &reqid, OmnString &errmsg)
{
	mErrorCode = true;
	mErrmsg << "reqid: " << reqid << ", " << errmsg << ";";
	mSem->post();
}


void
AosRaidReadReq::dataWrite(const u64 &reqid)
{
	OmnNotImplementedYet;
}


void
AosRaidReadReq::writeError(u64 &reqid, OmnString &errmsg)
{
	OmnNotImplementedYet;
}


bool
AosRaidReadReq::sanityCheck()
{
	return true;
}



