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
#include "UtilHash/HashedObjStr.h"

#include "Util/Buff.h"
#include "Util/File.h"


AosHashedObjStr::AosHashedObjStr()

{

}


AosHashedObjStr::AosHashedObjStr(
		const OmnString &key, 
		const OmnString &value)
:
AosHashedObj(key, eU64),
mValue(value)
{
}


bool 
AosHashedObjStr::isSame(const AosHashedObjPtr &rhs) const
{
	aos_assert_r(rhs, false);
	if (rhs->getType() != eU64) return false;
	AosHashedObjStr *rr = (AosHashedObjStr *)(rhs.getPtr());
	OmnString rhsKey = rr->getKeyStr();
	OmnString rhsValue = rr->getValue();
	return mKey == rhsKey && mValue == rhsValue;
}


int
AosHashedObjStr::doesRecordMatch(
		AosBuff &buff,
		const OmnString &key_in_buff,
		const OmnString &expected_key,
		AosHashedObjPtr &obj)
{
	OmnString value = buff.getOmnStr("");
	if (key_in_buff != expected_key) 
	{
		obj = 0;
		return 0;
	}
	obj = OmnNew AosHashedObjStr(expected_key, value);
	return value.length();
}


int
AosHashedObjStr::setContentsToBuff(AosBuff &buff)
{
	buff.setOmnStr(mValue);
	return mValue.length();
}


AosHashedObjPtr 
AosHashedObjStr::clone()
{
	return OmnNew AosHashedObjStr();
}


bool 
AosHashedObjStr::reset() 
{
	mKey = "";
	mValue = "";
	return true;
}
