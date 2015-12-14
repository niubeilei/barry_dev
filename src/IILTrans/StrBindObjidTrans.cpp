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
// 10/05/2011	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "IILTrans/StrBindObjidTrans.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "DocUtil/DocUtil.h"
#include "Util/Buff.h"
#include "XmlUtil/XmlTag.h"


AosIILTransStrBindObjid::AosIILTransStrBindObjid(const bool flag)
:
AosIILTrans(AosTransType::eStrBindObjid, flag AosMemoryCheckerArgs),
mObjid(""),
mDocid(0)
{
}


AosIILTransStrBindObjid::AosIILTransStrBindObjid(
		const OmnString &iilname,
		const OmnString &objid,
		const u64 docid,
		const bool need_save,
		const bool need_resp,
		const u64 snap_id)
:
AosIILTrans(AosTransType::eStrBindObjid, iilname,
		false, snap_id, need_save, need_resp AosMemoryCheckerArgs),
mObjid(objid),
mDocid(docid)
{
}


AosTransPtr 
AosIILTransStrBindObjid::clone()
{
	return OmnNew AosIILTransStrBindObjid(false);
}


bool
AosIILTransStrBindObjid::serializeFrom(const AosBuffPtr &buff)
{
	bool rslt = AosIILTrans::serializeFrom(buff);
	aos_assert_r(rslt, false);

	mObjid = buff->getOmnStr("");
	mDocid = buff->getU64(0);
	aos_assert_r(mObjid != "" && mDocid, false);

	return true;
}


bool
AosIILTransStrBindObjid::serializeTo(const AosBuffPtr &buff)
{
	bool rslt = AosIILTrans::serializeTo(buff);
	aos_assert_r(rslt, false);

	buff->setOmnStr(mObjid);
	buff->setU64(mDocid);

	return true;
}


bool
AosIILTransStrBindObjid::proc(
		const AosIILObjPtr &iilobj,
		AosBuffPtr &resp_buff,
		const AosRundataPtr &rdata)
{
	aos_assert_r(iilobj, false);

	bool rslt = bindObjid(iilobj, mObjid, mDocid, rdata);

	resp_buff = OmnNew AosBuff(10 AosMemoryCheckerArgs);
	resp_buff->setU8(rslt);

	return true;
}


bool
AosIILTransStrBindObjid::bindObjid(
		const AosIILObjPtr &iilobj,
		const OmnString &objid, 
		const u64 &docid,
		const AosRundataPtr &rdata)
{
	aos_assert_r(iilobj, false);
	aos_assert_r(iilobj->getIILType() == eAosIILType_Str, false);

	rdata->setOverrideFlag(false);

	bool rslt = iilobj->addDocSafe(
		objid, docid, true, true, rdata);
	if (rslt)
	{
		// The objid is added to the iil. 
		rdata->setOk();
		return true;
	}

	OmnAlarm << rdata->getErrmsg() << enderr;
	return false;
}


AosIILType 
AosIILTransStrBindObjid::getIILType() const
{
	bool isNumAlpha = AosDocUtil::isNumAlpha(mIILName.data(), mIILName.length());
	if (isNumAlpha) return eAosIILType_NumAlpha;
	return eAosIILType_Str;
}


int
AosIILTransStrBindObjid::getSerializeSize() const 
{
	OmnNotImplementedYet;
	return 0;
}

