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
#include "IILTrans/GetTotalNumDocsByNameTrans.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "Util/Buff.h"
#include "XmlUtil/XmlTag.h"


AosIILTransGetTotalNumDocsByName::AosIILTransGetTotalNumDocsByName(const bool flag)
:
AosIILTrans(AosTransType::eGetTotalNumDocsByName, flag AosMemoryCheckerArgs)
{
}


AosIILTransGetTotalNumDocsByName::AosIILTransGetTotalNumDocsByName(
		const OmnString &iilname,
		const u64 &snap_id,
		const bool need_save,
		const bool need_resp)
:
AosIILTrans(AosTransType::eGetTotalNumDocsByName, iilname,
	false, snap_id, need_save, need_resp AosMemoryCheckerArgs)
{
}


bool
AosIILTransGetTotalNumDocsByName::serializeFrom(const AosBuffPtr &buff)
{
	bool rslt = AosIILTrans::serializeFrom(buff);
	aos_assert_r(rslt, false);

	return true;
}


bool
AosIILTransGetTotalNumDocsByName::serializeTo(const AosBuffPtr &buff)
{
	bool rslt = AosIILTrans::serializeTo(buff);
	aos_assert_r(rslt, false);
	
	return true;
}


AosTransPtr 
AosIILTransGetTotalNumDocsByName::clone()
{
	return OmnNew AosIILTransGetTotalNumDocsByName(false);
}


bool
AosIILTransGetTotalNumDocsByName::proc(
		const AosIILObjPtr &iilobj,
		AosBuffPtr &resp_buff,
		const AosRundataPtr &rdata)
{
	aos_assert_r(iilobj, false);

	resp_buff = OmnNew AosBuff(10 AosMemoryCheckerArgs);
	resp_buff->setU8(true);
	resp_buff->setI64(iilobj->getNumDocs());

	return true;
}


int
AosIILTransGetTotalNumDocsByName::getSerializeSize() const 
{
	OmnNotImplementedYet;
	return 0;
}


AosIILType 
AosIILTransGetTotalNumDocsByName::getIILType() const
{
	return eAosIILType_Invalid;
}

