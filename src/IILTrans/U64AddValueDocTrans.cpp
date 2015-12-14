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
// 2014/05/20 Created By Ken Lee
////////////////////////////////////////////////////////////////////////////
#include "IILTrans/U64AddValueDocTrans.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "IILTrans/IILTransInclude.h"
#include "Util/Buff.h"
#include "XmlUtil/XmlTag.h"


AosIILTransU64AddValueDoc::AosIILTransU64AddValueDoc(const bool flag)
:
AosIILTrans(AosTransType::eU64AddValueDoc, flag AosMemoryCheckerArgs),
mValue(0),
mDocid(0)
{
}


AosIILTransU64AddValueDoc::AosIILTransU64AddValueDoc(
		const u64 &iilid,
		const bool isPersis,
		const u64 &value,
		const u64 &docid,
		const bool valueUnique,
		const bool docidUnique,
		const bool need_save,
		const bool need_resp,
		const u64 snap_id)
:
AosIILTrans(AosTransType::eU64AddValueDoc, iilid,
	isPersis, snap_id, need_save, need_resp AosMemoryCheckerArgs),
mValue(value),
mDocid(docid)
{
	setValueUnique(valueUnique);
	setDocidUnique(docidUnique);
}


AosIILTransU64AddValueDoc::AosIILTransU64AddValueDoc(
		const OmnString &iilname,
		const bool isPersis,
		const u64 &value,
		const u64 &docid,
		const bool valueUnique,
		const bool docidUnique,
		const bool need_save,
		const bool need_resp,
		const u64 snap_id)
:
AosIILTrans(AosTransType::eU64AddValueDoc, iilname,
	isPersis, snap_id, need_save, need_resp AosMemoryCheckerArgs),
mValue(value),
mDocid(docid)
{
	setValueUnique(valueUnique);
	setDocidUnique(docidUnique);
}


AosTransPtr
AosIILTransU64AddValueDoc::clone()
{
	return OmnNew AosIILTransU64AddValueDoc(false);
}


bool
AosIILTransU64AddValueDoc::serializeTo(const AosBuffPtr &buff)
{
	bool rslt = AosIILTrans::serializeTo(buff);
	aos_assert_r(rslt, false);

	buff->setU64(mValue);
	buff->setU64(mDocid);
	return true;
}


bool
AosIILTransU64AddValueDoc::serializeFrom(const AosBuffPtr &buff)
{
	bool rslt = AosIILTrans::serializeFrom(buff);
	aos_assert_r(rslt, false);

	mValue = buff->getU64(0);
	mDocid = buff->getU64(0);
	return true;
}


bool
AosIILTransU64AddValueDoc::proc(
		const AosIILObjPtr &iilobj,
		AosBuffPtr &resp_buff,
		const AosRundataPtr &rdata)
{
	aos_assert_r(iilobj, false);
	aos_assert_r(iilobj->getIILType() == eAosIILType_U64 ||
		iilobj->getIILType() == eAosIILType_BigU64, false);

	return iilobj->addDocSafe(mValue, mDocid,
		isValueUnique(), isDocidUnique(), rdata);
}


AosIILType 
AosIILTransU64AddValueDoc::getIILType() const
{
	if (AosIsSuperIIL(mIILName))
	{
		return eAosIILType_BigU64;
	}
	return eAosIILType_U64;
}


int
AosIILTransU64AddValueDoc::getSerializeSize() const
{
	return AosIILTrans::getSerializeSize()
		+ AosBuff::getStrSize(mIILName.length())
		+ AosBuff::getU64Size()
		+ AosBuff::getU64Size();
}

