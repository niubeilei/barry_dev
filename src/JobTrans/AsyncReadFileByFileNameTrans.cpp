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
//
// Modification History:
// 07/28/2011	Created by Linda 
////////////////////////////////////////////////////////////////////////////
#if 0
#include "JobTrans/AsyncReadFileByFileNameTrans.h"

#include "API/AosApi.h"
#include "XmlUtil/XmlTag.h"
#include "SEInterfaces/NetFileMgrObj.h"
//#include "JobTrans/AsyncReadFileByFileNameCbTrans.h"
#include "StorageMgr/NetFileMgr.h"


AosAsyncReadFileByFileNameTrans::AosAsyncReadFileByFileNameTrans(const bool regflag)
:
AosTaskTrans(AosTransType::eAsyncReadFileByFileName, regflag)
{
}


AosAsyncReadFileByFileNameTrans::AosAsyncReadFileByFileNameTrans(
		const u64 &reqid,
		const AosAioCallerPtr &caller,
		const OmnString &filename,
		const int64_t &seekPos,
		const u32 bytes_to_read,
		const int svr_id,
		const bool need_save,
		const bool need_resp)
:
AosTaskTrans(AosTransType::eAsyncReadFileByFileName, svr_id, need_save, need_resp),
mReqId(reqid),
mCaller(caller),
mFileName(filename),
mSeekPos(seekPos),
mBytesToRead(bytes_to_read)
{
}


AosAsyncReadFileByFileNameTrans::~AosAsyncReadFileByFileNameTrans()
{
}


bool
AosAsyncReadFileByFileNameTrans::serializeFrom(const AosBuffPtr &buff)
{
	bool rslt = AosTaskTrans::serializeFrom(buff);
	aos_assert_r(rslt, false);
	mReqId = buff->getU64(0);
	mFileName = buff->getOmnStr("");
	mSeekPos = buff->getInt64(0);
	mBytesToRead = buff->getU32(0);
	return true;
}


bool
AosAsyncReadFileByFileNameTrans::serializeTo(const AosBuffPtr &buff)
{
	bool rslt = AosTaskTrans::serializeTo(buff);
	aos_assert_r(rslt, false);
	buff->setU64(mReqId);
	buff->setOmnStr(mFileName);
	buff->setI64(mSeekPos);
	buff->setU32(mBytesToRead);
	return true;
}


AosTransPtr
AosAsyncReadFileByFileNameTrans::clone()
{
	return OmnNew AosAsyncReadFileByFileNameTrans(false);
}


bool
AosAsyncReadFileByFileNameTrans::proc()
{
	AosAioCallerPtr thisPtr(this, false);
	bool rslt = AosNetFileMgr::getSelf()->asyncReadFileLocal(mFileName, mSeekPos, mBytesToRead, mReqId, thisPtr, mRdata.getPtr());
	aos_assert_r(rslt, false);
	mRdata->setOk();
	return true;
}

AosBuffPtr
AosAsyncReadFileByFileNameTrans::getIoBuff(const u64 &reqId, const int64_t &size AosMemoryCheckDecl)
{
	AosBuffPtr buff = OmnNew AosBuff(size AosMemoryCheckerFileLine);
	return buff;
}

void
AosAsyncReadFileByFileNameTrans::dataRead(const AosAioCaller::Parameter &parm, const AosBuffPtr &buff)
{
if (buff)
{
OmnScreen << "========== " << parm.reqId << " : " << buff->dataLen() << endl;
}
	//AosTransPtr trans = OmnNew AosAsyncReadFileByFileNameCbTrans(parm, mCallerId, buff, getFromSvrId(), getFromProcId(), false, false);
	//bool rslt = AosSendTrans(mRdata, trans);
	//aos_assert(rslt);
	
	AosBuffPtr resp_buff = OmnNew AosBuff(AosMemoryCheckerArgsBegin);
	resp_buff->setU32(buff->dataLen());
	if (buff->dataLen() > 0)
	{
		resp_buff->setBuff(buff);
	}
	AosAioCaller::Parameter pp = parm; 
	pp.serializeTo(resp_buff);

	sendResp(resp_buff);
}

void
AosAsyncReadFileByFileNameTrans::readError(u64 &reqid, OmnString &errmsg)
{
	OmnNotImplementedYet;
}

void
AosAsyncReadFileByFileNameTrans::dataWrite(const u64 &reqid)
{
	OmnNotImplementedYet;
}

void
AosAsyncReadFileByFileNameTrans::writeError(u64 &reqid, OmnString &errmsg)
{
	OmnNotImplementedYet;
}

bool
AosAsyncReadFileByFileNameTrans::respCallBack()
{
	AosAioCaller::Parameter parm;
	if (isSvrDeath())
	{
		OmnString errmsg = "svr death";
		mCaller->readError(mReqId, errmsg);
		mRdata->setOk();
		return true;
	}

	AosBuffPtr resp = getResp();
	AosBuffPtr data_buff;
	u32 len = resp->getU32(0);
	if (len > 0)
	{
		data_buff = resp->getBuff(len, true AosMemoryCheckerArgs);
	}
	else
	{
		data_buff = OmnNew AosBuff(AosMemoryCheckerArgsBegin);
		data_buff->setDataLen(0);
	}
	parm.serializeFrom(resp);

	aos_assert_r(mCaller, false);
	mCaller->dataRead(parm, data_buff);
	mRdata->setOk();
	return true;
}

#endif

