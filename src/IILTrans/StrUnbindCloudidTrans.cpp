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
#include "IILTrans/StrUnbindCloudidTrans.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "DocUtil/DocUtil.h"
#include "Util/Buff.h"
#include "XmlUtil/XmlTag.h"


AosIILTransStrUnbindCloudid::AosIILTransStrUnbindCloudid(const bool flag)
:
AosIILTrans(AosTransType::eStrUnbindCloudid, flag AosMemoryCheckerArgs),
mCloudid(""),
mDocid(0)
{
}


AosIILTransStrUnbindCloudid::AosIILTransStrUnbindCloudid(
		const OmnString &iilname,
		const OmnString cloudid,
		const u64 docid,
		const bool need_save,
		const bool need_resp,
		const u64 snap_id)
:
AosIILTrans(AosTransType::eStrUnbindCloudid, iilname,
	false, snap_id, need_save, need_resp AosMemoryCheckerArgs),
mCloudid(cloudid),
mDocid(docid)
{
}


AosTransPtr 
AosIILTransStrUnbindCloudid::clone()
{
	return OmnNew AosIILTransStrUnbindCloudid(false);
}


bool
AosIILTransStrUnbindCloudid::serializeFrom(const AosBuffPtr &buff)
{
	bool rslt = AosIILTrans::serializeFrom(buff);
	aos_assert_r(rslt, false);

	mCloudid = buff->getOmnStr("");
	mDocid = buff->getU64(0);
	aos_assert_r(mCloudid != "" && mDocid, false);

	return true;
}


bool
AosIILTransStrUnbindCloudid::serializeTo(const AosBuffPtr &buff)
{
	bool rslt = AosIILTrans::serializeTo(buff);
	aos_assert_r(rslt, false);

	buff->setOmnStr(mCloudid);
	buff->setU64(mDocid);

	return true;
}


bool
AosIILTransStrUnbindCloudid::proc(
		const AosIILObjPtr &iilobj,
		AosBuffPtr &resp_buff,
		const AosRundataPtr &rdata)
{
	aos_assert_r(iilobj, false);

	bool rslt = unbindCloudid(iilobj, mCloudid, mDocid, rdata);

	resp_buff = OmnNew AosBuff(10 AosMemoryCheckerArgs);
	resp_buff->setU8(rslt);

	return true;
}


bool
AosIILTransStrUnbindCloudid::unbindCloudid(
		const AosIILObjPtr &iilobj,
		const OmnString &cloudid, 
		const u64 &docid,
		const AosRundataPtr &rdata)
{
	aos_assert_r(iilobj, false);
	aos_assert_r(iilobj->getIILType() == eAosIILType_Str, false);

	rdata->setOverrideFlag(false);
	bool rslt = iilobj->removeDocSafe(cloudid, docid, rdata);
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
AosIILTransStrUnbindCloudid::getIILType() const
{
	bool isNumAlpha = AosDocUtil::isNumAlpha(mIILName.data(), mIILName.length());
	if (isNumAlpha) return eAosIILType_NumAlpha;
	return eAosIILType_Str;
}


int
AosIILTransStrUnbindCloudid::getSerializeSize() const 
{
	OmnNotImplementedYet;
	return 0;
}

