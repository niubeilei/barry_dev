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
// 10/06/2011	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "IILTrans/GetDocidTrans.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "Debug/Except.h"
#include "DocUtil/DocUtil.h"
#include "Util/Buff.h"
#include "XmlUtil/XmlTag.h"


AosIILTransGetDocid::AosIILTransGetDocid(const bool flag)
:
AosIILTrans(AosTransType::eGetDocid, flag AosMemoryCheckerArgs)
{
}


AosIILTransGetDocid::AosIILTransGetDocid(
		const OmnString &iilname,
		const AosIILType &iiltype,
		const OmnString &key,
		const AosOpr opr,
		const bool reverse,
		const bool need_save,
		const bool need_resp,
		const u64 snap_id)
:
AosIILTrans(AosTransType::eGetDocid, iilname,
	false, snap_id, need_save, need_resp AosMemoryCheckerArgs),
mKey(key),
mOpr(opr),
mReverse(reverse)
{
}

AosIILTransGetDocid::AosIILTransGetDocid(
		const u64 &iilid,
		const AosIILType &iiltype,
		const OmnString &key,
		const AosOpr opr,
		const bool reverse,
		const bool need_save,
		const bool need_resp,
		const u64 snap_id)
:
AosIILTrans(AosTransType::eGetDocid, iilid,
	false, snap_id, need_save, need_resp AosMemoryCheckerArgs),
mKey(key),
mOpr(opr),
mReverse(reverse)
{
}


AosTransPtr 
AosIILTransGetDocid::clone()
{
	return OmnNew AosIILTransGetDocid(false);
}


bool
AosIILTransGetDocid::serializeFrom(const AosBuffPtr &buff)
{
	bool rslt = AosIILTrans::serializeFrom(buff);
	aos_assert_r(rslt, false);

	mKey = buff->getOmnStr("");
	mOpr = (AosOpr)buff->getU32(0);
	mReverse = buff->getU8(0);

	if (mKey == "" || !AosOpr_valid(mOpr) || mIILName == "")
	{
		OmnAlarm << "error!" << enderr;
		return false;
	}
	mIILType = (AosIILType)buff->getU32(0);

	return true;
}


bool
AosIILTransGetDocid::serializeTo(const AosBuffPtr &buff)
{
	bool rslt = AosIILTrans::serializeTo(buff);
	aos_assert_r(rslt, false);

	buff->setOmnStr(mKey);
	buff->setU32(mOpr);
	buff->setU8(mReverse);
	buff->setU32((u32)mIILType);
	return true;
}


bool
AosIILTransGetDocid::proc(
		const AosIILObjPtr &iilobj,
		AosBuffPtr &resp_buff,
		const AosRundataPtr &rdata)
{
	aos_assert_r(iilobj, false);
	aos_assert_r(iilobj->getIILType() == eAosIILType_Str ||
		iilobj->getIILType() == eAosIILType_BigStr ||
		iilobj->getIILType() == eAosIILType_BigU64 || 
		iilobj->getIILType() == eAosIILType_BigI64 || 
		iilobj->getIILType() == eAosIILType_BigD64, false);

	u64 docid = 0;
	bool isunique = false;
	int64_t idx = -10;
	int64_t iilidx = -10;
	AosIILIdx the_idx;
	bool rslt = true;
    
	rslt = iilobj->nextDocidSafe(the_idx, idx, iilidx,
		mReverse, mOpr, mKey, docid, isunique, rdata); 

	resp_buff = OmnNew AosBuff(50 AosMemoryCheckerArgs);
	resp_buff->setU8(rslt);
	resp_buff->setU64(docid);
	resp_buff->setU8(isunique);

	return true;
}


AosIILType 
AosIILTransGetDocid::getIILType() const
{
	if(mIILType == eAosIILType_Str)
	{
		bool isNumAlpha = AosDocUtil::isNumAlpha(mIILName.data(), mIILName.length());
		if(isNumAlpha) return eAosIILType_NumAlpha;
	}
	return mIILType;
}

	
int
AosIILTransGetDocid::getSerializeSize() const
{
	// Ketty 2012/11/09
	return AosIILTrans::getSerializeSize()
		+ AosBuff::getStrSize(mIILName.length()) 
		+ AosBuff::getStrSize(mKey.length())
		+ AosBuff::getU32Size() + sizeof(u8)
		+ AosBuff::getU32Size();
	
}
	

bool
AosIILTransGetDocid::procGetResp()
{
	// Ketty 2013/09/16
	return AosIILTrans::proc();
}

