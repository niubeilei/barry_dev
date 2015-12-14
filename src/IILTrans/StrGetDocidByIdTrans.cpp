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
#include "IILTrans/StrGetDocidByIdTrans.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "Debug/Except.h"
#include "Util/Buff.h"
#include "XmlUtil/XmlTag.h"


AosIILTransStrGetDocidById::AosIILTransStrGetDocidById(const bool flag)
:
AosIILTrans(AosTransType::eStrGetDocidById, flag AosMemoryCheckerArgs)
{
}


AosIILTransStrGetDocidById::AosIILTransStrGetDocidById(
		const u64 iilid,
		const OmnString &key,
		const AosOpr opr,
		const bool reverse,
		const bool need_save,
		const bool need_resp,
		const u64 snap_id)
:
AosIILTrans(AosTransType::eStrGetDocidById, iilid, 
	false, snap_id, need_save, need_resp AosMemoryCheckerArgs),
mKey(key),
mOpr(opr),
mReverse(reverse)
{
}


AosTransPtr 
AosIILTransStrGetDocidById::clone()
{
	return OmnNew AosIILTransStrGetDocidById(false);
}


bool
AosIILTransStrGetDocidById::serializeFrom(const AosBuffPtr &buff)
{
	bool rslt = AosIILTrans::serializeFrom(buff);
	aos_assert_r(rslt, false);

	mKey = buff->getOmnStr("");
	mOpr = (AosOpr)buff->getU32(0);
	mReverse = buff->getU8(0);

	return true;
}


bool
AosIILTransStrGetDocidById::serializeTo(const AosBuffPtr &buff)
{
	bool rslt = AosIILTrans::serializeTo(buff);
	aos_assert_r(rslt, false);

	buff->setOmnStr(mKey);
	buff->setU32(mOpr);
	buff->setU8(mReverse);
	return true;
}


bool
AosIILTransStrGetDocidById::proc(
		const AosIILObjPtr &iilobj,
		AosBuffPtr &resp_buff,
		const AosRundataPtr &rdata)
{
	aos_assert_r(iilobj, false);

	aos_assert_r(iilobj->getIILType() == eAosIILType_Str, false);

	u64 docid = 0;
	bool isunique = false;
	int64_t idx = -10;
	int64_t iilidx = -10;
	AosIILIdx the_idx;
	bool rslt = iilobj->nextDocidSafe(the_idx, idx, iilidx,
		mReverse, mOpr, mKey, docid, isunique, rdata); 
	
	resp_buff = OmnNew AosBuff(50 AosMemoryCheckerArgs);
	resp_buff->setU8(rslt);
	resp_buff->setU64(docid);
	resp_buff->setU8(isunique);

	return true;
}


AosIILType 
AosIILTransStrGetDocidById::getIILType() const
{
	return eAosIILType_Str;
}

	
int
AosIILTransStrGetDocidById::getSerializeSize() const
{
	OmnNotImplementedYet;
	return 0;
}

