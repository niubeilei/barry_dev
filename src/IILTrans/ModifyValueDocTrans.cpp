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
#include "IILTrans/ModifyValueDocTrans.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "DocUtil/DocUtil.h"
#include "IILTrans/IILTransInclude.h"
#include "Util/Buff.h"
#include "XmlUtil/XmlTag.h"


AosIILTransModifyValueDoc::AosIILTransModifyValueDoc(const bool flag)
:
AosIILTrans(AosTransType::eModifyValueDoc, flag AosMemoryCheckerArgs),
mDocid(0),
mOverride(false)
{
}


AosIILTransModifyValueDoc::AosIILTransModifyValueDoc(
		const u64 &iilid,
		const AosIILType &iiltype,
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
AosIILTrans(AosTransType::eModifyValueDoc, iilid,
	false, snap_id, need_save, need_resp AosMemoryCheckerArgs),
mOldValue(oldvalue),
mNewValue(newvalue),
mDocid(docid),
mOverride(override),
mIILType(iiltype)
{
	setValueUnique(valueUnique);
	setDocidUnique(docidUnique);
}


AosIILTransModifyValueDoc::AosIILTransModifyValueDoc(
		const OmnString &iilname,
		const AosIILType &iiltype,
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
AosIILTrans(AosTransType::eModifyValueDoc, iilname,
	false, snap_id, need_save, need_resp AosMemoryCheckerArgs),
mOldValue(oldvalue),
mNewValue(newvalue),
mDocid(docid),
mOverride(override),
mIILType(iiltype)
{
	setValueUnique(valueUnique);
	setDocidUnique(docidUnique);
}


AosTransPtr
AosIILTransModifyValueDoc::clone()
{
	return OmnNew AosIILTransModifyValueDoc(false);
}


bool
AosIILTransModifyValueDoc::serializeTo(const AosBuffPtr &buff)
{
	bool rslt = AosIILTrans::serializeTo(buff);
	aos_assert_r(rslt, false);

	buff->setOmnStr(mOldValue);
	buff->setOmnStr(mNewValue);
	buff->setU64(mDocid);
	buff->setU8(mOverride);
	buff->setU32((u32)mIILType);
	return true;
}


bool
AosIILTransModifyValueDoc::serializeFrom(const AosBuffPtr &buff)
{
	bool rslt = AosIILTrans::serializeFrom(buff);
	aos_assert_r(rslt, false);

	mOldValue = buff->getOmnStr("");
	mNewValue = buff->getOmnStr("");
	mDocid = buff->getU64(0);
	mOverride = buff->getU8(0);
	mIILType = (AosIILType)(buff->getU32(0));
	return true;
}


bool
AosIILTransModifyValueDoc::proc(
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
			rslt = iilobj->modifyDocSafe(mOldValue, mNewValue,mDocid, isValueUnique(), isDocidUnique(), rdata);
			break;
		case eAosIILType_BigU64:
			rslt = iilobj->modifyDocSafe(mOldValue.toU64(), mNewValue.toU64(),mDocid, isValueUnique(), isDocidUnique(), rdata);
			break;
		case eAosIILType_BigI64:
			rslt = iilobj->modifyDocSafe(mOldValue.toI64(), mNewValue.toI64(),mDocid, isValueUnique(), isDocidUnique(), rdata);
			break;
		case eAosIILType_BigD64:
			rslt = iilobj->modifyDocSafe(mOldValue.toD64(), mNewValue.toD64(),mDocid, isValueUnique(), isDocidUnique(), rdata);
			break;
		default:
			break;
	} 
	return rslt;

	
}


AosIILType 
AosIILTransModifyValueDoc::getIILType() const
{
	return mIILType;
}


int
AosIILTransModifyValueDoc::getSerializeSize() const
{
	return AosIILTrans::getSerializeSize()
		+ AosBuff::getStrSize(mIILName.length())
		+ AosBuff::getStrSize(mOldValue.length())
		+ AosBuff::getStrSize(mNewValue.length())
		+ AosBuff::getU64Size()
		+ AosBuff::getU32Size();
}

