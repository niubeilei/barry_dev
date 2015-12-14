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
#include "IILTrans/U64GetDocidByIdTrans.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "Util/Buff.h"
#include "XmlUtil/XmlTag.h"


AosIILTransU64GetDocidById::AosIILTransU64GetDocidById(const bool flag)
:
AosIILTrans(AosTransType::eU64GetDocidById, flag AosMemoryCheckerArgs),
mKey(0)
{
}
	

AosIILTransU64GetDocidById::AosIILTransU64GetDocidById(
		const u64 iilid,
		const u64 key,
		const bool need_save,
		const bool need_resp,
		const u64 snap_id)
:
AosIILTrans(AosTransType::eU64GetDocidById, iilid,
		false, snap_id, need_save, need_resp AosMemoryCheckerArgs),
mKey(key)
{
}


AosTransPtr 
AosIILTransU64GetDocidById::clone()
{
	return OmnNew AosIILTransU64GetDocidById(false);
}


bool
AosIILTransU64GetDocidById::serializeFrom(const AosBuffPtr &buff)
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
AosIILTransU64GetDocidById::serializeTo(const AosBuffPtr &buff)
{
	bool rslt = AosIILTrans::serializeTo(buff);
	aos_assert_r(rslt, false);
	
	buff->setU64(mKey);
	return true; 
}


bool
AosIILTransU64GetDocidById::proc(
		const AosIILObjPtr &iilobj, 
		AosBuffPtr &resp_buff,
		const AosRundataPtr &rdata)
{
	aos_assert_rr(iilobj, rdata, false);
	aos_assert_rr(iilobj->getIILType() == eAosIILType_U64, rdata, false);

	u64 docid = 0;
	bool found = false;
	bool is_unique = false;
	int64_t idx = -10;
	int64_t iilidx = -10;
	u64 kkk = mKey;
	bool rslt = iilobj->nextDocidSafe(idx, iilidx, false, eAosOpr_eq, kkk, docid, is_unique, rdata);
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
	resp_buff->setU8(is_unique);
	resp_buff->setU8(found);
	resp_buff->setU64(docid);
	
	return true;
}



AosIILType 
AosIILTransU64GetDocidById::getIILType() const
{
	return eAosIILType_U64;
}
	

int
AosIILTransU64GetDocidById::getSerializeSize() const
{
	OmnNotImplementedYet;
	return 0;
}

