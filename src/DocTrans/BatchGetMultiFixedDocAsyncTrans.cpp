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
// 10/20/2014	Created by Andy
////////////////////////////////////////////////////////////////////////////
#include "DocTrans/BatchGetMultiFixedDocAsyncTrans.h"

#include "API/AosApi.h"
#include "SEInterfaces/DocSvrObj.h"
#include "XmlUtil/XmlTag.h"


AosBatchGetMultiFixedDocAsyncTrans::AosBatchGetMultiFixedDocAsyncTrans(const bool regflag)
:
AosDocTrans(AosTransType::eBatchGetMultiFixedDocAsync, regflag)
{
}


AosBatchGetMultiFixedDocAsyncTrans::AosBatchGetMultiFixedDocAsyncTrans(
		const u64 data_scan_req_id,
		const u32 vid,
		const vector<u64> &docids,
		const vector<int> &record_lens,
		const u64 snap_id,
		const AosAsyncRespCallerPtr &resp_caller)
:
AosDocTrans(AosTransType::eBatchGetMultiFixedDocAsync, vid, true, false, true, snap_id),
mDataScanReqId(data_scan_req_id),
mRespCaller(resp_caller),
mDocids(docids),
mRecordLens(record_lens),
mVid(vid)
{
	aos_assert(docids.size() == record_lens.size());
}


AosBatchGetMultiFixedDocAsyncTrans::~AosBatchGetMultiFixedDocAsyncTrans()
{
}


bool
AosBatchGetMultiFixedDocAsyncTrans::serializeFrom(const AosBuffPtr &buff)
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

	int record_len = 0;
	for(u32 i=0; i<docid_num; i++)
	{
		record_len = buff->getInt(-1);
		aos_assert_r(record_len > -1, false);
		mRecordLens.push_back(record_len);
	}
	setDistId(mVid);
	return true;
}


bool
AosBatchGetMultiFixedDocAsyncTrans::serializeTo(const AosBuffPtr &buff)
{
	bool rslt = AosDocTrans::serializeTo(buff);
	aos_assert_r(rslt, false);
	aos_assert_r(mDocids.size() == mRecordLens.size(), false);

	buff->setU64(mDataScanReqId);
	buff->setU32(mDocids.size());
	for(u32 i=0; i<mDocids.size(); i++)
	{
		buff->setU64(mDocids[i]);
	}

	for(u32 i=0; i<mRecordLens.size(); i++)
	{
		buff->setInt(mRecordLens[i]);
	}
	return true;
}


AosTransPtr
AosBatchGetMultiFixedDocAsyncTrans::clone()
{
	return OmnNew AosBatchGetMultiFixedDocAsyncTrans(false);
}


bool
AosBatchGetMultiFixedDocAsyncTrans::proc()
{
	bool rslt = splitDocid();
	aos_assert_r(rslt, false);
	map<int, vector<u64> >::iterator itr;
	AosBuffPtr doc_buff;
	AosBuffPtr resp_buff = OmnNew AosBuff(10 AosMemoryCheckerArgs);
	
	resp_buff->setU64(mDataScanReqId);
	resp_buff->setU8(true);

	for (itr = mDocidGroup.begin(); itr != mDocidGroup.end(); itr++)
	{
		doc_buff = AosDocSvrObj::getDocSvr()->getCommonDocs(mRdata, itr->second, itr->first);	
		if (!doc_buff) return setErrResp();
		resp_buff->setBuff(doc_buff->data(), doc_buff->dataLen());
	}

	sendResp(resp_buff);
	return true;
}


bool
AosBatchGetMultiFixedDocAsyncTrans::setErrResp()
{
	AosBuffPtr resp_buff = OmnNew AosBuff(100 AosMemoryCheckerArgs);
	
	resp_buff->setU64(mDataScanReqId);
	resp_buff->setU8(false);	
	sendResp(resp_buff);
	return true;
}

bool
AosBatchGetMultiFixedDocAsyncTrans::respCallBack()
{
	AosBuffPtr resp = getResp();
	bool svr_death = isSvrDeath();

	AosTransPtr thisptr(this, false);
	mRespCaller->callback(thisptr, resp, svr_death);
	return true;
}


bool
AosBatchGetMultiFixedDocAsyncTrans::splitDocid()
{
	aos_assert_r(mRecordLens.size() == mDocids.size(), false);
	int record_len;
	for(u32 i=0; i<mRecordLens.size(); i++)
	{
		record_len = mRecordLens[i];
		if (mDocidGroup.find(record_len) != mDocidGroup.end())
		{
			mDocidGroup[record_len].push_back(mDocids[i]);
		}
		else
		{
			vector<u64> vec;
			vec.push_back(mDocids[i]);
			mDocidGroup[record_len] = vec;
		}
	}
	return true;
}
