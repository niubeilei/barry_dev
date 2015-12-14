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
#include "IILTrans/RemoveValueDocTrans.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "Debug/Except.h"
#include "DocUtil/DocUtil.h"
#include "Util/Buff.h"
#include "XmlUtil/XmlTag.h"


AosIILTransRemoveValueDoc::AosIILTransRemoveValueDoc(const bool flag)
:
AosIILTrans(AosTransType::eRemoveValueDoc, flag AosMemoryCheckerArgs),
mDocid(0)
{
}


AosIILTransRemoveValueDoc::AosIILTransRemoveValueDoc(
		const OmnString &iilname,
		const AosIILType &iiltype,
		const OmnString &value,
		const u64 &docid,
		const bool need_save,
		const bool need_resp,
		const u64 snap_id)
:
AosIILTrans(AosTransType::eRemoveValueDoc, iilname,
	false, snap_id, need_save, need_resp AosMemoryCheckerArgs),
mValue(value),
mDocid(docid),
mIILType(iiltype)
{
}

AosIILTransRemoveValueDoc::AosIILTransRemoveValueDoc(
		const u64 &iilid,
		const AosIILType &iiltype,
		const OmnString &value,
		const u64 &docid,
		const bool need_save,
		const bool need_resp,
		const u64 snap_id)
:
AosIILTrans(AosTransType::eRemoveValueDoc, iilid,
	false, snap_id, need_save, need_resp AosMemoryCheckerArgs),
mValue(value),
mDocid(docid),
mIILType(iiltype)
{
}


AosTransPtr 
AosIILTransRemoveValueDoc::clone()
{
	return OmnNew AosIILTransRemoveValueDoc(false);
}


bool
AosIILTransRemoveValueDoc::serializeFrom(const AosBuffPtr &buff)
{
	bool rslt = AosIILTrans::serializeFrom(buff);
	aos_assert_r(rslt, false);

	mValue = buff->getOmnStr("");
	mDocid = buff->getU64(0);
	mIILType = (AosIILType)buff->getU32(0);
	return true;
}


bool
AosIILTransRemoveValueDoc::serializeTo(const AosBuffPtr &buff)
{
	bool rslt = AosIILTrans::serializeTo(buff);
	aos_assert_r(rslt, false);

	buff->setOmnStr(mValue);
	buff->setU64(mDocid);
	buff->setU32((u32)mIILType);
	return true;
}


bool
AosIILTransRemoveValueDoc::proc(
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
			rslt = iilobj->removeDocSafe(mValue, mDocid, rdata);
			break;
		case eAosIILType_BigU64:
			rslt = iilobj->removeDocSafe(mValue.toU64(), mDocid, rdata);
			break;
		case eAosIILType_BigI64:
			rslt = iilobj->removeDocSafe(mValue.toI64(), mDocid, rdata);
			break;
		case eAosIILType_BigD64:
			rslt = iilobj->removeDocSafe(mValue.toD64(), mDocid, rdata);
			break;
		default:
			break;
	} 
	return rslt;
}


AosIILType 
AosIILTransRemoveValueDoc::getIILType() const
{
	return mIILType;
}


int
AosIILTransRemoveValueDoc::getSerializeSize() const
{
	return AosIILTrans::getSerializeSize()
		+ AosBuff::getStrSize(mIILName.length())
		+ AosBuff::getStrSize(mValue.length())
		+ AosBuff::getU64Size()
		+ AosBuff::getU32Size();
}

