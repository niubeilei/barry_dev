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
// 08/07/2013 Created by Linda 
////////////////////////////////////////////////////////////////////////////
#include "DataScanner/DocScanner.h"

#include "API/AosApi.h"
#include "DataScanner/Ptrs.h"
#include "SEInterfaces/DocClientObj.h"
#include "TransBasic/Trans.h"
#include "XmlUtil/XmlDoc.h"

AosDocScanner::AosDocScanner(const bool flag)
:
AosDataScanner(AOSDATASCANNER_DOC, AosDataScannerType::eDoc, flag),
mLock(OmnNew OmnMutex()),
mCondVar(OmnNew OmnCondVar()),
mFieldsInfo(OmnNew AosBuff(AosMemoryCheckerArgsBegin)),
mTotalReceivedSize(0),
mCrtVidIdx(0)
{
	mSizeIdLen = OmnNew AosBuff(AosMemoryCheckerArgsBegin); 
}


AosDocScanner::AosDocScanner(const AosXmlTagPtr &conf, const AosRundataPtr &rdata)
:
AosDataScanner(AOSDATASCANNER_DOC, AosDataScannerType::eDoc, false),
mLock(OmnNew OmnMutex()),
mCondVar(OmnNew OmnCondVar()),
mFieldsInfo(OmnNew AosBuff(AosMemoryCheckerArgsBegin)),
mTotalReceivedSize(0),
mCrtVidIdx(0)
{
	mSizeIdLen = OmnNew AosBuff(AosMemoryCheckerArgsBegin); 
	bool rslt = config(conf, rdata);
	if (!rslt)
	{
		OmnThrowException(rdata->getErrmsg());
	}
}


AosDocScanner::~AosDocScanner()
{
}


bool
AosDocScanner::config(const AosXmlTagPtr &conf, const AosRundataPtr &rdata)
{
	return true;
}


bool
AosDocScanner::getNextBlock(AosBuffPtr &buff, const AosRundataPtr &rdata)
{
	AosBuffDataPtr info;
	bool rslt = getNextBlock(info, rdata);
	buff = info->getBuff();
	return rslt;
}


bool
AosDocScanner::getNextBlock(
		AosBuffDataPtr &info,
		const AosRundataPtr &rdata)
{
	info = OmnNew AosBuffData();
	AosBuffPtr buff = 0;

	mLock->lock();
	while(mQueue.size() == 0)
	{
		if (mDocids.size() == 0)
		{
			info->setBuff(buff);
			mLock->unlock();
			return true;
		}

		sanityCheck();
		bool timeout = false;
		mCondVar->timedWait(mLock, timeout, 1);
	}
	buff = mQueue.front();
	mQueue.pop();
	mTotalReceivedSize -= eBlockSize;	
	info->setBuff(buff);

	addRequestLocked(rdata);
	mLock->unlock();
	return true;
}


void
AosDocScanner::addRequestLocked(const AosRundataPtr &rdata)
{
	vector<u32> total_vids = AosGetTotalCubeIds();
	aos_assert(total_vids.size() > mCrtVidIdx);

	u32 init_vid_idx = mCrtVidIdx;
	while (1)
	{
		if (mTotalReceivedSize + eBlockSize > eMaxMemory) break;

		u32 vid = total_vids[mCrtVidIdx];
		if (mDocids.count(vid) != 0 && mDocids[vid].isLoading() == false)
		{
			mTotalReceivedSize += eBlockSize;
			readData(rdata, vid);	
		}

		mCrtVidIdx ++;
		if (mCrtVidIdx >= total_vids.size()) mCrtVidIdx = 0;
		if (mCrtVidIdx == init_vid_idx) break;
	}
}


bool                                  
AosDocScanner::initDocScanner(      
		const AosRundataPtr &rdata,
		const u64 *docids,
		const int64_t num_docids,
		const AosBuffPtr &field_info)
{
	mFieldsInfo = field_info;
	aos_assert_r(num_docids > 0, false);
	mLock->lock();
	map<u64, int> sizeid_len_map;
	for (u32 i = 0; i< num_docids; i++)
	{
		u64 docid = docids[i];
		shuffer(sizeid_len_map, docid, rdata);
	}
	addRequestLocked(rdata);
	mLock->unlock();
	return true;	
}

bool                                  
AosDocScanner::shuffer(      
		map<u64, int> &sizeidmap,        
		const u64 &docid,
		const AosRundataPtr &rdata)   
{
	int vid = AosGetCubeId(AosXmlDoc::getOwnDocid(docid));

	AosDocType::E type = AosDocType::getDocType(docid);
	aos_assert_r(AosDocType::isValid(type), false);
	if (type == AosDocType::eGroupedDoc)
	{
		u64 sizeid = AosGetSizeIdByDocid(docid);
		if (sizeidmap.count(sizeid) == 0)
		{
			int record_len = AosGetDataRecordLenByDocid(
					rdata->getSiteid(), docid, rdata);
			sizeidmap.insert(make_pair(sizeid, record_len));
			mSizeIdLen->setU64(sizeid);
			mSizeIdLen->setInt(record_len);
		}
	}

	if (mDocids.count(vid) == 0)
	{
		StDocids st;
		st.loading = false;
		st.set(docid);
		mDocids.insert(make_pair(vid, st));
	}
	else
	{
		mDocids[vid].set(docid);
	}
	return true;
}

