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
#include "IILTrans/U64IncrementDocidByNameTrans.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "Util/Buff.h"
#include "XmlUtil/XmlTag.h"


AosIILTransU64IncrementDocidByName::AosIILTransU64IncrementDocidByName(const bool flag)
:
AosIILTrans(AosTransType::eU64IncrementDocidByName, flag AosMemoryCheckerArgs),
mKey(0),
mIncValue(0),
mInitValue(0)
{
}


AosIILTransU64IncrementDocidByName::AosIILTransU64IncrementDocidByName(
		const OmnString &iilname,
		const bool isPersis,
		const u64 key,
		const u64 incValue,
		const u64 initValue,
		const bool add_flag,
		const u64 dftValue,
		const bool need_save,
		const bool need_resp,
		const u64 snap_id)
:
AosIILTrans(AosTransType::eU64IncrementDocidByName, iilname, 
		isPersis, snap_id, need_save, need_resp AosMemoryCheckerArgs),
mKey(key),
mIncValue(incValue),
mInitValue(initValue),
mAddFlag(add_flag),
mDftValue(dftValue)
{
}


AosTransPtr 
AosIILTransU64IncrementDocidByName::clone()
{
	return OmnNew AosIILTransU64IncrementDocidByName(false);
}


bool
AosIILTransU64IncrementDocidByName::serializeFrom(const AosBuffPtr &buff)
{
	bool rslt = AosIILTrans::serializeFrom(buff);
	aos_assert_r(rslt, false);
	
	mKey = buff->getU64(0);
	mIncValue = buff->getU64(0);
	mInitValue = buff->getU64(0);
	mDftValue = buff->getU64(0);
	mAddFlag = buff->getU8(0);
	return true;
}


bool
AosIILTransU64IncrementDocidByName::serializeTo(const AosBuffPtr &buff)
{
	bool rslt = AosIILTrans::serializeTo(buff);
	aos_assert_r(rslt, false);
	
	buff->setU64(mKey);
	buff->setU64(mIncValue);
	buff->setU64(mInitValue);
	buff->setU64(mDftValue);
	buff->setU8(mAddFlag);
	return true;
}


bool
AosIILTransU64IncrementDocidByName::proc(
		const AosIILObjPtr &iilobj,
		AosBuffPtr &resp_buff,
		const AosRundataPtr &rdata)
{
	aos_assert_rr(iilobj, rdata, false);
	aos_assert_rr(iilobj->getIILType() == eAosIILType_U64, rdata, false);

	u64 value;
	bool rslt = iilobj->incrementDocidSafe(mKey, value, mIncValue, mInitValue, mAddFlag, mDftValue, rdata);

	resp_buff = OmnNew AosBuff(10 AosMemoryCheckerArgs);
	resp_buff->setU8(rslt);
	resp_buff->setU64(value);
	return true;
}


int
AosIILTransU64IncrementDocidByName::getSerializeSize() const 
{
	OmnNotImplementedYet;
	return 0;
}


AosIILType 
AosIILTransU64IncrementDocidByName::getIILType() const
{
	return eAosIILType_U64;
}

