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
#include "DocTrans/BatchGetFixedDocAsyncTrans.h"

#include "API/AosApi.h"
#include "SEInterfaces/DocSvrObj.h"
#include "XmlUtil/XmlTag.h"


AosBatchGetFixedDocAsyncTrans::AosBatchGetFixedDocAsyncTrans(const bool regflag)
:
AosDocTrans(AosTransType::eBatchGetFixedDocAsync, regflag)
{
}


AosBatchGetFixedDocAsyncTrans::AosBatchGetFixedDocAsyncTrans(
		const u64 data_scan_req_id,
		const u32 vid,
		const vector<u64> &docids,
		const int record_len,
		const u64 snap_id,
		const AosAsyncRespCallerPtr &resp_caller)
:
AosDocTrans(AosTransType::eBatchGetFixedDocAsync, vid, true, false, true, snap_id),
mDataScanReqId(data_scan_req_id),
mRespCaller(resp_caller),
mDocids(docids),
mRecordLen(record_len),
mVid(vid)
{
}


AosBatchGetFixedDocAsyncTrans::~AosBatchGetFixedDocAsyncTrans()
{
}


bool
AosBatchGetFixedDocAsyncTrans::serializeFrom(const AosBuffPtr &buff)
{
	bool rslt = AosDocTrans::serializeFrom(buff);
	aos_assert_r(rslt, false);
	
	mDataScanReqId = buff->getU64(0); 
	
	mRecordLen = buff->getInt(-1);
	aos_assert_r(mRecordLen > 0, false);
	
	u64 docid_num = buff->getU32(0);
	u64 docid;
	for(u32 i=0; i<docid_num; i++)
	{
		docid = buff->getU64(0);
		aos_assert_r(docid, false);
		mDocids.push_back(docid);
	}
	setDistId(mVid);
	return true;
}


bool
AosBatchGetFixedDocAsyncTrans::serializeTo(const AosBuffPtr &buff)
{
	bool rslt = AosDocTrans::serializeTo(buff);
	aos_assert_r(rslt, false);

	buff->setU64(mDataScanReqId);
	buff->setInt(mRecordLen);
	buff->setU32(mDocids.size());
	for(u32 i=0; i<mDocids.size(); i++)
	{
		buff->setU64(mDocids[i]);
	}
	return true;
}


AosTransPtr
AosBatchGetFixedDocAsyncTrans::clone()
{
	return OmnNew AosBatchGetFixedDocAsyncTrans(false);
}


bool
AosBatchGetFixedDocAsyncTrans::proc()
{
	AosBuffPtr resp_buff = OmnNew AosBuff(10 AosMemoryCheckerArgs);
	
	AosBuffPtr doc_buff  = AosDocSvrObj::getDocSvr()->getFixedDocs(mRdata, mDocids, mRecordLen);	
	if (!doc_buff) return setErrResp();
		
	resp_buff->setU64(mDataScanReqId);
	resp_buff->setU8(true);
	resp_buff->setBuff(doc_buff->data(), doc_buff->dataLen());

	sendResp(resp_buff);
	return true;
}


bool
AosBatchGetFixedDocAsyncTrans::setErrResp()
{
	AosBuffPtr resp_buff = OmnNew AosBuff(100 AosMemoryCheckerArgs);
	
	resp_buff->setU64(mDataScanReqId);
	resp_buff->setU8(false);	
	sendResp(resp_buff);
	return true;
}

bool
AosBatchGetFixedDocAsyncTrans::respCallBack()
{
	AosBuffPtr resp = getResp();
	bool svr_death = isSvrDeath();

	AosTransPtr thisptr(this, false);
	mRespCaller->callback(thisptr, resp, svr_death);
	return true;
}


