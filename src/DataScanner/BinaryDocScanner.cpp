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
// 07/09/2012 Created by Linda 
////////////////////////////////////////////////////////////////////////////
#if 0
#include "DataScanner/BinaryDocScanner.h"

#include "API/AosApi.h"
#include "DataScanner/Ptrs.h"
#include "DocClient/DocClient.h"
#include "DbQuery/Query.h"
#include "SEUtil/IILName.h"
#include "Util/ValueRslt.h"
#include "ValueSel/ValueSel.h"
#include "TransBasic/Trans.h"
#include "SEInterfaces/DataRecordObj.h"

#include "CompressUtil/Compress.h"

AosBinaryDocScanner::AosBinaryDocScanner(const bool flag)
:
AosDataScanner(AOSDATASCANNER_BINARYDOC, AosDataScannerType::eBinaryDoc, flag),
mLock(OmnNew OmnMutex()),
mCondVar(OmnNew OmnCondVar()),
mReadingNum(0),
mLimitMaxNum(10),
mTotalSize(0),
mTotalNum(0)
{
}


AosBinaryDocScanner::AosBinaryDocScanner(const AosXmlTagPtr &conf, const AosRundataPtr &rdata)
:
AosDataScanner(AOSDATASCANNER_BINARYDOC, AosDataScannerType::eBinaryDoc, false),
mLock(OmnNew OmnMutex()),
mCondVar(OmnNew OmnCondVar()),
mReadingNum(0),
mLimitMaxNum(10),
mTotalSize(0),
mTotalNum(0)
{
	bool rslt = config(conf, rdata);
	if (!rslt)
	{
		OmnThrowException(rdata->getErrmsg());
	}
}


AosBinaryDocScanner::~AosBinaryDocScanner()
{
}


bool
AosBinaryDocScanner::config(const AosXmlTagPtr &conf, const AosRundataPtr &rdata)
{
	return true;
}


bool
AosBinaryDocScanner::getNextBlock(AosBuffPtr &buff, const AosRundataPtr &rdata)
{
	AosBuffDataPtr info;
	bool rslt = getNextBlock(info, rdata);
	buff = info->getBuff();
	mLock->lock();
	if (buff)
	{
		mTotalSize += buff->dataLen();
		mTotalNum ++;
OmnScreen << "=========================current read all block size: " << mTotalSize << ", Total num:" << mTotalNum << endl;
	}
	mLock->unlock();
	return rslt;
}


bool
AosBinaryDocScanner::getNextBlock(
		AosBuffDataPtr &info,
		const AosRundataPtr &rdata)
{
	// This function is called when someone wants to read some more 
	// docs from this class. It sends a request to the physical server
	// identified by 'mServerId'. If there are no data, the function
	// will wait until some data are available. If there are no more
	// data, it should return immediately and 'buff' should contain
	// no data. 
	
	info = OmnNew AosBuffData();
	AosBuffPtr buff = 0;
	mLock->lock();
	while( mQueue.size() == 0)
	{
		if (mReadingNum == 0 && mDocids.empty())
		{
			info->setBuff(buff);
			mLock->unlock();
			return true;
		}

		bool timeout = false;
		mCondVar->timedWait(mLock, timeout, 1);
	}

	AosBuffPtr xml_buff = mQueue.front();
	aos_assert_r(xml_buff, false);
	mQueue.pop();
	mReadingNum--;
	int readnum = mLimitMaxNum-mReadingNum;
	mLock->unlock();

	if (readnum>0)
	{
		readData(readnum, rdata);
	}

	xml_buff->reset();
	bool rslt = xml_buff->getU8(0);
	if(!rslt)	return false;
	
	u32 doc_len = xml_buff->getU32(0);
	aos_assert_r(doc_len, false);
	AosBuffPtr doc_buff = xml_buff->getBuff(doc_len, false AosMemoryCheckerArgs);

	AosXmlParser parser;
	AosXmlTagPtr b_doc = parser.parse(doc_buff->data(), doc_buff->dataLen(), "" AosMemoryCheckerArgs);
	aos_assert_r(b_doc, false);

	OmnString nodename = b_doc->getAttrStr(AOSTAG_BINARY_NODENAME, "");
	AosBuffPtr compressbuff = b_doc->getNodeTextBinaryCopy(nodename AosMemoryCheckerArgs);
	aos_assert_r(compressbuff && compressbuff->dataLen() > 0, false);

	buff = compressbuff;

	//OmnString ctnr_objid = b_doc->getAttrStr(AOSTAG_PARENTC, "");
	//if (ctnr_objid != "")
	//{
	//	AosXmlTagPtr dd = AosDocClient::getSelf()->getDocByObjid(ctnr_objid, rdata);
	//	OmnString compress_alg = dd->getAttrStr(AOSTAG_COMPRESS_ALG, "");
	//	if (compress_alg != "")
	//	{
	//		u64 destLen = b_doc->getAttrU64(AOSTAG_BINARY_SOURCELEN, 0);
	//		aos_assert_r(destLen, 0);
	//		rslt = AosCompress::uncompressStatic(
	//			compress_alg, buff, destLen, compressbuff,
	//			compressbuff->dataLen(), rdata AosMemoryCheckerArgs);
	//		if (!rslt)
	//		{
	//			OmnAlarm << rdata->getErrmsg() << enderr;
	//			dd = 0;
	//			b_doc = 0;
	//			return false;
	//		}
	//		aos_assert_r(buff->dataLen() > 0 && (u32)buff->dataLen() == destLen, 0);
	//	}
	//	dd = 0;
	//}

	if (nodename == "")
	{
		b_doc->removeNodeTexts();
	}
	else
	{
		b_doc->removeNode(nodename, false, false);
	}

	info->setBuff(buff);
	info->addMetadata(b_doc);
	return true;
}


