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
#include "DocTrans/CheckDocLockTrans.h"

#include "API/AosApi.h"
#include "DocServer/DocSvr.h"
#include "XmlUtil/XmlTag.h"

AosCheckDocLockTrans::AosCheckDocLockTrans(const bool regflag)
:
AosDocTrans(AosTransType::eCheckDocLock, regflag)
{
}


AosCheckDocLockTrans::AosCheckDocLockTrans(
		const u64 docid,
		const OmnString &type,
		const bool need_save,
		const bool need_resp,
		const u64 snap_id)
:
AosDocTrans(AosTransType::eCheckDocLock, docid, need_save, need_resp, snap_id),
mDocid(docid),
mDType(type)
{
}


AosCheckDocLockTrans::~AosCheckDocLockTrans()
{
}


bool
AosCheckDocLockTrans::serializeFrom(const AosBuffPtr &buff)
{
	bool rslt = AosDocTrans::serializeFrom(buff);
	aos_assert_r(rslt, false);
	
	mDocid = buff->getU64(0);
	mDType = buff->getOmnStr("");
	aos_assert_r(mDocid && mDType != "", false);
	setDistId(mDocid);
	return true;
}


bool
AosCheckDocLockTrans::serializeTo(const AosBuffPtr &buff)
{
	bool rslt = AosDocTrans::serializeTo(buff);
	aos_assert_r(rslt, false);
	
	buff->setU64(mDocid);
	buff->setOmnStr(mDType);
	return true;
}


AosTransPtr
AosCheckDocLockTrans::clone()
{
	return OmnNew AosCheckDocLockTrans(false);
}


bool
AosCheckDocLockTrans::proc()
{
	// This function is the server side implementations of AosCheckDocLock::check(...)

	bool result = false; //AosDocSvr::getSelf()->procCheckLock(mRdata, mDocid, mDType);

	//OmnString contents = "<Contents><record ";
	//contents << "result" << "=\"" << (result?"true":"false")<< "\" /></Contents>";
	//rdata->setContents(contents);
	//rdata->setOk();

	AosBuffPtr resp_buff = OmnNew AosBuff(10 AosMemoryCheckerArgs);
	resp_buff->setU8(result);

	// Ketty 2013/07/23
	sendResp(resp_buff);
	//AosTransPtr thisptr(this, false);
	//AosSendResp(thisptr, resp_buff);
	return true;
}


