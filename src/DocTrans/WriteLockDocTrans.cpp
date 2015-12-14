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
#include "DocTrans/WriteLockDocTrans.h"

#include "API/AosApi.h"
#include "SEInterfaces/DocSvrObj.h"
#include "XmlUtil/XmlTag.h"

AosWriteLockTrans::AosWriteLockTrans(const bool regflag)
:
AosDocTrans(AosTransType::eWriteLockDoc, regflag)
{
}


AosWriteLockTrans::AosWriteLockTrans(
		const u64 docid,
		const u64 userid,
		const int waittimer,
		const int holdtimer,
		const bool need_save,
		const bool need_resp,
		const u64 snap_id)
:
//AosDocTrans(AosTransType::eGetDoc, AosXmlDoc::getOwnDocid(docid), need_save, need_resp),
AosDocTrans(AosTransType::eWriteLockDoc, docid, false, need_resp, snap_id),
mDocid(docid),
mUserid(userid),
mWaitTimer(waittimer),
mHoldTimer(holdtimer)
{
}


AosWriteLockTrans::~AosWriteLockTrans()
{
}


bool
AosWriteLockTrans::serializeFrom(const AosBuffPtr &buff)
{
	bool rslt = AosDocTrans::serializeFrom(buff);
	aos_assert_r(rslt, false);
	
	mDocid = buff->getU64(0);
	mUserid = buff->getU64(0);
	mWaitTimer = buff->getInt(0);
	mHoldTimer = buff->getInt(0);
	mStatus = buff->getInt(-1);
	aos_assert_r(mDocid != 0, false);
	setDistId(mDocid);
	return true;
}


bool
AosWriteLockTrans::serializeTo(const AosBuffPtr &buff)
{
	bool rslt = AosDocTrans::serializeTo(buff);
	aos_assert_r(rslt, false);
	
	buff->setU64(mDocid);
	buff->setU64(mUserid);
	buff->setInt(mWaitTimer);
	buff->setInt(mHoldTimer);
	buff->setInt(mStatus);
	return true;
}


AosTransPtr
AosWriteLockTrans::clone()
{
	return OmnNew AosWriteLockTrans(false);
}


bool
AosWriteLockTrans::waitLockExpired(
		const OmnString &user_id,
		const OmnString &start_time,
		const OmnString &wait_time,
		const OmnString &lock_type)
{
	// Tried to get the lock but failed. It waited but
	// the timer expired, which means that the lock is
	// still not available (normally, this indicates 
	// a serious problem).
	AosBuffPtr resp_buff = OmnNew AosBuff(10 AosMemoryCheckerArgs);
	resp_buff->setU8(false);
	OmnString errmsg = "Failed locking the doc ";
	errmsg << "userid : " << user_id << ", "
		   << "start_time : " << start_time << ", "
		   << "wait_time : " << wait_time << ", "
		   << "lock_type : " << lock_type;
	resp_buff->setOmnStr(errmsg);
	sendResp(resp_buff);
	OmnScreen << "@@@@@@ WriteLockDocTrans expired : " << mUserid << " errmsg : " << errmsg << endl;
	return true;
}


bool
AosWriteLockTrans::writeDocObtained(const AosRundataPtr &rdata)
{
	AosBuffPtr resp_buff = OmnNew AosBuff(10 AosMemoryCheckerArgs);
	resp_buff->setU8(true);
	sendResp(resp_buff);
	OmnScreen << "@@@@@@ WriteLockDocTrans success2 : " << mUserid << endl;
	return true;
}


bool
AosWriteLockTrans::proc()
{
	AosTransPtr thisptr(this, false);
	bool rslt = AosDocSvrObj::getDocSvr()->writeLockDoc(mDocid, mUserid, mWaitTimer, mHoldTimer, thisptr, mStatus, mRdata.getPtr());	

	AosBuffPtr resp_buff;
	if(!rslt)
	{
		// 1. There are errors in reading the doc. 
		// 2. It failed locking the doc.
		switch (mStatus)
		{
		case eError:
			 // Need to return an error;
			 resp_buff = OmnNew AosBuff(10 AosMemoryCheckerArgs);
			 resp_buff->setU8(false);
			 resp_buff->setOmnStr(mRdata->getErrmsg());
		OmnScreen << "@@@@@@ WriteLockDocTrans error : " << mUserid << " errmsg : " << mRdata->getErrmsg() << endl;
			 break;

		case eWaitingForLock:
			 // It means it failed obtaining the lock. It needs
			 // to wait until the lock becomes available.
			 // When the lock becomes available, it will call:
		OmnScreen << "@@@@@@ WriteLockDocTrans wait : " << mUserid << endl;
			 return true;

		default:
			 resp_buff = OmnNew AosBuff(10 AosMemoryCheckerArgs);
			 resp_buff->setU8(false);
			 OmnString errmsg = __FILE__;
			 errmsg << ":" << __LINE__ 
				 << " Internal Error: " << mStatus;
			 resp_buff->setOmnStr(errmsg);
		OmnScreen << "@@@@@@ WriteLockDocTrans invalid : " << mUserid << " errmsg : " << errmsg << endl;
			 break;
		}
	}
	else
	{
		resp_buff = OmnNew AosBuff(10 AosMemoryCheckerArgs);
		resp_buff->setU8(true);
		OmnScreen << "@@@@@@ WriteLockDocTrans success : " << mUserid << endl;
	}
	
	sendResp(resp_buff);
	return true;
}


bool
AosWriteLockTrans::procGetResp()
{
	// Ketty 2013/09/16
	return proc();
}

