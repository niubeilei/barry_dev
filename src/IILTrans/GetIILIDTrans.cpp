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
#include "IILTrans/GetIILIDTrans.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "DocUtil/DocUtil.h"
#include "Util/Buff.h"
#include "XmlUtil/XmlTag.h"


AosIILTransGetIILID::AosIILTransGetIILID(const bool flag)
:
AosIILTrans(AosTransType::eGetIILID, flag AosMemoryCheckerArgs)
{
}


AosIILTransGetIILID::AosIILTransGetIILID(
		const OmnString &iilname,
		const bool need_save,
		const bool need_resp,
		const u64 snap_id)
:
AosIILTrans(AosTransType::eGetIILID, iilname, false,
		snap_id, need_save, need_resp AosMemoryCheckerArgs)
{
}


bool
AosIILTransGetIILID::serializeFrom(const AosBuffPtr &buff)
{
	bool rslt = AosIILTrans::serializeFrom(buff);
	aos_assert_r(rslt, false);

	return true;
}


bool
AosIILTransGetIILID::serializeTo(const AosBuffPtr &buff)
{
	bool rslt = AosIILTrans::serializeTo(buff);
	aos_assert_r(rslt, false);
	
	return true;
}


AosTransPtr 
AosIILTransGetIILID::clone()
{
	return OmnNew AosIILTransGetIILID(false);
}


bool
AosIILTransGetIILID::proc(
		const AosIILObjPtr &iilobj,
		AosBuffPtr &resp_buff,
		const AosRundataPtr &rdata)
{
	u64 iilid = getIILID(rdata);

	resp_buff = OmnNew AosBuff(10 AosMemoryCheckerArgs);
	resp_buff->setU8(true);
	resp_buff->setU64(iilid);

	return true;
}


int
AosIILTransGetIILID::getSerializeSize() const 
{
	OmnNotImplementedYet;
	return 0;
}


AosIILType 
AosIILTransGetIILID::getIILType() const
{
	return eAosIILType_Invalid;
}

