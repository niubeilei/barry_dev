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
#include "DocTrans/ReadUnLockDocTrans.h"

#include "API/AosApi.h"
#include "SEInterfaces/DocSvrObj.h"
#include "XmlUtil/XmlTag.h"

AosReadUnLockTrans::AosReadUnLockTrans(const bool regflag)
:
AosDocTrans(AosTransType::eReadUnLockDoc, regflag)
{
}


AosReadUnLockTrans::AosReadUnLockTrans(
		const u64 docid,
		const u64 userid,
		const bool need_save,
		const bool need_resp,
		const u64 snap_id)
:
//AosDocTrans(AosTransType::eGetDoc, AosXmlDoc::getOwnDocid(docid), need_save, need_resp),
AosDocTrans(AosTransType::eReadUnLockDoc, docid, false, need_resp, snap_id),
mDocid(docid),
mUserid(userid)
{
}


AosReadUnLockTrans::~AosReadUnLockTrans()
{
}


bool
AosReadUnLockTrans::serializeFrom(const AosBuffPtr &buff)
{
	bool rslt = AosDocTrans::serializeFrom(buff);
	aos_assert_r(rslt, false);
	
	mDocid = buff->getU64(0);
	mUserid = buff->getU64(0);
	aos_assert_r(mDocid != 0, false);
	setDistId(mDocid);
	return true;
}


bool
AosReadUnLockTrans::serializeTo(const AosBuffPtr &buff)
{
	bool rslt = AosDocTrans::serializeTo(buff);
	aos_assert_r(rslt, false);
	
	buff->setU64(mDocid);
	buff->setU64(mUserid);
	return true;
}


AosTransPtr
AosReadUnLockTrans::clone()
{
	return OmnNew AosReadUnLockTrans(false);
}


bool
AosReadUnLockTrans::proc()
{
	bool rslt = AosDocSvrObj::getDocSvr()->readUnLockDoc(mDocid, mUserid, mRdata.getPtr());

	AosBuffPtr resp_buff = OmnNew AosBuff(100 AosMemoryCheckerArgs);
	resp_buff->setU8(rslt);
	resp_buff->setOmnStr(mRdata->getErrmsg());
	
	sendResp(resp_buff);
	return true;
}

bool
AosReadUnLockTrans::procGetResp()
{
	// Ketty 2013/09/16
	return proc();
}

