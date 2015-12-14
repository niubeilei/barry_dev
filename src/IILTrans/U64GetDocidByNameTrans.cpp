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
#include "IILTrans/U64GetDocidByNameTrans.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "Util/Buff.h"
#include "XmlUtil/XmlTag.h"


AosIILTransU64GetDocidByName::AosIILTransU64GetDocidByName(const bool flag)
:
AosIILTrans(AosTransType::eU64GetDocidByName, flag AosMemoryCheckerArgs),
mKey(0)
{
}


AosIILTransU64GetDocidByName::AosIILTransU64GetDocidByName(
		const OmnString &iilname,
		const u64 &key,
		const bool need_save, 
		const bool need_resp,
		const u64 snap_id)
:
AosIILTrans(AosTransType::eU64GetDocidByName, iilname,
	false, snap_id, need_save, need_resp AosMemoryCheckerArgs),
mKey(0)
{
}


AosTransPtr 
AosIILTransU64GetDocidByName::clone()
{
	return OmnNew AosIILTransU64GetDocidByName(false);
}


bool
AosIILTransU64GetDocidByName::serializeFrom(const AosBuffPtr &buff)
{
	bool rslt = AosIILTrans::serializeFrom(buff);
	aos_assert_r(rslt, false);

	mKey = buff->getU64(0);

	if (mKey == 0)
	{
		OmnAlarm << "error!" << enderr;
		return false;
	}

	return true;
}


bool
AosIILTransU64GetDocidByName::serializeTo(const AosBuffPtr &buff)
{
	bool rslt = AosIILTrans::serializeTo(buff);
	aos_assert_r(rslt, false);

	buff->setU64(mKey);
	return true;
}


bool
AosIILTransU64GetDocidByName::proc(
		const AosIILObjPtr &iilobj,
		AosBuffPtr &resp_buff,
		const AosRundataPtr &rdata)
{
	aos_assert_r(iilobj, false);
	aos_assert_r(iilobj->getIILType() == eAosIILType_U64 ||
		iilobj->getIILType() == eAosIILType_BigU64, false);

	u64 docid = 0;
	bool isunique = false;
	bool found = false;
	int64_t idx = -10;
	int64_t iilidx = -10;
	AosIILIdx the_idx;
	u64 kkk = mKey;
	bool rslt = true;

	rslt = iilobj->nextDocidSafe(the_idx, idx, iilidx,
		false, eAosOpr_eq, kkk, docid, isunique, rdata);

	if (idx < 0)
	{
	    found = false;
	    docid = 0;
	}
	else
	{
	    found = true;
	}

	resp_buff = OmnNew AosBuff(50 AosMemoryCheckerArgs);
	resp_buff->setU8(rslt);
	resp_buff->setU8(isunique);
	resp_buff->setU8(found);
	resp_buff->setU64(docid);

	return true;
}


AosIILType 
AosIILTransU64GetDocidByName::getIILType() const
{
	if (AosIsSuperIIL(mIILName))
	{
		return eAosIILType_BigU64;
	}
	return eAosIILType_U64;
}


int
AosIILTransU64GetDocidByName::getSerializeSize() const
{
	OmnNotImplementedYet;
	return 0;
}

