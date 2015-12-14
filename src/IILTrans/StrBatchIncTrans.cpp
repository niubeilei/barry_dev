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
#include "IILTrans/StrBatchIncTrans.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "SEInterfaces/IILMgrObj.h"
#include "TransUtil/TaskTransChecker.h"
#include "Util/Buff.h"
#include "Util/StrU64Array.h"
#include "XmlUtil/XmlTag.h"


AosIILTransStrBatchInc::AosIILTransStrBatchInc(const bool flag)
:
AosIILTrans(AosTransType::eStrBatchInc, flag AosMemoryCheckerArgs),
mLen(-1),
mInitdocid(0),
mTaskDocid(0),
mCheckMd5(false),
mMd5(""),
mCompressFlag(false)
{
}


AosIILTransStrBatchInc::AosIILTransStrBatchInc(
		const OmnString &iilname,
		const int entry_len,
		const AosBuffPtr &buff,
		const u64 init_value,
		const AosIILUtil::AosIILIncType inc_type,
		const u64 &snap_id,
		const u64 &task_docid,
		const bool check_md5,
		const OmnString &md5,
		const bool need_save,
		const bool need_resp)
:
AosIILTrans(AosTransType::eStrBatchInc, iilname, 
		false, snap_id, need_save, need_resp AosMemoryCheckerArgs),
mLen(entry_len),
mBuff(buff),
mInitdocid(init_value),
mIncType(inc_type),
mTaskDocid(task_docid),
mCheckMd5(check_md5),
mMd5(md5),
mCompressFlag(false)
{
	if(buff->dataLen() > 500000)
	{
		mBuff = AosBuff::compress(buff);
		mCompressFlag = true;
	}
}


AosTransPtr 
AosIILTransStrBatchInc::clone()
{
	return OmnNew AosIILTransStrBatchInc(false);
}


bool
AosIILTransStrBatchInc::serializeFrom(const AosBuffPtr &buff)
{
	bool rslt = AosIILTrans::serializeFrom(buff);
	aos_assert_r(rslt, false);
	
	mLen = buff->getInt(-1);
	
	u32 data_len = buff->getU32(0);
	mBuff = buff->getBuff(data_len, true AosMemoryCheckerArgs);

	mInitdocid = buff->getU64(0);
	mIncType = (AosIILUtil::AosIILIncType)buff->getU32(0);
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
	mCompressFlag = buff->getU8(0);
	
	return true;
}


bool
AosIILTransStrBatchInc::serializeTo(const AosBuffPtr &buff)
{
	bool rslt = AosIILTrans::serializeTo(buff);
	aos_assert_r(rslt, false);

	buff->setInt(mLen);
	buff->setU32(mBuff->dataLen());
	buff->setBuff(mBuff);
	buff->setU64(mInitdocid);
	buff->setU32(mIncType);
	buff->setU64(mTaskDocid);
	buff->setU8(mCheckMd5);
	buff->setOmnStr(mMd5);
	buff->setU8(mCompressFlag);
	return true;
}


bool
AosIILTransStrBatchInc::proc(
		const AosIILObjPtr &iilobj,
		AosBuffPtr &resp_buff,
		const AosRundataPtr &rdata)
{
	aos_assert_r(iilobj, false);
	aos_assert_r(iilobj->getIILType() == eAosIILType_BigStr, false);

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
		OmnScreen << "AosIILTransStrBatchInc Discard taskdocid:" << mTaskDocid << ";snap_id:" << mSnapshotId << ";"<< endl;
		return false;
	}

	if (mCompressFlag)
	{
		mBuff = AosBuff::unCompress(mBuff);
	}
	char * entries = mBuff->data();
	int64_t num = mBuff->dataLen() / mLen;
	rslt = iilobj->batchIncSafe(
		entries, mLen, num, mInitdocid, mIncType, rdata);

	resp_buff = OmnNew AosBuff(10 AosMemoryCheckerArgs);
	resp_buff->setU8(rslt);

	return true;
}


AosIILType 
AosIILTransStrBatchInc::getIILType() const
{
	return eAosIILType_BigStr;
}


int
AosIILTransStrBatchInc::getSerializeSize() const
{
	OmnShouldNeverComeHere;
	return -1;
}