bool                                  
AosBinaryDocScanner::initBinaryDocScanner(      
		const vector<u64> &docids,        
		const int physical_id,         
		const AosRundataPtr &rdata)   
{
	aos_assert_r(docids.size(), false);
	aos_assert_r(physical_id != -1, false);
OmnScreen << "===========================initBinaryDocScanner docid size: " << docids.size() << endl;
	mDocids.resize(docids.size());
	copy(docids.begin(), docids.end(), mDocids.begin());
	mServerId = physical_id;
	readData(mLimitMaxNum, rdata);
	return true;	
}


bool
AosBinaryDocScanner::readData(
		const int readnum,
		const AosRundataPtr &rdata)
{
	//send remote_binary_trans
	for (int i=0; i<readnum; i++)
	{
		mLock->lock();
		if (mDocids.empty())
		{
			mLock->unlock();
			return true;
		}
		u64 docid = mDocids.front();
		mDocids.pop_front();
		mReadingNum++;
		mLock->unlock();

OmnScreen << "==============================readData docid: " << docid << endl;
	
		// Ketty 2013/07/20
		//AosAsyncReqCallerPtr thisPtr(this, false);
		//AosDocClientObj::getDocClient()->
		//	retrieveBinaryDoc(docid, mServerId, thisPtr, rdata);

		AosAsyncRespCallerPtr thisPtr(this, false); 
		//AosDocClientObj::getDocClient()->retrieveBinaryDoc(thisPtr, docid, rdata);
		AosDocClientObj::getDocClient()->retrieveBinaryDoc(thisPtr, docid, rdata);
	}

	return true;
}


void
AosBinaryDocScanner::callback(
		const AosTransPtr &trans,
		const AosBuffPtr &resp,
		const bool svr_death)
{
	// Ketty 2013/07/20
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
	mQueue.push(resp);
	mCondVar->signal();
	mLock->unlock();
}


bool
AosBinaryDocScanner::signal(const int threadLogicId)
{
	return true;
}


bool
AosBinaryDocScanner::createBinaryDocScanner(
		vector<AosDataScannerObjPtr> &scanners,
		const AosRundataPtr &rdata)
{
	try
	{
		AosBinaryDocScanner* scanner = OmnNew AosBinaryDocScanner(false);
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
AosBinaryDocScanner::serializeTo(
		const AosBuffPtr &buff, 
		const AosRundataPtr &rdata)
{
	OmnNotImplementedYet;
	return false;
}


bool
AosBinaryDocScanner::serializeFrom(
		const AosBuffPtr &buff, 
		const AosRundataPtr &rdata)
{
	OmnNotImplementedYet;
	return false;
}


AosJimoPtr
AosBinaryDocScanner::cloneJimo()  const
{
	return OmnNew AosBinaryDocScanner(*this);
}


AosDataScannerObjPtr 
AosBinaryDocScanner::clone(
		const AosXmlTagPtr &def,
		const AosRundataPtr &rdata)
{
	OmnNotImplementedYet;
	return 0;
}


int64_t 
AosBinaryDocScanner::getTotalSize() const
{
	return 100;
}


int
AosBinaryDocScanner::getPhysicalId() const
{
	return -1;
}
#endif
