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
#include "IILTrans/U64RemoveValueDocByNameTrans.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "Debug/Except.h"
#include "Util/Buff.h"
#include "XmlUtil/XmlTag.h"


AosIILTransU64RemoveValueDocByName::AosIILTransU64RemoveValueDocByName(const bool flag)
:
AosIILTrans(AosTransType::eU64RemoveValueDocByName, flag AosMemoryCheckerArgs),
mValue(0),
mDocid(0)
{
}


AosIILTransU64RemoveValueDocByName::AosIILTransU64RemoveValueDocByName(
		const OmnString &iilname,
		const u64 &value,
		const u64 &docid,
		const bool need_save,
		const bool need_resp,
		const u64 snap_id)
:
AosIILTrans(AosTransType::eU64RemoveValueDocByName, iilname,
	false, snap_id, need_save, need_resp AosMemoryCheckerArgs),
mValue(value),
mDocid(docid)
{
}


AosTransPtr 
AosIILTransU64RemoveValueDocByName::clone()
{
	return OmnNew AosIILTransU64RemoveValueDocByName(false);
}


bool
AosIILTransU64RemoveValueDocByName::serializeFrom(const AosBuffPtr &buff)
{
	bool rslt = AosIILTrans::serializeFrom(buff);
	aos_assert_r(rslt, false);

	mValue = buff->getU64(0);
	mDocid = buff->getU64(0);
	return true;
}


bool
AosIILTransU64RemoveValueDocByName::serializeTo(const AosBuffPtr &buff)
{
	bool rslt = AosIILTrans::serializeTo(buff);
	aos_assert_r(rslt, false);

	buff->setU64(mValue);
	buff->setU64(mDocid);
	return true;
}


bool
AosIILTransU64RemoveValueDocByName::proc(
		const AosIILObjPtr &iilobj,
		AosBuffPtr &resp_buff,
		const AosRundataPtr &rdata)
{
	aos_assert_r(iilobj, false);
	aos_assert_r(iilobj->getIILType() == eAosIILType_U64 ||
		iilobj->getIILType() == eAosIILType_BigU64, false);

	return iilobj->removeDocSafe(mValue, mDocid, rdata);
}


AosIILType 
AosIILTransU64RemoveValueDocByName::getIILType() const
{
	if (AosIsSuperIIL(mIILName))
	{
		return eAosIILType_BigU64;
	}

	return eAosIILType_U64;
}


int
AosIILTransU64RemoveValueDocByName::getSerializeSize() const
{
	return AosIILTrans::getSerializeSize()
		+ AosBuff::getStrSize(mIILName.length())
		+ AosBuff::getU64Size()
		+ AosBuff::getU64Size();
}

