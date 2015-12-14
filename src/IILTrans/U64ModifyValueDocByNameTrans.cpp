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
#include "IILTrans/U64ModifyValueDocByNameTrans.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "Debug/Except.h"
#include "Util/Buff.h"
#include "XmlUtil/XmlTag.h"


AosIILTransU64ModifyValueDocByName::AosIILTransU64ModifyValueDocByName(const bool flag)
:
AosIILTrans(AosTransType::eU64ModifyValueDocByName, flag AosMemoryCheckerArgs)
{
}


AosIILTransU64ModifyValueDocByName::AosIILTransU64ModifyValueDocByName(
		const OmnString &iilname,
		const u64 oldvalue,
		const u64 newvalue,
		const bool value_unique,
		const bool docid_unique,
		const u64 &docid,
		const bool need_save,
		const bool need_resp,
		const u64 snap_id)
:
AosIILTrans(AosTransType::eU64ModifyValueDocByName, iilname,
		false, snap_id, need_save, need_resp AosMemoryCheckerArgs),
mDocid(docid),
mOldValue(oldvalue),
mNewValue(newvalue)
{
	setValueUnique(value_unique);
	setDocidUnique(docid_unique);
}


AosTransPtr 
AosIILTransU64ModifyValueDocByName::clone()
{
	return OmnNew AosIILTransU64ModifyValueDocByName(false);
}


bool
AosIILTransU64ModifyValueDocByName::serializeFrom(const AosBuffPtr &buff)
{
	bool rslt = AosIILTrans::serializeFrom(buff);
	aos_assert_r(rslt, false);

	mDocid = buff->getU64(0);
	mOldValue = buff->getU64(0);
	mNewValue = buff->getU64(0);
	return true;
}


bool
AosIILTransU64ModifyValueDocByName::serializeTo(const AosBuffPtr &buff)
{
	bool rslt = AosIILTrans::serializeTo(buff);
	aos_assert_r(rslt, false);

	buff->setU64(mDocid);
	buff->setU64(mOldValue);
	buff->setU64(mNewValue);
	return true;
}


bool
AosIILTransU64ModifyValueDocByName::proc(
		const AosIILObjPtr &iilobj,
		AosBuffPtr &resp_buff,
		const AosRundataPtr &rdata)
{
	aos_assert_rr(iilobj, rdata, false);
	aos_assert_rr(iilobj->getIILType() == eAosIILType_U64, rdata, false);

	return iilobj->modifyDocSafe(mOldValue, mNewValue, mDocid, true, true, rdata);
}


AosIILType 
AosIILTransU64ModifyValueDocByName::getIILType() const
{
	return eAosIILType_U64;
}

	
int
AosIILTransU64ModifyValueDocByName::getSerializeSize() const
{
	return AosIILTrans::getSerializeSize() + 
		AosBuff::getStrSize(mIILName.length()) +
		AosBuff::getU64Size() + 
		AosBuff::getU64Size() + 
		AosBuff::getU64Size();
}