bool                                  
AosDocScanner::initDocScanner(      
		const vector<u64> &docids,        
		const AosRundataPtr &rdata)   
{
	aos_assert_r(docids.size(), false);
	mLock->lock();
	map<u64, int> sizeid_len_map;
	for (u32 i = 0; i< docids.size(); i++)
	{
		u64 docid = docids[i];
		shuffer(sizeid_len_map, docid, rdata);
	}
	addRequestLocked(rdata);
	mLock->unlock();
	return true;	
}


bool
AosDocScanner::readData(
		const AosRundataPtr &rdata,
		const int &vid)
{
	u32 num_docs = eMaxNumDocsPerTrans;
	AosBuffPtr docids_buff = OmnNew AosBuff(AosMemoryCheckerArgsBegin); 
	mFieldsInfo->reset();
	u32 fields_num = mFieldsInfo->getU32(0);
	if(fields_num > 0)
	{
		docids_buff->setU32(1);
		docids_buff->setU64(mFieldsInfo->dataLen()-sizeof(u32));
		docids_buff->appendBuff(mFieldsInfo);
	}
	else
	{
		docids_buff->setU32(0);
	}

	aos_assert_r(mDocids.count(vid), false);
	mDocids[vid].serializeToBuff(docids_buff, num_docs);
	aos_assert_r(docids_buff->dataLen() > 0, false);

	AosAsyncRespCallerPtr thisPtr(this, false); 
	AosDocClientObj::getDocClient()->batchGetDocs(rdata, vid, 
			num_docs, eBlockSize, docids_buff, mSizeIdLen, thisPtr);
	return true;
}


void
AosDocScanner::callback(
		const AosTransPtr &trans,
		const AosBuffPtr &resp,
		const bool svr_death)
{
	if(svr_death)
	{
		OmnScreen << "send Trans failed. svr death."
			<< "; trans_id:" << trans->getTransId().toString()
			<< "; death_svr_id:" << trans->getToSvrId()
			<< endl;
		return;
	}
	
	aos_assert(resp);

	mLock->lock();
	bool rslt = resp->getU8(0); 
	aos_assert(rslt);

	u64 read_edocid = resp->getU64(0); 
	aos_assert(read_edocid);
	
	u32 doc_len = resp->getU32(0);
	aos_assert(doc_len);

	char *data = resp->data();
	int64_t offset = sizeof(u8) + sizeof(u64) + sizeof(u32); 
	memmove(data, &data[offset], doc_len);
	resp->setDataLen(doc_len);
	mQueue.push(resp);

	int vid = AosGetCubeId(AosXmlDoc::getOwnDocid(read_edocid));
	mDocids[vid].removeDocids(read_edocid);
	if (mDocids[vid].size() == 0) mDocids.erase(vid);
	mLock->unlock();
}


bool
AosDocScanner::signal(const int threadLogicId)
{
	return true;
}


bool
AosDocScanner::createDocScanner(
		vector<AosDataScannerObjPtr> &scanners,
		const AosRundataPtr &rdata)
{
	try
	{
		AosDocScanner* scanner = OmnNew AosDocScanner(false);
		scanners.push_back(scanner);
	}
	catch(...)
	{
		OmnAlarm << "Faild to create FileScanner" << enderr;
		return false;
	}
	return true;
}


bool
AosDocScanner::serializeTo(
		const AosBuffPtr &buff, 
		const AosRundataPtr &rdata)
{
	OmnNotImplementedYet;
	return false;
}


bool
AosDocScanner::serializeFrom(
		const AosBuffPtr &buff, 
		const AosRundataPtr &rdata)
{
	OmnNotImplementedYet;
	return false;
}


AosJimoPtr
AosDocScanner::cloneJimo()  const
{
	try
	{
		return OmnNew AosDocScanner(*this);
	}

	catch (...)
	{
		OmnAlarm << "Failed creating object" << enderr;
		return 0;
	}
}


AosDataScannerObjPtr 
AosDocScanner::clone(
		const AosXmlTagPtr &def,
		const AosRundataPtr &rdata)
{
	OmnNotImplementedYet;
	return 0;
}


int64_t 
AosDocScanner::getTotalSize() const
{
	return 100;
}


bool
AosDocScanner::sanityCheck()
{
	if (mDocids.size() == 0) return true;
	bool isloading = false;
	map<int, StDocids>::iterator itr = mDocids.begin();
	for (; itr != mDocids.end(); ++itr)
	{
		if (itr->second.isLoading()) 
		{
			isloading = true;
		}
	}
	aos_assert_r(isloading, false);
	return true;
}


int
AosDocScanner::getPhysicalId() const
{
	return -1;
}

