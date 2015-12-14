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
// 03/25/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "IILTrans/U64BatchAddTrans.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "IILMgr/IIL.h"
#include "SEInterfaces/BitmapEngineObj.h"
#include "SEInterfaces/IILExecutorObj.h"
#include "SEInterfaces/IILMgrObj.h"
#include "TransUtil/TaskTransChecker.h"
#include "Util/Buff.h"
#include "Util/StrU64Array.h"
#include "XmlUtil/XmlTag.h"


AosIILTransU64BatchAdd::AosIILTransU64BatchAdd(const bool flag)
:
AosIILTrans(AosTransType::eU64BatchAdd, flag AosMemoryCheckerArgs)
{
}


AosIILTransU64BatchAdd::AosIILTransU64BatchAdd(
		const OmnString &iilname,
		const int len,
		const AosBuffPtr &buff,
		const u64 &executor_id,
		const u64 &snap_id,
		const u64 &task_docid,
		const bool check_md5,
		const OmnString &md5,
		const bool need_save,
		const bool need_resp)
:
AosIILTrans(AosTransType::eU64BatchAdd, iilname,
	false, snap_id, need_save, need_resp AosMemoryCheckerArgs),
mLen(len),
mBuff(buff),
mExecutorID(executor_id),
mTaskDocid(task_docid),
mCheckMd5(check_md5),
mMd5(md5)
{
}


AosTransPtr 
AosIILTransU64BatchAdd::clone()
{
	return OmnNew AosIILTransU64BatchAdd(false);
}


bool
AosIILTransU64BatchAdd::serializeFrom(const AosBuffPtr &buff)
{
	bool rslt = AosIILTrans::serializeFrom(buff);
	aos_assert_r(rslt, false);

	mLen = buff->getInt(-1);
	//aos_assert_r(mLen >= (int)(sizeof(u64) * 2 + 1), false);
	aos_assert_r(mLen >= (int)(sizeof(u64) * 2), false);

	u32 buff_len = buff->getU32(0);
	aos_assert_r(buff_len % mLen == 0, false);

	mBuff = buff->getBuff(buff_len, true AosMemoryCheckerArgs);	
	mExecutorID = buff->getU64(0);

	mTaskDocid = buff->getU64(0);
	aos_assert_r(mTaskDocid, false);

	mCheckMd5 = buff->getU8(0);
	mMd5 = buff->getOmnStr("");
	aos_assert_r(mBuff, false);

	if (mCheckMd5)
	{
		OmnString v(mBuff->data(), mBuff->dataLen());
		OmnString md5_now = AosMD5Encrypt(v);
		if (mMd5 != md5_now)
		{
			OmnAlarm << "md5 error, should" << mMd5 << ",now:" << md5_now << enderr;
			return false;
		}
	}

	return true;
}


bool
AosIILTransU64BatchAdd::serializeTo(const AosBuffPtr &buff)
{
	bool rslt = AosIILTrans::serializeTo(buff);
	aos_assert_r(rslt, false);

	buff->setInt(mLen);
	buff->setU32(mBuff->dataLen());
	buff->setBuff(mBuff);
	buff->setU64(mExecutorID);

	aos_assert_r(mTaskDocid != 0, false);
	buff->setU64(mTaskDocid);

	buff->setU8(mCheckMd5);
	buff->setOmnStr(mMd5);

	return true;
}


bool
AosIILTransU64BatchAdd::proc(
		const AosIILObjPtr &iilobj,
		AosBuffPtr &resp_buff,
		const AosRundataPtr &rdata)
{
	aos_assert_r(iilobj, false);
	//Linda, 2014/03/13
	int vid = 0;
	u64 localid = 0;
	//bool rslt = AosIILMgr::getSelf()->parseIILID(mIILID, localid, vid, rdata);
	bool rslt = AosIILMgrObj::getIILMgr()->parseIILID(mIILID, localid, vid, rdata);
	aos_assert_r(rslt, false);

	rslt = AosTaskTransChecker::getSelf()->checkTrans(mTaskDocid, vid, mSnapshotId); 
	if (!rslt)
	{
		//Discard
		OmnScreen << "AosIILTransU64BatchAdd Discard taskdocid:" << mTaskDocid << ";snap_id:" << mSnapshotId << ";"<< endl;
		return false;
	}

	//AosIILExecutorObjPtr executor = AosBitmapEngineObj::getBitmapEngine()->getIILExecutorByID(mExecutorID, rdata);
	AosIILExecutorObjPtr executor = AosBitmapEngineObj::getBitmapEngine()->getIILExecutor(rdata);
	if(!mExecutorID)
	{
		executor->disable();
	}
	if(mExecutorID == eStreamingExecutorId)
	{
		executor->disable();
		executor->setExecutorID(mExecutorID);
	}

	char * entries = mBuff->data();
	int64_t num = mBuff->dataLen() / mLen;
	rslt = iilobj->batchAddSafe(
		entries, mLen, num, executor, rdata);
	
#ifdef AOS_SUPPORT_BITMAP_QUERY
	executor->finish(rdata, 0);
#endif
	resp_buff = OmnNew AosBuff(10 AosMemoryCheckerArgs);
	resp_buff->setU8(rslt);
	return rslt;
}


AosIILType 
AosIILTransU64BatchAdd::getIILType() const
{
	if (AosIsSuperIIL(mIILName))
	{
		return eAosIILType_BigU64;
	}
	return eAosIILType_U64;
}


int
AosIILTransU64BatchAdd::getSerializeSize() const
{
	OmnShouldNeverComeHere;
	return -1;
}

