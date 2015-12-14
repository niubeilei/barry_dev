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
// This is the server side module that is responsible for reading docs. 
// Doc reading is organized by Scanner IDs. Different doc reading will
// assign a different scanner ID. A doc reading starts when this class
// receives the first request for the scanner. There will be multiple
// clients. When a client finishes the doc reading, it will send a finish
// message to this class. A doc reading finishes if and only if all 
// its clients sent the finished messages.
//
// Client:
// Before a client sends any data, it should send a Start message to all
// the servers. After that, it can send data. 
// When finishing, a client should send a Finish message to all
// the servers. 
//
// Server:
// For each scanner_id, when it starts (that is, a start message is 
// received for a non-existing scanner), the server should send a 
// Scanner Start message to its job. When a scanner finishes
// (that is, all clients finished and no more data for the scanner), 
// the server should send a message to its job.
//
// Description:
// 09/24/2012 Created by Linda 
////////////////////////////////////////////////////////////////////////////
#if 0
#include "DocServer/DocBatchReaderMgr.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "API/AosApi.h"
//#include "DocTrans/BatchGetDocsResp.h"
#include "DocServer/DocSvr.h"
#include "DocServer/DocBatchReader.h"
#include "DocServer/Ptrs.h"
#include "DocServer/DocBatchReaderReq.h"
#include "SingletonClass/SingletonImpl.cpp"
#include "Util/BuffArray.h"
#include "Thread/Ptrs.h"
#include "XmlUtil/XmlDoc.h"

OmnSingletonImpl(AosDocBatchReaderMgrSingleton,
                 AosDocBatchReaderMgr,
                 AosDocBatchReaderMgrSelf,
                "AosDocBatchReaderMgr");

AosDocBatchReaderMgr::AosDocBatchReaderMgr()
:
mLock(OmnNew OmnMutex()),
mCondVar(OmnNew OmnCondVar()),
mQueueFullCondVar(OmnNew OmnCondVar()),
mAllReaderQueueSize(0),
mTotalBitmapSize(0)
{
	//mNumVirtuals = AosGetNumCubes(); 
	//aos_assert(mNumVirtuals);
	OmnThreadedObjPtr thisptr(this, false);
	mThread = OmnNew OmnThread(thisptr, "docreader", 0, false, true, __FILE__, __LINE__);
	mThread->start();
}

AosDocBatchReaderMgr::~AosDocBatchReaderMgr()
{
}


bool
AosDocBatchReaderMgr::start()
{
	return true;
}


bool
AosDocBatchReaderMgr::stop()
{
	return true;
}


bool
AosDocBatchReaderMgr::config(const AosXmlTagPtr &def)
{
	return true;
}


