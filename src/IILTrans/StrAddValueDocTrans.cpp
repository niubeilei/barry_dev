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
// 2014/05/19 Created By Ken Lee
////////////////////////////////////////////////////////////////////////////
#include "IILTrans/StrAddValueDocTrans.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "DocUtil/DocUtil.h"
#include "IILTrans/IILTransInclude.h"
#include "Util/Buff.h"
#include "XmlUtil/XmlTag.h"


AosIILTransStrAddValueDoc::AosIILTransStrAddValueDoc(const bool flag)
:
AosIILTrans(AosTransType::eStrAddValueDoc, flag AosMemoryCheckerArgs),
mDocid(0)
{
}


AosIILTransStrAddValueDoc::AosIILTransStrAddValueDoc(
		const u64 &iilid,
		const bool isPersis,
		const OmnString &value,
		const u64 &docid,
		const bool valueUnique,
		const bool docidUnique,
		const bool need_save,
		const bool need_resp,
		const u64 snap_id)
:
AosIILTrans(AosTransType::eStrAddValueDoc, iilid,
	isPersis, snap_id, need_save, need_resp AosMemoryCheckerArgs),
mValue(value),
mDocid(docid)
{
	setValueUnique(valueUnique);
	setDocidUnique(docidUnique);
}


AosIILTransStrAddValueDoc::AosIILTransStrAddValueDoc(
		const OmnString &iilname,
		const bool isPersis,
		const OmnString &value,
		const u64 &docid,
		const bool valueUnique,
		const bool docidUnique,
		const bool need_save,
		const bool need_resp,
		const u64 snap_id)
:
AosIILTrans(AosTransType::eStrAddValueDoc, iilname,
	isPersis, snap_id, need_save, need_resp AosMemoryCheckerArgs),
mValue(value),
mDocid(docid)
{
	setValueUnique(valueUnique);
	setDocidUnique(docidUnique);
}


AosTransPtr
AosIILTransStrAddValueDoc::clone()
{
	return OmnNew AosIILTransStrAddValueDoc(false);
}


bool
AosIILTransStrAddValueDoc::serializeTo(const AosBuffPtr &buff)
{
	bool rslt = AosIILTrans::serializeTo(buff);
	aos_assert_r(rslt, false);

	buff->setOmnStr(mValue);
	buff->setU64(mDocid);
	return true;
}


bool
AosIILTransStrAddValueDoc::serializeFrom(const AosBuffPtr &buff)
{
	bool rslt = AosIILTrans::serializeFrom(buff);
	aos_assert_r(rslt, false);

	mValue = buff->getOmnStr("");
	mDocid = buff->getU64(0);
	return true;
}


bool
AosIILTransStrAddValueDoc::proc(
		const AosIILObjPtr &iilobj,
		AosBuffPtr &resp_buff,
		const AosRundataPtr &rdata)
{
	aos_assert_r(iilobj, false);
	aos_assert_r(iilobj->getIILType() == eAosIILType_Str ||
		iilobj->getIILType() == eAosIILType_BigStr, false);

	return iilobj->addDocSafe(mValue, mDocid,
		isValueUnique(), isDocidUnique(), rdata);
}


AosIILType 
AosIILTransStrAddValueDoc::getIILType() const
{
	if (mIILName != "")
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
	}

	return eAosIILType_Str;
}


int
AosIILTransStrAddValueDoc::getSerializeSize() const
{
	return AosIILTrans::getSerializeSize()
		+ AosBuff::getStrSize(mIILName.length())
		+ AosBuff::getStrSize(mValue.length())
		+ AosBuff::getU64Size();
}

