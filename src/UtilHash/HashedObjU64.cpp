////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// Description:
// This is the simple form of implementing <OmnString, u64> hashing. The 
// bucket format is:
// 		Bucket Length			(4 bytes, the length not include this field and
// 								 ContinueField and EndPoison)
// 		FrontPoison				(4 bytes)
// 		key (OmnString)	
// 		value (u64)
// 		key (OmnString)	
// 		value (u64)
// 		...
// 		ContinueField			(u64)
// 		EndPoison				(4 bytes)
//
// Modification History:
// 09/04/2011	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "UtilHash/HashedObjU64.h"

#include "Util/Buff.h"
#include "Util/File.h"


AosHashedObjU64::AosHashedObjU64()
{
}


AosHashedObjU64::AosHashedObjU64(
		const OmnString &key, 
		const u64 &value)
:
AosHashedObj(key, eU64),
mValue(value)
{
}


bool 
AosHashedObjU64::isSame(const AosHashedObjPtr &rhs) const
{
	aos_assert_r(rhs, false);
	if (rhs->getType() != eU64) return false;
	AosHashedObjU64 *rr = (AosHashedObjU64 *)(rhs.getPtr());
	OmnString rhsKey = rr->getKeyStr();
	u64 rhsValue = rr->getValue();
	return mKey == rhsKey && mValue == rhsValue;
}


int
AosHashedObjU64::doesRecordMatch(
		AosBuff &buff,
		const OmnString &key_in_buff,
		const OmnString &expected_key,
		AosHashedObjPtr &obj)
{
	u64 value =	buff.getU64(0);
	if (key_in_buff != expected_key) 
	{
		obj = 0;
		return 0;
	}
	obj = OmnNew AosHashedObjU64(expected_key, value);
	return sizeof(mValue);
}


int
AosHashedObjU64::setContentsToBuff(AosBuff &buff)
{
	buff.setU64(mValue);
	return sizeof(mValue);
}


AosHashedObjPtr 
AosHashedObjU64::clone()
{
	return OmnNew AosHashedObjU64();
}


bool 
AosHashedObjU64::reset() 
{
	mKey = "";
	mValue = 0;
	return true;
}