bool
AosDocBatchReaderMgr::threadFunc(OmnThrdStatus::E &state, const OmnThreadPtr &thread)
{
	while(state == OmnThrdStatus::eActive)
	{
		mLock->lock();
		if (mQueue.empty())
		{
			mCondVar->wait(mLock);
			mLock->unlock();
			continue;
		}
		AosDocBatchReaderReqPtr request = mQueue.front();
		mQueue.pop();
		aos_assert_rl(request, mLock, false);

		bool rslt = false;
		AosDocBatchReaderPtr doc_reader;
		AosRundataPtr rdata = request->rdata;
		OmnString scanner_id = request->scannerid;
		aos_assert_rl(scanner_id != "", mLock, false);
		u32 client_id = request->client_id;
		switch (request->type)
		{
		case AosDocBatchReaderReq::eStart:
			 doc_reader = getDocReaderLocked(request, rdata);
			 if (!doc_reader)
			 {
			     OmnAlarm << "Failed retrieving the doc reader: " << scanner_id << enderr;
			 }
			 else
			 {
			 	//procReaderStart(request, doc_reader, rdata);
			 	rslt = procStartReqLocked(request, doc_reader, rdata);
			 }
			 mLock->unlock();
			 continue;

		case AosDocBatchReaderReq::eFinished:
			 doc_reader = getDocReaderLocked(scanner_id, rdata);
			 if (!doc_reader)
			 {
				 OmnAlarm << "Failed retrieving the doc reader: " << scanner_id << enderr;
			 }
			 else
			 {
			 	rslt = procFinishedReqLocked(request, doc_reader, rdata);
				aos_assert_rl(rslt, mLock, false);
			 }
			 mLock->unlock();
			 continue;

		case AosDocBatchReaderReq::eData:
			 aos_assert_rl(!(request->bitmap)->isEmpty(), mLock, false);

			 doc_reader = getDocReaderLocked(scanner_id, rdata);
			 if (!doc_reader)
			 {
			     OmnAlarm << "Failed retrieving the doc reader: " << scanner_id << enderr;
			 	 mLock->unlock();
			 }
			 else
			 {
			 	mLock->unlock();
			 	rslt = procDataLocked(request->bitmap, client_id, request, doc_reader, rdata);
			 }
			 continue;

		case AosDocBatchReaderReq::eStartRead:
			 doc_reader = getDocReaderLocked(scanner_id, rdata);
			 if (!doc_reader)
			 {
			    OmnAlarm << "Failed retrieving the doc reader: " << scanner_id << enderr;
			 }
			 else
			 {
				rslt = procStartRead(request->client_id, doc_reader, rdata);
			 }
			 mLock->unlock();
			 continue;

		default:
			 break;
		}

		OmnAlarm << "Unrecognized request type: " << request->type << enderr;
		mLock->unlock();
	}
	return true;
}


AosDocBatchReaderPtr
AosDocBatchReaderMgr::getDocReaderLocked(
		const AosDocBatchReaderReqPtr &request, 
		const AosRundataPtr &rdata)
{
	// This function retrieves the doc reader by the scanner id from 'request'. 
	// If the doc reader does not exist, and if 'create_flag' is set, it 
	// will create the reader.
	OmnString scanner_id = request->scannerid;
	map<OmnString, AosDocBatchReaderPtr>::iterator itr 
		= mScanners.find(scanner_id);
	if (itr == mScanners.end())
	{
		aos_assert_r(request->block_size > 0 && request->block_size < eMaxBlockSize, 0);
		AosDocBatchReaderPtr doc_reader = OmnNew AosDocBatchReader(scanner_id, request, rdata);
		mScanners.insert(make_pair(scanner_id, doc_reader));

		mAllReaderQueueSize += (doc_reader->getBlockSize() * doc_reader->getQueueSize());
		return doc_reader;
	}
	return itr->second;
}



AosDocBatchReaderPtr
AosDocBatchReaderMgr::getDocReaderLocked(
		const OmnString &scanner_id, 
		const AosRundataPtr &rdata)
{
	// This function retrieves the doc reader by the scanner id from 'request'. 
	// If the doc reader does not exist, and if 'create_flag' is set, it 
	// will create the reader.
	map<OmnString, AosDocBatchReaderPtr>::iterator itr 
		= mScanners.find(scanner_id);
	if (itr == mScanners.end())
	{
		return 0;
	}
	return itr->second;
}


bool
AosDocBatchReaderMgr::procStartReqLocked(
		const AosDocBatchReaderReqPtr &request,
		const AosDocBatchReaderPtr &doc_reader,
		const AosRundataPtr &rdata)
{
	u32 client_id = request->client_id;
	return doc_reader->setStartClient(client_id, rdata);
}


bool
AosDocBatchReaderMgr::procFinishedReqLocked(
		const AosDocBatchReaderReqPtr &request,
		const AosDocBatchReaderPtr &doc_reader,
		const AosRundataPtr &rdata)
{
	u32 client_id = request->client_id;
	int total_num_data_msgs = request->total_num_data_msgs;
	return	doc_reader->setFinishedClient(client_id, total_num_data_msgs, rdata);
}


bool
AosDocBatchReaderMgr::signal(const int threadLogicId)
{
	return true;
}


