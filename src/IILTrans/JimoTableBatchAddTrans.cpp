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
#include "IILTrans/JimoTableBatchAddTrans.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "SEInterfaces/BitmapEngineObj.h"
#include "SEInterfaces/IILExecutorObj.h"
#include "SEInterfaces/IILMgrObj.h"
#include "TransUtil/TaskTransChecker.h"
#include "Util/Buff.h"
#include "Util/StrU64Array.h"
#include "XmlUtil/XmlTag.h"
#include "Util/BuffArrayVar.h"


AosIILTransJimoTableBatchAdd::AosIILTransJimoTableBatchAdd(const bool flag)
:
AosIILTrans(AosTransType::eJimoTableBatchAdd, flag AosMemoryCheckerArgs)
{
}


AosIILTransJimoTableBatchAdd::AosIILTransJimoTableBatchAdd(
		const OmnString &iilname,
		const AosXmlTagPtr &cmp_tag,
		const AosBuffPtr &buff,
		const u64 &executor_id,
		const u64 &snap_id,
		const u64 &task_docid,
		const bool check_md5,
		const OmnString &md5,
		const bool need_save,
		const bool need_resp)
:
AosIILTrans(AosTransType::eJimoTableBatchAdd, iilname,
	false, snap_id, need_save, need_resp AosMemoryCheckerArgs),
mCmpTag(cmp_tag),
mBuff(buff),
mExecutorID(executor_id),
mTaskDocid(task_docid),
mCheckMd5(check_md5),
mMd5(md5)
{
}


AosTransPtr 
AosIILTransJimoTableBatchAdd::clone()
{
	return OmnNew AosIILTransJimoTableBatchAdd(false);
}


bool
AosIILTransJimoTableBatchAdd::serializeFrom(const AosBuffPtr &buff)
{
	bool rslt = AosIILTrans::serializeFrom(buff);
	aos_assert_r(rslt, false);

	OmnString cmp_str = buff->getOmnStr("");
	aos_assert_r(cmp_str != "", false);
	mCmpTag = AosXmlParser::parse(cmp_str AosMemoryCheckerArgs);
	aos_assert_r(mCmpTag, false);

	u32 buff_len = buff->getU32(0);
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
AosIILTransJimoTableBatchAdd::serializeTo(const AosBuffPtr &buff)
{
	bool rslt = AosIILTrans::serializeTo(buff);
	aos_assert_r(rslt, false);

	aos_assert_r(mCmpTag, false);
	OmnString cmp_str = mCmpTag->toString();
	aos_assert_r(cmp_str != "", false);
	buff->setOmnStr(cmp_str);

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
AosIILTransJimoTableBatchAdd::proc(
		const AosIILObjPtr &iilobj,
		AosBuffPtr &resp_buff,
		const AosRundataPtr &rdata)
{
	aos_assert_r(iilobj, false);

	int vid = 0;
	u64 localid = 0;
	//bool rslt = AosIILMgr::getSelf()->parseIILID(mIILID, localid, vid, rdata);
	bool rslt = AosIILMgrObj::getIILMgr()->parseIILID(mIILID, localid, vid, rdata);
	aos_assert_r(rslt, false);

	rslt = AosTaskTransChecker::getSelf()->checkTrans(mTaskDocid, vid, mSnapshotId); 
	if (!rslt)
	{
		//Discard
		OmnScreen << "AosIILTransJimoTableBatchAdd Discard taskdocid:" << mTaskDocid << ";snap_id:" << mSnapshotId << ";"<< endl;
		resp_buff = OmnNew AosBuff(10 AosMemoryCheckerArgs);
		resp_buff->setU8(rslt);
		return false;
	}

	AosIILExecutorObjPtr executor = AosBitmapEngineObj::getBitmapEngine()->getIILExecutor(rdata);

	//AosJimoTablePtr jimo = dynamic_cast<AosJimoTable *>(iilobj.getPtr());
	if (iilobj->getNumDocs() == 0)
	{
		rslt = iilobj->setCmpTag(mCmpTag, rdata);
		aos_assert_r(rslt, false);
	}
	
	mBuff->reset();
	int reamin_size = 0;
	AosBuffArrayVarPtr array = AosBuffArrayVar::create(mCmpTag, true, rdata);
	rslt = array->setBodyBuff(mBuff, reamin_size);
	aos_assert_r(rslt && reamin_size == 0, false);

	//rslt = jimo->batchAddSafe(array, rdata);
	rslt = iilobj->batchAddSafe(array, rdata);
	
#ifdef AOS_SUPPORT_BITMAP_QUERY
	executor->finish(rdata, 0);
#endif
	resp_buff = OmnNew AosBuff(10 AosMemoryCheckerArgs);
	resp_buff->setU8(rslt);
	return rslt;
}


AosIILType 
AosIILTransJimoTableBatchAdd::getIILType() const
{
	return eAosIILType_JimoTable;
}


int
AosIILTransJimoTableBatchAdd::getSerializeSize() const
{
	OmnShouldNeverComeHere;
	return -1;
}

