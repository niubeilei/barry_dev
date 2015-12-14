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
#include "IILTrans/StrRemoveValueDocByNameTrans.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "Debug/Except.h"
#include "DocUtil/DocUtil.h"
#include "Util/Buff.h"
#include "XmlUtil/XmlTag.h"


AosIILTransStrRemoveValueDocByName::AosIILTransStrRemoveValueDocByName(const bool flag)
:
AosIILTrans(AosTransType::eStrRemoveValueDocByName, flag AosMemoryCheckerArgs),
mDocid(0)
{
}


AosIILTransStrRemoveValueDocByName::AosIILTransStrRemoveValueDocByName(
		const OmnString &iilname,
		const OmnString &value,
		const u64 &docid,
		const bool need_save,
		const bool need_resp,
		const u64 snap_id)
:
AosIILTrans(AosTransType::eStrRemoveValueDocByName, iilname,
	false, snap_id, need_save, need_resp AosMemoryCheckerArgs),
mValue(value),
mDocid(docid)
{
}


AosTransPtr 
AosIILTransStrRemoveValueDocByName::clone()
{
	return OmnNew AosIILTransStrRemoveValueDocByName(false);
}


bool
AosIILTransStrRemoveValueDocByName::serializeFrom(const AosBuffPtr &buff)
{
	bool rslt = AosIILTrans::serializeFrom(buff);
	aos_assert_r(rslt, false);

	mValue = buff->getOmnStr("");
	mDocid = buff->getU64(0);
	return true;
}


bool
AosIILTransStrRemoveValueDocByName::serializeTo(const AosBuffPtr &buff)
{
	bool rslt = AosIILTrans::serializeTo(buff);
	aos_assert_r(rslt, false);

	buff->setOmnStr(mValue);
	buff->setU64(mDocid);
	return true;
}


bool
AosIILTransStrRemoveValueDocByName::proc(
		const AosIILObjPtr &iilobj,
		AosBuffPtr &resp_buff,
		const AosRundataPtr &rdata)
{
	aos_assert_r(iilobj, false);
	aos_assert_r(iilobj->getIILType() == eAosIILType_Str ||
		iilobj->getIILType() == eAosIILType_BigStr, false);

	return iilobj->removeDocSafe(mValue, mDocid, rdata);
}


AosIILType 
AosIILTransStrRemoveValueDocByName::getIILType() const
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
AosIILTransStrRemoveValueDocByName::getSerializeSize() const
{
	return AosIILTrans::getSerializeSize()
		+ AosBuff::getStrSize(mIILName.length())
		+ AosBuff::getStrSize(mValue.length())
		+ AosBuff::getU64Size();
}