bool
AosDocBatchReaderMgr::checkThread(OmnString &err, const int thrdLogicId) const
{
	return true;
}


bool
AosDocBatchReaderMgr::proc(
		const OmnString &scanner_id,
		const AosDocBatchReaderReq::E type,
		const AosBuffPtr &cont,
		const u32 &client_id,
		const AosRundataPtr &rdata)
{
	// This function is called when a request is received from a client.
	// It creates a request and adds the request to the queue.
	bool rslt = false;
	switch (type)
	{
	case AosDocBatchReaderReq::eStart:
		 rslt = start(scanner_id, client_id, cont, rdata);
		 break;

	case AosDocBatchReaderReq::eFinished:
		 rslt = finished(scanner_id, client_id, cont, rdata);
		 break;

	case AosDocBatchReaderReq::eData:
		 rslt = procData(scanner_id, client_id, cont, rdata); 
		 break;

	case AosDocBatchReaderReq::eStartRead:
		 break;

	default:
		 OmnAlarm << "Unrecognized request type: " << type << enderr;
		 break;
	}
	aos_assert_r(rslt, false);
	return true;
}

/*
bool
AosDocBatchReaderMgr::waitOnReadyCreated(const AosDocBatchReaderReq &ll)
{
	u64 blocksize = ll->block_size;
	u32 queue_size = ll->queue_size;
	while (1)
	{
		mLock->lock();
		if (mAllReaderQueueSize + (blocksize * queue_size) >= eMaxAllReaderQueueSize)
		{
			OmnScreen << "Queue wait !!!!!!!! " << ll->scannerid << " , " << mAllReaderQueueSize << endl;
			OmnAlarm << "Queue Full !!!!!!!!! " << enderr;
			mCondVar->wait(mLock);
			mLock->unlock();
			continue;
		}
		mQueue.push(ll);
		mCondVar->signal();
		mLock->unlock();
		return true;
	}
	return false;
}
*/


bool
AosDocBatchReaderMgr::start(
		const OmnString &scanner_id,
		const u32 &client_id,
		const AosBuffPtr &cont,
		const AosRundataPtr &rdata)
{
	// This function is called when the server receives a Start message. 
	// It creates a request and puts the request into mQueue.
	aos_assert_rr(scanner_id != "", rdata, false);

	u32 queue_size = cont->getU32(0);
	OmnString read_policy = cont->getOmnStr(""); 
	u64 blocksize = cont->getU64(0); 
	OmnString batch_type_str = cont->getOmnStr("read");;
	OmnString field_names_str = cont->getOmnStr("");
	AosXmlTagPtr xml;
	if (field_names_str != "")
	{
		AosXmlParser parser;
		AosXmlTagPtr root = parser.parse(field_names_str, "" AosMemoryCheckerArgs);
		if (root)
		{
			xml = root->getFirstChild();
		}
	}

	AosDocBatchReaderReqPtr request = OmnNew AosDocBatchReaderReq();
	request->rdata = rdata->clone(AosMemoryCheckerArgsBegin);
	request->scannerid = scanner_id;
	request->client_id = client_id;
	request->type = AosDocBatchReaderReq::eStart;
	request->queue_size = queue_size;
	request->read_policy_str = read_policy;
	request->block_size = (blocksize > 0 && blocksize < eMaxBlockSize)? blocksize:eDftBlockSize;
	aos_assert_r(request->block_size > 0, false);
	request->batch_type_str = batch_type_str;
	request->field_names = xml;

	mLock->lock();
	mQueue.push(request);
	mCondVar->signal();
	mLock->unlock();
	// bool rslt = waitOnReadyCreated(request);
	// aos_assert_r(rslt, false);
	return true;
}


