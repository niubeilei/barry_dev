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
// 2014/08/06	Created By Ken Lee
////////////////////////////////////////////////////////////////////////////
#include "JobTrans/AsyncReadFileTrans.h"

#include "API/AosApi.h"
#include "XmlUtil/XmlTag.h"
#include "SEInterfaces/NetFileMgrObj.h"


AosAsyncReadFileTrans::AosAsyncReadFileTrans(const bool regflag)
:
AosTaskTrans(AosTransType::eAsyncReadFile, regflag),
mReqId(0),
mFileId(0),
mFileName(""),
mSeekPos(-1),
mBytesToRead(0)
{
}


AosAsyncReadFileTrans::AosAsyncReadFileTrans(
		const u64 &reqid,
		const AosAioCallerPtr &caller,
		const u64 &fileId,
		const int svr_id,
		const int64_t &seekPos,
		const u32 bytes_to_read)
:
AosTaskTrans(AosTransType::eAsyncReadFile, svr_id, false, true),
mReqId(reqid),
mCaller(caller),
mFileId(fileId),
mSeekPos(seekPos),
mBytesToRead(bytes_to_read)
{
}


AosAsyncReadFileTrans::AosAsyncReadFileTrans(
		const u64 &reqid,
		const AosAioCallerPtr &caller,
		const OmnString &filename,
		const int svr_id,
		const int64_t &seekPos,
		const u32 bytes_to_read)
:
AosTaskTrans(AosTransType::eAsyncReadFile, svr_id, false, true),
mReqId(reqid),
mCaller(caller),
mFileId(0),
mFileName(filename),
mSeekPos(seekPos),
mBytesToRead(bytes_to_read)
{
}


AosAsyncReadFileTrans::~AosAsyncReadFileTrans()
{
}


bool
AosAsyncReadFileTrans::serializeFrom(const AosBuffPtr &buff)
{
	bool rslt = AosTaskTrans::serializeFrom(buff);
	aos_assert_r(rslt, false);
	mReqId = buff->getU64(0);
	mFileId = buff->getU64(0);
	mFileName = buff->getOmnStr("");
	mSeekPos = buff->getI64(0);
	mBytesToRead = buff->getU32(0);
	return true;
}


bool
AosAsyncReadFileTrans::serializeTo(const AosBuffPtr &buff)
{
	bool rslt = AosTaskTrans::serializeTo(buff);
	aos_assert_r(rslt, false);
	buff->setU64(mReqId);
	buff->setU64(mFileId);
	buff->setOmnStr(mFileName);
	buff->setI64(mSeekPos);
	buff->setU32(mBytesToRead);
	return true;
}


AosTransPtr
AosAsyncReadFileTrans::clone()
{
	return OmnNew AosAsyncReadFileTrans(false);
}


bool
AosAsyncReadFileTrans::proc()
{
	bool rslt = true;
	AosAioCallerPtr thisPtr(this, false);
	if (mFileName != "")
	{
		rslt = AosNetFileMgrObj::asyncReadFileLocalStatic(
			mFileName, mSeekPos, mBytesToRead, mReqId, thisPtr, mRdata.getPtr());
	}
	else
	{
		rslt = AosNetFileMgrObj::asyncReadFileLocalStatic(
			mFileId, mSeekPos, mBytesToRead, mReqId, thisPtr, mRdata.getPtr());
	}
	if (!rslt)
	{
		OmnAlarm << "read file error" << enderr;
		AosBuffPtr resp_buff = OmnNew AosBuff(AosMemoryCheckerArgsBegin);
		resp_buff->setU8(false);
		resp_buff->setU64(mReqId);
		sendResp(resp_buff);
		return true;
	}
	aos_assert_r(rslt, false);
	mRdata->setOk();
	return true;
}


AosBuffPtr
AosAsyncReadFileTrans::getIoBuff(const u64 &reqId, const int64_t &size AosMemoryCheckDecl)
{
	AosBuffPtr buff = OmnNew AosBuff(size AosMemoryCheckerFileLine);
	return buff;
}


void
AosAsyncReadFileTrans::dataRead(const AosAioCaller::Parameter &parm, const AosBuffPtr &buff)
{
//if (buff)
//{
//OmnScreen << "========== " << parm.reqId << " : " << buff->dataLen() << endl;
//}
	
	AosBuffPtr resp_buff = OmnNew AosBuff(AosMemoryCheckerArgsBegin);
	resp_buff->setU8(true);
	
	i64 len = buff->dataLen();
	if(len <= 0)
	{
		resp_buff->setU32(len);
	}
	else if(len <= 500000)
	{
		resp_buff->setU32(len);
		resp_buff->setU8(false);
		resp_buff->setBuff(buff);
	}
	else
	{
		AosBuffPtr compressBuff = AosBuff::compress(buff);
		resp_buff->setU32(compressBuff->dataLen());
		resp_buff->setU8(true);
		resp_buff->setBuff(compressBuff);
	}

	AosAioCaller::Parameter pp = parm; 
	pp.serializeTo(resp_buff);

	sendResp(resp_buff);
}

void
AosAsyncReadFileTrans::readError(u64 &reqid, OmnString &errmsg)
{
	OmnNotImplementedYet;
}

void
AosAsyncReadFileTrans::dataWrite(const u64 &reqid)
{
	OmnNotImplementedYet;
}

void
AosAsyncReadFileTrans::writeError(u64 &reqid, OmnString &errmsg)
{
	OmnNotImplementedYet;
}

bool
AosAsyncReadFileTrans::respCallBack()
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
	bool rslt = resp->getU8(0);
	if (!rslt)
	{
		aos_assert_r(mCaller, false);
		u64 reqid = resp->getU64(0);
		OmnString errmsg;
		mCaller->readError(reqid, errmsg);
		mRdata->setOk();
		return true;
	}
	u32 len = resp->getU32(0);
	if (len > 0)
	{
		bool compressFlag = resp->getU8(false);
		data_buff = resp->getBuff(len, true AosMemoryCheckerArgs);
		if (compressFlag)
		{
			data_buff = AosBuff::unCompress(data_buff);
		}
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

