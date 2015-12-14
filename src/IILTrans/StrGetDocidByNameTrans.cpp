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
#include "IILTrans/StrGetDocidByNameTrans.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "Debug/Except.h"
#include "DocUtil/DocUtil.h"
#include "Util/Buff.h"
#include "XmlUtil/XmlTag.h"


AosIILTransStrGetDocidByName::AosIILTransStrGetDocidByName(const bool flag)
:
AosIILTrans(AosTransType::eStrGetDocidByName, flag AosMemoryCheckerArgs)
{
}


AosIILTransStrGetDocidByName::AosIILTransStrGetDocidByName(
		const OmnString &iilname,
		const OmnString &key,
		const AosOpr opr,
		const bool reverse,
		const bool need_save,
		const bool need_resp,
		const u64 snap_id)
:
AosIILTrans(AosTransType::eStrGetDocidByName, iilname,
	false, snap_id, need_save, need_resp AosMemoryCheckerArgs),
mKey(key),
mOpr(opr),
mReverse(reverse)
{
}


AosTransPtr 
AosIILTransStrGetDocidByName::clone()
{
	return OmnNew AosIILTransStrGetDocidByName(false);
}


bool
AosIILTransStrGetDocidByName::serializeFrom(const AosBuffPtr &buff)
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

	return true;
}


bool
AosIILTransStrGetDocidByName::serializeTo(const AosBuffPtr &buff)
{
	bool rslt = AosIILTrans::serializeTo(buff);
	aos_assert_r(rslt, false);

	buff->setOmnStr(mKey);
	buff->setU32(mOpr);
	buff->setU8(mReverse);
	return true;
}


bool
AosIILTransStrGetDocidByName::proc(
		const AosIILObjPtr &iilobj,
		AosBuffPtr &resp_buff,
		const AosRundataPtr &rdata)
{
	aos_assert_r(iilobj, false);
	aos_assert_r(iilobj->getIILType() == eAosIILType_Str ||
		iilobj->getIILType() == eAosIILType_BigStr, false);

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
AosIILTransStrGetDocidByName::getIILType() const
{
	if (AosIsSuperIIL(mIILName))
	{
		return eAosIILType_BigStr;
	}

	bool isNumAlpha = AosDocUtil::isNumAlpha(mIILName.data(), mIILName.length());
	if (isNumAlpha)
	{
		return eAosIILType_NumAlpha;
	}
	return eAosIILType_Str;
}

	
int
AosIILTransStrGetDocidByName::getSerializeSize() const
{
	// Ketty 2012/11/09
	return AosIILTrans::getSerializeSize()
		+ AosBuff::getStrSize(mIILName.length()) 
		+ AosBuff::getStrSize(mKey.length())
		//+ AosBuff::getU32Size() + AosBuff::getU8Size();
		+ AosBuff::getU32Size() + sizeof(u8);
	
	//OmnNotImplementedYet;
	//return 0;
}
	

bool
AosIILTransStrGetDocidByName::procGetResp()
{
	// Ketty 2013/09/16
	return AosIILTrans::proc();
}

