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
#if 0
#include "DocTrans/BatchSendStatusTrans.h"

#include "DocServer/DocSvr.h"
#include "XmlUtil/XmlTag.h"
#include "API/AosApi.h"

AosBatchSendStatusTrans::AosBatchSendStatusTrans(const bool regflag)
:
AosTaskTrans(AosTransType::eBatchSendStatus, regflag)
{
}


AosBatchSendStatusTrans::AosBatchSendStatusTrans(
		const OmnString &scanner_id,
		const AosDocBatchReaderReq::E type,
		const AosBuffPtr &cont,
		const int svr_id,
		const bool need_save,
		const bool need_resp)
:
AosTaskTrans(AosTransType::eBatchSendStatus, svr_id, need_save, need_resp),
mScannerId(scanner_id),
mReqType(type),
mCont(cont)
{
}


AosBatchSendStatusTrans::~AosBatchSendStatusTrans()
{
}


bool
AosBatchSendStatusTrans::serializeFrom(const AosBuffPtr &buff)
{
	bool rslt = AosTaskTrans::serializeFrom(buff);
	aos_assert_r(rslt, false);
	
	mScannerId = buff->getOmnStr("");
	mReqType = (AosDocBatchReaderReq::E)buff->getU32(0);
	u32 data_len = buff->getU32(0);
	mCont = buff->getBuff(data_len, true AosMemoryCheckerArgs);
	
	aos_assert_r(mScannerId != "" && mCont, false);
	return true;
}


bool
AosBatchSendStatusTrans::serializeTo(const AosBuffPtr &buff)
{
	bool rslt = AosTaskTrans::serializeTo(buff);
	aos_assert_r(rslt, false);
	
	buff->setOmnStr(mScannerId);
	buff->setU32(mReqType);
	buff->setU32(mCont->dataLen());
	buff->setBuff(mCont->data(), mCont->dataLen());
	return true;
}


AosTransPtr
AosBatchSendStatusTrans::clone()
{
	return OmnNew AosBatchSendStatusTrans(false);
}


bool
AosBatchSendStatusTrans::proc()
{
	// This function is the server side implementations of AosDocMgr::getDoc(...)

	// Ketty 2013/07/18
	//u32 client_id = getClientId();
	u32 client_id = getFromSvrId();

	bool rslt = AosDocSvr::getSelf()->batchReader(
			mScannerId, mReqType, mCont, client_id, mRdata);
	aos_assert_r(rslt, false);

	AosBuffPtr resp_buff = OmnNew AosBuff(10 AosMemoryCheckerArgs);
	resp_buff->setU8(rslt);

	// Ketty 2013/07/23
	sendResp(resp_buff);
	//AosTransPtr thisptr(this, false);
	//AosSendResp(thisptr, resp_buff);
	return true;
}
#endif
