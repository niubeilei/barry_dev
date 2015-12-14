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
#include "DocFileMgr/RaidWriteReq.h"

#include "Rundata/Rundata.h"
#include "StorageMgr/AioRequest.h"
#include "StorageMgr/StorageMgrAyscIo.h"
#include "Util/File.h"

AosRaidWriteReq::AosRaidWriteReq(
		const char *buff,
		const int64_t &len,
		const OmnSemPtr &sem)
:
mSem(sem)
{	
	mBuff = OmnNew AosBuff((char*)buff, len, len, false AosMemoryCheckerArgs);
}


AosRaidWriteReq::AosRaidWriteReq()
:
mBuff(0),
mSem(0)
{
}


AosRaidWriteReq::~AosRaidWriteReq()
{
}


bool
AosRaidWriteReq::addRequest(
		const u64 &reqid,
		const OmnFilePtr &ff,         
		const int64_t &seek_pos,      
		const int64_t &bytes_to_read)
{
	AosAioCallerPtr thisptr(this, false);
	AosAioRequestPtr req = OmnNew AosAioRequest(reqid, thisptr, ff.getPtr(), seek_pos, bytes_to_read, AosAioRequest::eAioReqWrite);
	mRequest.push_back(req);
	return true;
}


bool
AosRaidWriteReq::sendRequest()
{
	bool rslt = AosStorageMgrAyscIo::getSelf()->addRequest(mRequest);
	aos_assert_r(rslt, false);
	return true;
}


void
AosRaidWriteReq::clear()
{
	mRequest.clear();
}

AosBuffPtr 
AosRaidWriteReq::getIoBuff(const u64 &reqId, const int64_t &size AosMemoryCheckDecl)
{
	aos_assert_r(mBuff->dataLen() > 0 && reqId < (u64)mBuff->dataLen(), 0)
	char *data_buff = mBuff->data();
	aos_assert_r(data_buff, 0);
	AosBuffPtr buff = OmnNew AosBuff(&data_buff[reqId], size, size, false AosMemoryCheckerFileLine);
	return buff;
}


void
AosRaidWriteReq::dataRead(const Parameter &parm, const AosBuffPtr &buff)
{
	OmnNotImplementedYet;
}


void
AosRaidWriteReq::readError(u64 &reqid, OmnString &errmsg)
{
	OmnNotImplementedYet;
}


void
AosRaidWriteReq::dataWrite(const u64 &reqid)
{
	mSem->post();
}


void
AosRaidWriteReq::writeError(u64 &reqid, OmnString &errmsg)
{
	OmnNotImplementedYet;
}


bool
AosRaidWriteReq::sanityCheck()
{
	return true;
}