bool
AosDocBatchReaderMgr::finished(
		const OmnString &scanner_id,
		const u32 &client_id,
		const AosBuffPtr &cont,
		const AosRundataPtr &rdata)
{
	// This function is called when an End message is received
	// by this class. It creates a request and adds the request
	// to mQueue.
	aos_assert_r(scanner_id != "" && cont, false);
	AosDocBatchReaderReqPtr request = OmnNew AosDocBatchReaderReq();	

	int num_data = cont->getInt(-1); 

	request->rdata = rdata->clone(AosMemoryCheckerArgsBegin);
	request->scannerid = scanner_id;
	request->client_id = client_id;
	request->total_num_data_msgs = num_data;
	request->type = AosDocBatchReaderReq::eFinished;

	mLock->lock();
	mQueue.push(request);
	mCondVar->signal();
	mLock->unlock();
	return true;
}


bool
AosDocBatchReaderMgr::procData(
		const OmnString &scanner_id,
		const u32 &client_id,
		const AosBuffPtr &cont,
		const AosRundataPtr &rdata)
{
	// This function is called when a data message is received. 
	// It creates a request and adds it into mQueue.
	aos_assert_r(scanner_id != "" && cont, false); 
	aos_assert_r(rdata, false);

	bool call_back = cont->getU8(0);
	int	call_back_serverid = cont->getInt(0);
	u32 bm_len = cont->getU32(0);
	AosBuffPtr bm_buff = cont->getBuff(bm_len, false AosMemoryCheckerArgs);
	// AosBitmapObjPtr bitmap = AosBitmapObj::getBitmapStatic();
	AosBitmapObjPtr bitmap = AosGetBitmap();
	bitmap->loadFromBuff(bm_buff);
	aos_assert_r(!bitmap->isEmpty(), false);

	AosDocBatchReaderReqPtr request = OmnNew AosDocBatchReaderReq();	
	request->rdata = rdata->clone(AosMemoryCheckerArgsBegin);
	request->scannerid = scanner_id;
	request->client_id = client_id;
	request->type = AosDocBatchReaderReq::eData;
	request->bitmap = bitmap;
	request->call_back = call_back;
	request->call_back_serverid = call_back_serverid;

	bool rslt = addDataRequest(request, bitmap->getMemorySize(), rdata);
	aos_assert_rr(rslt, rdata, false);
	return true;
}


bool
AosDocBatchReaderMgr::addDataRequest(
		const AosDocBatchReaderReqPtr &request, 
		const u64 &bitmap_size, 
		const AosRundataPtr &rdata)
{
	mLock->lock();
	mTotalBitmapSize += bitmap_size;
	while (mTotalBitmapSize > eMaxBitmapSize)
	{
		flushBitmapsToFiles(rdata);
		mQueueFullCondVar->wait(mLock);
	}

	mQueue.push(request);
	mCondVar->signal();
	mLock->unlock();
	return true;
}


bool
AosDocBatchReaderMgr::flushBitmapsToFiles(const AosRundataPtr &rdata)
{
	OmnNotImplementedYet;
	return false;
}


bool
AosDocBatchReaderMgr::procDataLocked(
		const AosBitmapObjPtr &bitmap,
		const u32 &client_id,
		const AosDocBatchReaderReqPtr &request,
		const AosDocBatchReaderPtr &doc_reader, 
		const AosRundataPtr &rdata)
{
	// This function shuffles docids in 'bitmap' to mBitmaps based on virtual IDs.
	doc_reader->setCallBack(request->call_back);
	doc_reader->setCallBackServerId(request->call_back_serverid);
	return doc_reader->shuffle(bitmap, client_id, rdata);
}


bool
AosDocBatchReaderMgr::retrieveDocs(
		const OmnString &scanner_id,
		AosBuffPtr &resp_buff,
		const AosRundataPtr &rdata)
{
	// This function reads in a block of data and returns the data to 
	// the caller. If nothing to be read, it sets the data to null and 
	// returns.
	mLock->lock();
	AosDocBatchReaderPtr doc_reader = getDocReaderLocked(scanner_id, rdata);
	if (!doc_reader)
	{
		//send a message indicating it is not there yet;
		mLock->unlock();
		return true;
	}

	mLock->unlock();
	bool rslt = doc_reader->retrieveDocs(scanner_id, resp_buff, rdata);
	aos_assert_r(rslt, false);

	mLock->lock();
	rslt = isFinishedLocked(scanner_id, doc_reader, rdata);
	mLock->unlock();
	return true;
}


