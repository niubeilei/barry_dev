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
#include "IILTrans/HitRemoveDocByNameTrans.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "Debug/Except.h"
#include "Util/Buff.h"
#include "XmlUtil/XmlTag.h"


AosIILTransHitRemoveDocByName::AosIILTransHitRemoveDocByName(const bool flag)
:
AosIILTrans(AosTransType::eHitRemoveDocByName, flag AosMemoryCheckerArgs),
mDocid(0)
{
}


AosIILTransHitRemoveDocByName::AosIILTransHitRemoveDocByName(
		const OmnString &iilname,
		const u64 &docid,
		const bool need_save,
		const bool need_resp,
		const u64 snap_id)
:
AosIILTrans(AosTransType::eHitRemoveDocByName, iilname,
	false, snap_id, need_save, need_resp AosMemoryCheckerArgs),
mDocid(docid)
{
}


AosTransPtr 
AosIILTransHitRemoveDocByName::clone()
{
	return OmnNew AosIILTransHitRemoveDocByName(false);
}


bool 
AosIILTransHitRemoveDocByName::serializeTo(const AosBuffPtr &buff)
{
	bool rslt = AosIILTrans::serializeTo(buff);
	aos_assert_r(rslt, false);

	buff->setU64(mDocid);	
	return true;
}


bool 
AosIILTransHitRemoveDocByName::serializeFrom(const AosBuffPtr &buff)
{
	bool rslt = AosIILTrans::serializeFrom(buff);
	aos_assert_r(rslt, false);

	mDocid = buff->getU64(0);
	return true;
}


bool
AosIILTransHitRemoveDocByName::proc(
		const AosIILObjPtr &iilobj,
		AosBuffPtr &resp_buff,
		const AosRundataPtr &rdata)
{
	aos_assert_rr(iilobj, rdata, false);

	aos_assert_rr(iilobj->getIILType() == eAosIILType_Hit, rdata, false);

	return iilobj->removeDocSafe(mDocid, rdata);
}


AosIILType 
AosIILTransHitRemoveDocByName::getIILType() const
{
	return eAosIILType_Hit;
}


int
AosIILTransHitRemoveDocByName::getSerializeSize() const
{
	return AosIILTrans::getSerializeSize()
		+ AosBuff::getStrSize(mIILName.length())
		+ AosBuff::getU64Size();
}

