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
//   
//
// Modification History:
// 07/28/2011	Created by Linda 
////////////////////////////////////////////////////////////////////////////
#include "DocTrans/DocLockTrans.h"

#include "DocServer/DocSvr.h"
#include "XmlUtil/XmlTag.h"

AosDocLockTrans::AosDocLockTrans(const bool regflag)
:
AosDocTrans(AosTransType::eDocLock, regflag)
{
}


AosDocLockTrans::AosDocLockTrans(
		const u64 docid,
		const OmnString &lock_type,
		const u64 lock_timer,
		const u64 lockid,
		const bool need_save,
		const bool need_resp,
		const u64 snap_id)
:
AosDocTrans(AosTransType::eDocLock, docid, need_save, need_resp, snap_id),
mDocid(docid),
mLockType(lock_type),
mLockTimer(lock_timer),
mLockid(lockid)
{
}


AosDocLockTrans::~AosDocLockTrans()
{
}


bool
AosDocLockTrans::serializeFrom(const AosBuffPtr &buff)
{
	bool rslt = AosDocTrans::serializeFrom(buff);
	aos_assert_r(rslt, false);
	
	mDocid = buff->getU64(0);
	mLockType = buff->getOmnStr("");
	mLockTimer = buff->getU64(0);
	mLockid = buff->getU64(0);
	aos_assert_r(mDocid && mLockType != "", false);
	setDistId(mDocid);
	return true;
}


bool
AosDocLockTrans::serializeTo(const AosBuffPtr &buff)
{
	bool rslt = AosDocTrans::serializeTo(buff);
	aos_assert_r(rslt, false);
	
	buff->setU64(mDocid);
	buff->setOmnStr(mLockType);
	buff->setU64(mLockTimer);
	buff->setU64(mLockid);
	return true;
}


AosTransPtr
AosDocLockTrans::clone()
{
	return OmnNew AosDocLockTrans(false);
}


bool
AosDocLockTrans::proc()
{
	// This function is the server side implementations of AosDocLock::check(...)
	AosTransPtr thisptr(this, false);
	bool rslt = false; //AosDocSvr::getSelf()->procDocLock(mRdata, thisptr, mDocid, mLockType, mLockTimer, mLockid);
	aos_assert_r(rslt, false);
	return true;
}
