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
// This is the implementations of <OmnString, buff> hashing. The 
// bucket format is:
// 		Bucket Length			(4 bytes, the length not include this field and
// 								 ContinueField and EndPoison)
// 		FrontPoison				(4 bytes)
// 		key 	(OmnString)	
// 		len		(short)
// 		buff 	(variable)
// 		key 	(OmnString)	
// 		len		(short)
// 		buff	(variable)
// 		...
// 		ContinueField			(u64)
// 		EndPoison				(4 bytes)
//
// Modification History:
// 01/01/2012	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "UtilHash/HashedObjBuff.h"

#include "Util/Buff.h"
#include "Util/File.h"


AosHashedObjBuff::AosHashedObjBuff()
{
}


AosHashedObjBuff::AosHashedObjBuff(
		const OmnString &key,
		const AosBuffPtr &buff)
:
AosHashedObj(key, eBuff),
mBuff(buff)
{
	// Note that 'buff' does not include the length field.
}


bool 
AosHashedObjBuff::isSame(const AosHashedObjPtr &rhs) const
{
	// This is a buffed obj. Two such objects are the same
	// if and only if their keys are the same and contents
	// are the same, too.
	aos_assert_r(rhs, false);
	if (rhs->getType() != eBuff) return false;
	AosHashedObjBuff *rr = (AosHashedObjBuff *)(rhs.getPtr());
	if (mKey != rr->getKeyStr()) return false;
	aos_assert_r(mBuff, false);
	aos_assert_r(rr->mBuff, false);
	return ((mBuff->dataLen() == rr->mBuff->dataLen()) &&
		   (memcmp(mBuff->data(), rr->mBuff->data(), mBuff->dataLen()) == 0));
}


int
AosHashedObjBuff::doesRecordMatch(
		AosBuff &buff,
		const OmnString &key_in_buff,
		const OmnString &expected_key,
		AosHashedObjPtr &obj)
{
	// Each record is in the following format:
	// 	key			(string)
	// 	len			(short)
	// 	contents	(variable)
	// 	key			(string)
	// 	len			(short)
	// 	contents	(variable)
	// 	...
	int idx = buff.getCrtIdx();
	aos_assert_r(idx >= 0, -1);
	int len = buff.getU16(0);
	aos_assert_r(len >= 0, -1);
	if (key_in_buff != expected_key)
	{
		// It does not match. Need to move the buff to the next
		// record.
		buff.setCrtIdx((u32)(idx + len));
		return len;
	}

	AosBuffPtr bf = buff.copyContents(idx, len);
	obj = OmnNew AosHashedObjBuff(expected_key, bf);
	return len;
}


int
AosHashedObjBuff::setContentsToBuff(AosBuff &buff)
{
	// It copies the contents to 'buff'.
	aos_assert_r(mBuff, -1);
	int idx = buff.getCrtIdx();
	int len = mBuff->dataLen();
	aos_assert_r(len >= 0, -1);
	buff.setU16(len);
	buff.appendBuff(mBuff);
	return buff.getCrtIdx() - idx;
}


bool 
AosHashedObjBuff::reset() 
{
	mKey = "";
	if (mBuff) mBuff->setDataLen(0);
	return true;
}

