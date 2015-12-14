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
// 05/07/2014	Created by Felicia
////////////////////////////////////////////////////////////////////////////
#include "DocTrans/BatchGetNormDocAsyncTrans.h"

#include "API/AosApi.h"
#include "SEInterfaces/DocSvrObj.h"
#include "XmlUtil/XmlTag.h"


AosBatchGetNormDocAsyncTrans::AosBatchGetNormDocAsyncTrans(const bool regflag)
:
AosDocTrans(AosTransType::eBatchGetNormDocAsync, regflag)
{
}


AosBatchGetNormDocAsyncTrans::AosBatchGetNormDocAsyncTrans(
		const u64 data_scan_req_id,
		const u32 vid,
		const vector<u64> &docids,
		const u64 snap_id,
		const AosAsyncRespCallerPtr &resp_caller)
:
AosDocTrans(AosTransType::eBatchGetNormDocAsync, vid, true, false, true, snap_id),
mDataScanReqId(data_scan_req_id),
mRespCaller(resp_caller),
mDocids(docids),
mVid(vid)
{
}


AosBatchGetNormDocAsyncTrans::~AosBatchGetNormDocAsyncTrans()
{
}


bool
AosBatchGetNormDocAsyncTrans::serializeFrom(const AosBuffPtr &buff)
{
	bool rslt = AosDocTrans::serializeFrom(buff);
	aos_assert_r(rslt, false);
	
	mDataScanReqId = buff->getU64(0); 
	
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
AosBatchGetNormDocAsyncTrans::serializeTo(const AosBuffPtr &buff)
{
	bool rslt = AosDocTrans::serializeTo(buff);
	aos_assert_r(rslt, false);

	buff->setU64(mDataScanReqId);
	buff->setU32(mDocids.size());
	for(u32 i=0; i<mDocids.size(); i++)
	{
		buff->setU64(mDocids[i]);
	}
	return true;
}


AosTransPtr
AosBatchGetNormDocAsyncTrans::clone()
{
	return OmnNew AosBatchGetNormDocAsyncTrans(false);
}


bool
AosBatchGetNormDocAsyncTrans::proc()
{
	AosBuffPtr resp_buff = OmnNew AosBuff(10 AosMemoryCheckerArgs);
	
	vector<AosXmlTagPtr> docs;
	bool rslt = getDocs(docs);

	if (!rslt) return setErrResp();
		
	resp_buff->setU64(mDataScanReqId);
	resp_buff->setU8(true);

	//resp_buff->setU32(docs.size());

	int doc_len;
	for(u32 i=0; i<docs.size(); i++)
	{
		OmnString data = docs[i]->toString();
		doc_len = data.length();

		u64 docid = docs[i]->getAttrU64(AOSTAG_DOCID, 0);
		aos_assert_r(docid > 0, false);

		//AosBuffPtr buff = OmnNew AosBuff(doc_len+sizeof(u32) AosMemoryCheckerArgs);
		//buff->setU32(doc_len);
		//buff->setBuff(data.data(), doc_len);

		//resp_buff->setU32(buff->dataLen());
		//resp_buff->setBuff(buff);
		resp_buff->setU64(docid);
		resp_buff->setU32(doc_len);
		resp_buff->setU64(0);
		resp_buff->setBuff(data.data(), doc_len);
	}
	
	sendResp(resp_buff);
	return true;
}


bool
AosBatchGetNormDocAsyncTrans::getDocs(vector<AosXmlTagPtr> &docs)
{
	docs.clear();
	AosXmlTagPtr doc;
	for(u32 i=0; i<mDocids.size(); i++)
	{
		doc = AosDocSvrObj::getDocSvr()->getDoc(mDocids[i], mSnapshotId, mRdata);	
		if(!doc)	return false;

		aos_assert_r(doc->getAttrU64(AOSTAG_DOCID, 0) == mDocids[i], false);
		docs.push_back(doc);
	}
	return true;
}

bool
AosBatchGetNormDocAsyncTrans::setErrResp()
{
	AosBuffPtr resp_buff = OmnNew AosBuff(100 AosMemoryCheckerArgs);
	
	resp_buff->setU64(mDataScanReqId);
	resp_buff->setU8(false);	
	sendResp(resp_buff);
	return true;
}

bool
AosBatchGetNormDocAsyncTrans::respCallBack()
{
	AosBuffPtr resp = getResp();
	bool svr_death = isSvrDeath();

	AosTransPtr thisptr(this, false);
	mRespCaller->callback(thisptr, resp, svr_death);
	return true;
}