bool
AosDocBatchReaderMgr::isFinished(
		const OmnString &scanner_id,
		const AosRundataPtr &rdata)
{
	mLock->lock();
	AosDocBatchReaderPtr doc_reader = getDocReaderLocked(scanner_id, rdata);
	if (!doc_reader)
	{
		//send a message indicating it is not there yet;
		mLock->unlock();
		OmnAlarm << "missing" << enderr;
		return false;
	}
	bool rslt = isFinishedLocked(scanner_id, doc_reader, rdata);
	mLock->unlock();
	return rslt;
}


bool
AosDocBatchReaderMgr::isFinishedLocked(
		const OmnString &scanner_id,
		const AosDocBatchReaderPtr &doc_reader, 
		const AosRundataPtr &rdata)
{
	// A scanner finishes if and only if all clients have sent the finished
	// message, and all data were read. 
	if (!doc_reader->isFinished(rdata)) return false;
OmnScreen << "==================== Finish " << scanner_id << endl;
	mAllReaderQueueSize -= (doc_reader->getBlockSize() * doc_reader->getQueueSize());
	mCondVar->signal();

	// It is finished.
	mScanners.erase(scanner_id);
	sendScannerFinishedMsg(scanner_id, rdata);
	return true;
}


bool
AosDocBatchReaderMgr::sendScannerStartedMsg(
		const OmnString &scanner_id,
		const AosRundataPtr &rdata)
{
	// This function sends a ScannerStarted message to its job.
	return true;
}


bool
AosDocBatchReaderMgr::sendScannerFinishedMsg(
		const OmnString &scanner_id,
		const AosRundataPtr &rdata)
{
	// This function sends a Finished message to its job.
	return true;
}


bool
AosDocBatchReaderMgr::procStartRead(
		const u32 client_id,
		const AosDocBatchReaderPtr &doc_reader, 
		const AosRundataPtr &rdata)
{
	// This function is called when a client sent a message indicating that 
	// the server may begin reading data. 
	aos_assert_rr(doc_reader, rdata, false);
	return doc_reader->setStartRead(client_id, rdata);
}


bool
AosDocBatchReaderMgr::readerDestroyed(const u64 &total_received_size)
{
	aos_assert_r(mTotalBitmapSize >= total_received_size, false);
	mTotalBitmapSize -= total_received_size;
	return true;
}


bool
AosDocBatchReaderMgr::callBackClient(
		const OmnString &scanner_id,
		const AosBuffPtr &buff,
		const int call_back_serverid,
		const int num_call_back_data_msgs,
		const AosRundataPtr &rdata)
{
	isFinished(scanner_id, rdata);
	AosGetSelfServerId();
	//Linda. 2013/09/23
	//AosTransPtr trans = OmnNew AosBatchGetDocsResp(
	//		scanner_id, call_back_serverid, serverid, buff, finished, num_call_back_data_msgs);
	//bool rslt = AosSendTrans(rdata, trans);
	//aos_assert_rr(rslt, rdata, false);
	return true;
}


bool
AosDocBatchReaderMgr::deleteDocs(
		const OmnString &scanner_id,
		const AosRundataPtr &rdata)
{
	// This function reads in a block of data and returns the data to 
	// the caller. If nothing to be read, it sets the data to null and 
	// returns.
	mLock->lock();
	AosDocBatchReaderPtr doc_reader = getDocReaderLocked(scanner_id, rdata);
	if (!doc_reader)
	{
		//send a message indicating it is not there yet;
		mLock->unlock();
		return true;
	}

	mLock->unlock();
	bool rslt = doc_reader->deleteDocs(scanner_id, rdata);
	aos_assert_r(rslt, false);

	mLock->lock();
	rslt = isFinishedLocked(scanner_id, doc_reader, rdata);
	mLock->unlock();
	return true;
}


#endif
