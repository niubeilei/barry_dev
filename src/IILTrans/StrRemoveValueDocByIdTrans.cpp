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
#include "IILTrans/StrRemoveValueDocByIdTrans.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "Debug/Except.h"
#include "Util/Buff.h"
#include "XmlUtil/XmlTag.h"


AosIILTransStrRemoveValueDocById::AosIILTransStrRemoveValueDocById(const bool flag)
:
AosIILTrans(AosTransType::eStrRemoveValueDocById, flag AosMemoryCheckerArgs),
mValue(""),
mDocid(0)
{
}


AosIILTransStrRemoveValueDocById::AosIILTransStrRemoveValueDocById(
		const u64 &iilid,
		const OmnString &value,
		const u64 &docid,
		const bool need_save,
		const bool need_resp,
		const u64 snap_id)
:
AosIILTrans(AosTransType::eStrRemoveValueDocById, 
		iilid, true, snap_id, need_save, need_resp AosMemoryCheckerArgs),
mValue(value),
mDocid(docid)
{
}


AosTransPtr 
AosIILTransStrRemoveValueDocById::clone()
{
	return OmnNew AosIILTransStrRemoveValueDocById(false);
}


bool
AosIILTransStrRemoveValueDocById::serializeFrom(const AosBuffPtr &buff)
{
	bool rslt = AosIILTrans::serializeFrom(buff);
	aos_assert_r(rslt, false);

	mValue = buff->getOmnStr("");
	mDocid = buff->getU64(0);
	return true;
}


bool
AosIILTransStrRemoveValueDocById::serializeTo(const AosBuffPtr &buff)
{
	bool rslt = AosIILTrans::serializeTo(buff);
	aos_assert_r(rslt, false);
	
	buff->setOmnStr(mValue);
	buff->setU64(mDocid);
	return true;
}


bool
AosIILTransStrRemoveValueDocById::proc(
		const AosIILObjPtr &iilobj, 
		AosBuffPtr &resp_buff,
		const AosRundataPtr &rdata)
{
	aos_assert_rr(iilobj, rdata, false);
	aos_assert_rr(iilobj->getIILType() == eAosIILType_Str, rdata, false);

	return iilobj->removeDocSafe(mValue, mDocid, rdata);
}


AosIILType 
AosIILTransStrRemoveValueDocById::getIILType() const
{
	return eAosIILType_Str;
}


int
AosIILTransStrRemoveValueDocById::getSerializeSize() const
{
	return AosIILTrans::getSerializeSize() + 
		AosBuff::getStrSize(mValue.length()) +
		AosBuff::getU64Size();
}

