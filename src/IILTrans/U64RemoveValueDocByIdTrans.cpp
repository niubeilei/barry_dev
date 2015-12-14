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
#include "IILTrans/U64RemoveValueDocByIdTrans.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "Debug/Except.h"
#include "Util/Buff.h"
#include "XmlUtil/XmlTag.h"


AosIILTransU64RemoveValueDocById::AosIILTransU64RemoveValueDocById(const bool flag)
:
AosIILTrans(AosTransType::eU64RemoveValueDocById, flag AosMemoryCheckerArgs)
{
}


AosIILTransU64RemoveValueDocById::AosIILTransU64RemoveValueDocById(
		const u64 &iilid,
		const u64 &value,
		const u64 &docid,
		const bool need_save,
		const bool need_resp,
		const u64 snap_id)
:
AosIILTrans(AosTransType::eU64RemoveValueDocById,
		iilid, false, snap_id, need_save, need_resp AosMemoryCheckerArgs),
mValue(value),
mDocid(docid)
{
}


AosTransPtr 
AosIILTransU64RemoveValueDocById::clone()
{
	return OmnNew AosIILTransU64RemoveValueDocById(false);
}


bool
AosIILTransU64RemoveValueDocById::serializeFrom(const AosBuffPtr &buff)
{
	bool rslt = AosIILTrans::serializeFrom(buff);
	aos_assert_r(rslt, false);

	mValue = buff->getU64(0);
	mDocid = buff->getU64(0);
	return true;
}


bool
AosIILTransU64RemoveValueDocById::serializeTo(const AosBuffPtr &buff)
{
	bool rslt = AosIILTrans::serializeTo(buff);
	aos_assert_r(rslt, false);

	buff->setU64(mValue);
	buff->setU64(mDocid);
	return true;
}


bool
AosIILTransU64RemoveValueDocById::proc(
		const AosIILObjPtr &iilobj,
		AosBuffPtr &resp_buff,
		const AosRundataPtr &rdata)
{
	aos_assert_rr(iilobj, rdata, false);
	aos_assert_rr(iilobj->getIILType() == eAosIILType_U64, rdata, false);

	return iilobj->removeDocSafe(mValue, mDocid, rdata);
}


AosIILType 
AosIILTransU64RemoveValueDocById::getIILType() const
{
	return eAosIILType_U64;
}


int
AosIILTransU64RemoveValueDocById::getSerializeSize() const
{
	return AosIILTrans::getSerializeSize() 
		+ AosBuff::getU64Size()
		+ AosBuff::getU64Size();
}

