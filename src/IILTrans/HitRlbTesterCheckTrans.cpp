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
// 2013/09/06	Created by Ketty 
////////////////////////////////////////////////////////////////////////////
#include "IILTrans/HitRlbTesterCheckTrans.h"
#if 0

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "Debug/Except.h"
#include "Util/Buff.h"
#include "XmlUtil/XmlTag.h"


AosIILTransHitRlbTesterCheck::AosIILTransHitRlbTesterCheck(const bool flag)
:
AosIILTrans(AosTransType::eHitRlbTesterCheck, flag AosMemoryCheckerArgs)
{
}


AosIILTransHitRlbTesterCheck::AosIILTransHitRlbTesterCheck(
		const OmnString &iilname,
		const u64 &docid,
		const u64 snap_id)
:
AosIILTrans(AosTransType::eHitRlbTesterCheck, iilname, false, snap_id, false, true AosMemoryCheckerArgs),
mDocid(docid)
{
}


AosTransPtr 
AosIILTransHitRlbTesterCheck::clone()
{
	return OmnNew AosIILTransHitRlbTesterCheck(false);
}


bool
AosIILTransHitRlbTesterCheck::serializeFrom(const AosBuffPtr &buff)
{
	bool rslt = AosIILTrans::serializeFrom(buff);
	aos_assert_r(rslt, false);

	mDocid = buff->getU64(0);
	return true;
}


bool
AosIILTransHitRlbTesterCheck::serializeTo(const AosBuffPtr &buff)
{
	bool rslt = AosIILTrans::serializeTo(buff);
	aos_assert_r(rslt, false);

	buff->setU64(mDocid);
	return true;
}


bool
AosIILTransHitRlbTesterCheck::proc(
		const AosIILObjPtr &iilobj,
		AosBuffPtr &resp_buff,
		const AosRundataPtr &rdata)
{
	aos_assert_r(iilobj && iilobj->getIILType() == eAosIILType_Hit,  false);

	AosIILHitPtr u64iil = (AosIILHit*)(iilobj.getPtr());

	bool exist = u64iil->docExistDirectSafe(mDocid, rdata);
	
	resp_buff = OmnNew AosBuff(10, 0 AosMemoryCheckerArgs);
	resp_buff->setU8(exist);
	return true;
}


AosIILType 
AosIILTransHitRlbTesterCheck::getIILType() const
{
	return eAosIILType_Hit;
}


int
AosIILTransHitRlbTesterCheck::getSerializeSize() const
{
	return AosIILTrans::getSerializeSize()
		+ AosBuff::getStrSize(mIILName.length())
		+ AosBuff::getU64Size();
}

#endif
