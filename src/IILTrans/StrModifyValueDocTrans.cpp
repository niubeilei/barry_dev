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
#include "IILTrans/StrModifyValueDocTrans.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "DocUtil/DocUtil.h"
#include "IILTrans/IILTransInclude.h"
#include "Util/Buff.h"
#include "XmlUtil/XmlTag.h"


AosIILTransStrModifyValueDoc::AosIILTransStrModifyValueDoc(const bool flag)
:
AosIILTrans(AosTransType::eStrModifyValueDoc, flag AosMemoryCheckerArgs),
mDocid(0),
mOverride(false)
{
}


AosIILTransStrModifyValueDoc::AosIILTransStrModifyValueDoc(
		const u64 &iilid,
		const OmnString &oldvalue,
		const OmnString &newvalue,
		const u64 &docid,
		const bool valueUnique,
		const bool docidUnique,
		const bool override,
		const bool need_save,
		const bool need_resp,
		const u64 snap_id)
:
AosIILTrans(AosTransType::eStrModifyValueDoc, iilid,
	false, snap_id, need_save, need_resp AosMemoryCheckerArgs),
mOldValue(oldvalue),
mNewValue(newvalue),
mDocid(docid),
mOverride(override)
{
	setValueUnique(valueUnique);
	setDocidUnique(docidUnique);
}


AosIILTransStrModifyValueDoc::AosIILTransStrModifyValueDoc(
		const OmnString &iilname,
		const OmnString &oldvalue,
		const OmnString &newvalue,
		const u64 &docid,
		const bool valueUnique,
		const bool docidUnique,
		const bool override,
		const bool need_save,
		const bool need_resp,
		const u64 snap_id)
:
AosIILTrans(AosTransType::eStrModifyValueDoc, iilname,
	false, snap_id, need_save, need_resp AosMemoryCheckerArgs),
mOldValue(oldvalue),
mNewValue(newvalue),
mDocid(docid),
mOverride(override)
{
	setValueUnique(valueUnique);
	setDocidUnique(docidUnique);
}


AosTransPtr
AosIILTransStrModifyValueDoc::clone()
{
	return OmnNew AosIILTransStrModifyValueDoc(false);
}


bool
AosIILTransStrModifyValueDoc::serializeTo(const AosBuffPtr &buff)
{
	bool rslt = AosIILTrans::serializeTo(buff);
	aos_assert_r(rslt, false);

	buff->setOmnStr(mOldValue);
	buff->setOmnStr(mNewValue);
	buff->setU64(mDocid);
	buff->setU8(mOverride);
	return true;
}


bool
AosIILTransStrModifyValueDoc::serializeFrom(const AosBuffPtr &buff)
{
	bool rslt = AosIILTrans::serializeFrom(buff);
	aos_assert_r(rslt, false);

	mOldValue = buff->getOmnStr("");
	mNewValue = buff->getOmnStr("");
	mDocid = buff->getU64(0);
	mOverride = buff->getU8(0);
	return true;
}


bool
AosIILTransStrModifyValueDoc::proc(
		const AosIILObjPtr &iilobj,
		AosBuffPtr &resp_buff,
		const AosRundataPtr &rdata)
{
	aos_assert_r(iilobj, false);
	aos_assert_r(iilobj->getIILType() == eAosIILType_Str ||
		iilobj->getIILType() == eAosIILType_BigStr, false);

	return iilobj->modifyDocSafe(mOldValue, mNewValue,
		mDocid, isValueUnique(), isDocidUnique(), rdata);
}


AosIILType 
AosIILTransStrModifyValueDoc::getIILType() const
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
AosIILTransStrModifyValueDoc::getSerializeSize() const
{
	return AosIILTrans::getSerializeSize()
		+ AosBuff::getStrSize(mIILName.length())
		+ AosBuff::getStrSize(mOldValue.length())
		+ AosBuff::getStrSize(mNewValue.length())
		+ AosBuff::getU64Size();
}

