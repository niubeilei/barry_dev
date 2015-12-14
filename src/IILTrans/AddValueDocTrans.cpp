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
#include "IILTrans/AddValueDocTrans.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "DocUtil/DocUtil.h"
#include "IILTrans/IILTransInclude.h"
#include "Util/Buff.h"
#include "XmlUtil/XmlTag.h"


AosIILTransAddValueDoc::AosIILTransAddValueDoc(const bool flag)
:
AosIILTrans(AosTransType::eAddValueDoc, flag AosMemoryCheckerArgs),
mDocid(0)
{
}


AosIILTransAddValueDoc::AosIILTransAddValueDoc(
		const u64 &iilid,
		const AosIILType &iiltype,
		const bool isPersis,
		const OmnString &value,
		const u64 &docid,
		const bool valueUnique,
		const bool docidUnique,
		const bool need_save,
		const bool need_resp,
		const u64 snap_id)
:
AosIILTrans(AosTransType::eAddValueDoc, iilid,
	isPersis, snap_id, need_save, need_resp AosMemoryCheckerArgs),
mValue(value),
mDocid(docid),
mIILType(iiltype)
{
	setValueUnique(valueUnique);
	setDocidUnique(docidUnique);
}


AosIILTransAddValueDoc::AosIILTransAddValueDoc(
		const OmnString &iilname,
		const AosIILType &iiltype,
		const bool isPersis,
		const OmnString &value,
		const u64 &docid,
		const bool valueUnique,
		const bool docidUnique,
		const bool need_save,
		const bool need_resp,
		const u64 snap_id)
:
AosIILTrans(AosTransType::eAddValueDoc, iilname,
	isPersis, snap_id, need_save, need_resp AosMemoryCheckerArgs),
mValue(value),
mDocid(docid),
mIILType(iiltype)
{
	setValueUnique(valueUnique);
	setDocidUnique(docidUnique);
}


AosTransPtr
AosIILTransAddValueDoc::clone()
{
	return OmnNew AosIILTransAddValueDoc(false);
}



bool
AosIILTransAddValueDoc::serializeFrom(const AosBuffPtr &buff)
{
	bool rslt = AosIILTrans::serializeFrom(buff);
	aos_assert_r(rslt, false);

	mValue = buff->getOmnStr("");
	mDocid = buff->getU64(0);
	mIILType = (AosIILType)buff->getU32(0);
	return true;
}

bool
AosIILTransAddValueDoc::serializeTo(const AosBuffPtr &buff)
{
	bool rslt = AosIILTrans::serializeTo(buff);
	aos_assert_r(rslt, false);

	buff->setOmnStr(mValue);
	buff->setU64(mDocid);
	buff->setU32((u32)mIILType);
	return true;
}


bool
AosIILTransAddValueDoc::proc(
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

	bool rslt = false;
	switch(mIILType)
	{
		case eAosIILType_BigStr:
			rslt = iilobj->addDocSafe(mValue, mDocid, isValueUnique(), isDocidUnique(), rdata);
			break;
		case eAosIILType_BigU64:
			rslt = iilobj->addDocSafe(mValue.toU64(), mDocid, isValueUnique(), isDocidUnique(), rdata);
			break;
		case eAosIILType_BigI64:
			rslt = iilobj->addDocSafe(mValue.toI64(), mDocid, isValueUnique(), isDocidUnique(), rdata);
			break;
		case eAosIILType_BigD64:
			rslt = iilobj->addDocSafe(mValue.toD64(), mDocid, isValueUnique(), isDocidUnique(), rdata);
			break;
		default:
			break;
	} 
	return rslt;

	
}


AosIILType 
AosIILTransAddValueDoc::getIILType() const
{
	if(mIILType == eAosIILType_Str)
	{
		bool isNumAlpha = AosDocUtil::isNumAlpha(mIILName.data(), mIILName.length());
		if(isNumAlpha) return eAosIILType_NumAlpha;
	}
	return mIILType;
}


int
AosIILTransAddValueDoc::getSerializeSize() const
{
	return AosIILTrans::getSerializeSize()
		+ AosBuff::getStrSize(mIILName.length())
		+ AosBuff::getStrSize(mValue.length())
		+ AosBuff::getU64Size()
		+ AosBuff::getU32Size();
}

