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
// 03/23/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "IILTrans/U64IncrementDocidByIdTrans.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "Util/Buff.h"
#include "XmlUtil/XmlTag.h"


AosIILTransU64IncrementDocidById::AosIILTransU64IncrementDocidById(const bool flag)
:
AosIILTrans(AosTransType::eU64IncrementDocidById, flag AosMemoryCheckerArgs),
mKey(0),
mIncValue(0),
mInitValue(0),
mAddFlag(false)
{
}


AosIILTransU64IncrementDocidById::AosIILTransU64IncrementDocidById(
		const u64 iilid,
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
AosIILTrans(AosTransType::eU64IncrementDocidById, iilid, 
		isPersis, snap_id, need_save, need_resp AosMemoryCheckerArgs),
mKey(key),
mIncValue(incValue),
mInitValue(initValue),
mAddFlag(add_flag),
mDftValue(dftValue)
{
}


AosTransPtr 
AosIILTransU64IncrementDocidById::clone()
{
	return OmnNew AosIILTransU64IncrementDocidById(false);
}


bool
AosIILTransU64IncrementDocidById::serializeFrom(const AosBuffPtr &buff)
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
AosIILTransU64IncrementDocidById::serializeTo(const AosBuffPtr &buff)
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
AosIILTransU64IncrementDocidById::proc(
		const AosIILObjPtr &iilobj,
		AosBuffPtr &resp_buff,
		const AosRundataPtr &rdata)
{
	aos_assert_rr(iilobj, rdata, false);
	aos_assert_rr(iilobj->getIILType() == eAosIILType_U64, rdata, false);

	u64 value;
	bool rslt = iilobj->incrementDocidSafe(mKey, value, mIncValue, 
			mInitValue, mAddFlag, mDftValue, rdata);

	resp_buff = OmnNew AosBuff(10 AosMemoryCheckerArgs);
	resp_buff->setU8(rslt);
	resp_buff->setU64(value);
	return true;
}


int
AosIILTransU64IncrementDocidById::getSerializeSize() const 
{
	OmnNotImplementedYet;
	return 0;
}


AosIILType 
AosIILTransU64IncrementDocidById::getIILType() const
{
	return eAosIILType_U64;
}

