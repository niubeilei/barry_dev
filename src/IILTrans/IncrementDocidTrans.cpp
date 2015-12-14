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
#include "IILTrans/IncrementDocidTrans.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "DocUtil/DocUtil.h"
//#include "TransUtil/XmlTrans.h"
#include "Util/Buff.h"
#include "XmlUtil/XmlTag.h"


AosIILTransIncrementDocid::AosIILTransIncrementDocid(const bool flag)
:
AosIILTrans(AosTransType::eIncrementDocid, flag AosMemoryCheckerArgs),
//mIILName(""),
mKey(""),
mIncValue(0),
mInitValue(0)
{
}


AosIILTransIncrementDocid::AosIILTransIncrementDocid(
		const OmnString &iilname,
		const AosIILType &iiltype,
		const bool isPersis,
		const OmnString &key,
		const u64 inc_value,
		const u64 init_value,
		const bool add_flag,
		const bool need_save,
		const bool need_resp,
		const u64 snap_id)
:
AosIILTrans(AosTransType::eIncrementDocid, 
		iilname, isPersis, snap_id, need_save, need_resp AosMemoryCheckerArgs),
mKey(key),
mIncValue(inc_value),
mInitValue(init_value),
mAddFlag(add_flag),
mIILType(iiltype)
{
}

AosIILTransIncrementDocid::AosIILTransIncrementDocid(
		const u64 &iilid,
		const AosIILType &iiltype,
		const bool isPersis,
		const OmnString &key,
		const u64 inc_value,
		const u64 init_value,
		const bool add_flag,
		const bool need_save,
		const bool need_resp,
		const u64 snap_id)
:
AosIILTrans(AosTransType::eIncrementDocid, 
		iilid, isPersis, snap_id, need_save, need_resp AosMemoryCheckerArgs),
mKey(key),
mIncValue(inc_value),
mInitValue(init_value),
mAddFlag(add_flag),
mIILType(iiltype)
{
}

AosTransPtr 
AosIILTransIncrementDocid::clone()
{
	return OmnNew AosIILTransIncrementDocid(false);
}


bool
AosIILTransIncrementDocid::serializeFrom(const AosBuffPtr &buff)
{
	bool rslt = AosIILTrans::serializeFrom(buff);
	aos_assert_r(rslt, false);

	mKey = buff->getOmnStr("");
	mIncValue = buff->getU64(0);
	mInitValue = buff->getU64(0);
	mAddFlag = buff->getU8(0);
	mIILType = (AosIILType)buff->getU32(0);

	aos_assert_r(mKey != "", false);
	return true;
}


bool
AosIILTransIncrementDocid::serializeTo(const AosBuffPtr &buff)
{
	bool rslt = AosIILTrans::serializeTo(buff);
	aos_assert_r(rslt, false);
	
	buff->setOmnStr(mKey);
	buff->setU64(mIncValue);
	buff->setU64(mInitValue);
	buff->setU8(mAddFlag);
	buff->setU32((u32)mIILType);
	return true;
}


bool
AosIILTransIncrementDocid::proc(
		const AosIILObjPtr &iilobj,
		AosBuffPtr &resp_buff,
		const AosRundataPtr &rdata)
{
	aos_assert_r(iilobj, false);
	aos_assert_r(iilobj->getIILType() == eAosIILType_Str ||
		iilobj->getIILType() == eAosIILType_BigStr ||
		iilobj->getIILType() == eAosIILType_BigU64 || 
		iilobj->getIILType() == eAosIILType_BigI64 || 
		iilobj->getIILType() == eAosIILType_BigD64, false);
	
	u64 value;
	bool rslt = false;
	switch(mIILType)
	{
		case eAosIILType_Str:
		case eAosIILType_BigStr:
			rslt = iilobj->incrementDocidSafe(mKey, value, mIncValue, mInitValue, mAddFlag, rdata);
			break;
		case eAosIILType_U64:
		case eAosIILType_BigU64:
			rslt = iilobj->incrementDocidSafe(mKey.toU64(), value, mIncValue, mInitValue, mAddFlag, 0, rdata);
			break;
		case eAosIILType_BigI64:
//shawnxxx			rslt = iilobj->incrementDocidSafe(mKey.toI64(), value, mIncValue, mInitValue, mAddFlag, 0, rdata);
			break;
		case eAosIILType_BigD64:
//shawnxxx			rslt = iilobj->incrementDocidSafe(mKey.toD64(), value, mIncValue, mInitValue, mAddFlag, 0, rdata);
			break;
		default:
			break;
	} 

	resp_buff = OmnNew AosBuff(20 AosMemoryCheckerArgs);
	resp_buff->setU8(rslt);
	resp_buff->setU64(value);
	return rslt;
}

int
AosIILTransIncrementDocid::getSerializeSize() const 
{
	OmnNotImplementedYet;
	return 0;
}


AosIILType 
AosIILTransIncrementDocid::getIILType() const
{
	if(mIILType == eAosIILType_Str)
	{
		bool isNumAlpha = AosDocUtil::isNumAlpha(mIILName.data(), mIILName.length());
		if(isNumAlpha) return eAosIILType_NumAlpha;
	}
	return mIILType;
}

