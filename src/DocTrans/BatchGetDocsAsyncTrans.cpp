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
// 08/08/2013	Created by Linda  
////////////////////////////////////////////////////////////////////////////
#include "DocTrans/BatchGetDocsAsyncTrans.h"

#include "XmlUtil/XmlTag.h"
#include "SEInterfaces/DocSvrObj.h"
#include "DataScanner/BinaryDocScanner.h"

AosBatchGetDocsAsyncTrans::AosBatchGetDocsAsyncTrans(const bool regflag)
:
AosDocTrans(AosTransType::eBatchGetDocsAsync, regflag)
{
}


AosBatchGetDocsAsyncTrans::AosBatchGetDocsAsyncTrans(
		const AosAsyncRespCallerPtr &resp_caller,
		const int vid,
		const u32 num_docs,
		const u32 blocksize,
		const AosBuffPtr &docid_buff,
		const AosBuffPtr &sizeid_len,
		const u64 snap_id,
		const bool need_save,	
		const bool need_resp)
:
AosDocTrans(AosTransType::eBatchGetDocsAsync, 
		vid, true, need_save, need_resp, snap_id),
mNumDocs(num_docs),
mBlockSize(blocksize),
mDocids(docid_buff),
mSizeIdLen(sizeid_len),
mRespCaller(resp_caller),
mVid(vid)
{
}


AosBatchGetDocsAsyncTrans::~AosBatchGetDocsAsyncTrans()
{
}


bool
AosBatchGetDocsAsyncTrans::serializeFrom(const AosBuffPtr &buff)
{
	bool rslt = AosDocTrans::serializeFrom(buff);
	aos_assert_r(rslt, false);

	mVid = buff->getInt(0);

	mNumDocs = buff->getU32(0);
	aos_assert_r(mNumDocs != 0, false);

	mBlockSize = buff->getU32(0);
	aos_assert_r(mBlockSize != 0, false);

	u32 docid_len = buff->getU32(0);
	mDocids = buff->getBuff(docid_len, true AosMemoryCheckerArgs);

	u32 record_datalen = buff->getU32(0);
	if (record_datalen != 0)
	{
		mSizeIdLen = buff->getBuff(record_datalen, true AosMemoryCheckerArgs); 
		mSizeIdLen->reset();
	}
	setDistId(mVid);
	return true;
}


bool
AosBatchGetDocsAsyncTrans::serializeTo(const AosBuffPtr &buff)
{
	bool rslt = AosDocTrans::serializeTo(buff);
	aos_assert_r(rslt, false);

	buff->setInt(mVid);

	buff->setU32(mNumDocs);

	buff->setU32(mBlockSize);

	buff->setU32(mDocids->dataLen());
	buff->setBuff(mDocids);

	aos_assert_r(mSizeIdLen, false);
	buff->setU32(mSizeIdLen->dataLen());
	if (mSizeIdLen->dataLen() != 0)
	{
		buff->setBuff(mSizeIdLen);
	}
	return true;
}


AosTransPtr
AosBatchGetDocsAsyncTrans::clone()
{
	return OmnNew AosBatchGetDocsAsyncTrans(false);
}


bool
AosBatchGetDocsAsyncTrans::proc()
{
	map<u64, int> size_len_map;
	while(mSizeIdLen && mSizeIdLen->getCrtIdx() < mSizeIdLen->dataLen())
	{
		u64 sizeid = mSizeIdLen->getU64(0);
		int len = mSizeIdLen->getInt(0);
		size_len_map.insert(make_pair(sizeid, len)); 
	}

	u64 end_docid = 0;
	AosBuffPtr buff = OmnNew AosBuff(mBlockSize AosMemoryCheckerArgs);
	bool rslt = AosDocSvrObj::getDocSvr()->getDocs(mRdata, buff,
			mNumDocs, mDocids, size_len_map, end_docid, mSnapshotId);
	aos_assert_r(rslt, false);

	AosBuffPtr resp_buff;
	if(!rslt)
	{
		resp_buff = OmnNew AosBuff(10 AosMemoryCheckerArgs);
		resp_buff->setU8(false);
	}
	else
	{
		u32 doc_len = buff->dataLen();
		u32 size = doc_len + sizeof(u8) + sizeof (u64) + sizeof(u32);
		resp_buff = OmnNew AosBuff(doc_len + size AosMemoryCheckerArgs);
		resp_buff->setU8(true);
		resp_buff->setU64(end_docid);
		resp_buff->setU32(doc_len);
		resp_buff->setBuff((char *)buff->data(), doc_len);
	}
	sendResp(resp_buff);
	return true;
}


bool
AosBatchGetDocsAsyncTrans::respCallBack()
{
	AosBuffPtr resp = getResp();
	//bool svr_death = isSvrDeath();
	
	AosTransPtr thisptr(this, false);
//	mRespCaller->callback(thisptr, resp, svr_death);
	return true;
}


