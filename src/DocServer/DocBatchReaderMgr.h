////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// This type of IIL maintains a list of (string, docid) and is sorted
// based on the string value. 
//
// Modification History:
// 09/24/2012 Created by Linda 
////////////////////////////////////////////////////////////////////////////
#if 0
#ifndef AOS_DocServer_DocBatchReaderMgr_h
#define AOS_DocServer_DocBatchReaderMgr_h

#include "Rundata/Ptrs.h"
#include "SingletonClass/SingletonObjId.h"
#include "SingletonClass/SingletonTplt.h"
#include "SEInterfaces/BitmapObj.h"
#include "SEInterfaces/Ptrs.h"
#include "DocServer/Ptrs.h"
#include "DocServer/DocBatchReaderReq.h"
#include "Thread/Mutex.h"
#include "Thread/ThreadedObj.h"
#include "Thread/Thread.h"
#include "Thread/Ptrs.h"
#include "XmlUtil/DocTypes.h"

#include <vector>
#include <queue>

OmnDefineSingletonClass(AosDocBatchReaderMgrSingleton,         
		AosDocBatchReaderMgr,                  
		AosDocBatchReaderMgrSelf,              
		OmnSingletonObjId::eDocBatchReaderMgr, 
		"DocBatchReaderMgr");                  


class AosDocBatchReaderMgr : virtual public OmnThreadedObj 
{
	OmnDefineRCObject;

	enum
	{
		eMaxAllReaderQueueSize = 2000000000, // 2G
		eMaxBlockSize = 1000000000, //1G
		eDftBlockSize = 20000000, //20M
		eMaxBitmapSize = 1000000000 // 1G 
	};

private:
	OmnThreadPtr								mThread;
	OmnMutexPtr     							mLock;
	OmnCondVarPtr   							mCondVar;
	OmnCondVarPtr   							mQueueFullCondVar;
	map<OmnString, AosDocBatchReaderPtr>		mScanners;
	queue<AosDocBatchReaderReqPtr>          	mQueue;

	u64 										mAllReaderQueueSize;
	u64											mTotalBitmapSize;

public:
	AosDocBatchReaderMgr();
	~AosDocBatchReaderMgr();

    // Singleton class interface
    //
    static AosDocBatchReaderMgr *    	getSelf();
    virtual bool      	start();
    virtual bool        stop();
    virtual bool		config(const AosXmlTagPtr &def);

	//ThreadedObj Interface
	virtual bool    threadFunc(OmnThrdStatus::E &state, const OmnThreadPtr &thread);
	virtual bool    signal(const int threadLogicId);
	virtual bool    checkThread(OmnString &err, const int thrdLogicId) const;


	bool 	proc(
			const OmnString &scanner_id,
			const AosDocBatchReaderReq::E type,
			const AosBuffPtr &cont,
			const u32 &client_id,
			const AosRundataPtr &rdata);

	bool 	retrieveDocs(
			const OmnString &scanner_id,
			AosBuffPtr &resp_buff,
			const AosRundataPtr &rdata);

	bool	readerDestroyed(
			const u64 &total_received_size);

	bool	deleteDocs(
			const OmnString &scanner_id,
			const AosRundataPtr &rdata);

	bool	callBackClient(
			const OmnString &scanner_id,
			const AosBuffPtr &buff,
			const int call_back_serverid,
			const int num_call_back_data_msgs,
			const AosRundataPtr &rdata);

	bool	isFinishedLocked(
			const OmnString &scanner_id,
			const AosDocBatchReaderPtr &doc_reader,
			const AosRundataPtr &rdata);
private:
	AosDocBatchReaderPtr getDocReaderLocked(
			const AosDocBatchReaderReqPtr &request,
			const AosRundataPtr &rdata);

	AosDocBatchReaderPtr getDocReaderLocked(
			const OmnString &scanner_id,
			const AosRundataPtr &rdata);
	
	bool	procStartReqLocked(
			const AosDocBatchReaderReqPtr &request,
			const AosDocBatchReaderPtr &doc_reader,
			const AosRundataPtr &rdata);

	bool	procFinishedReqLocked(
			const AosDocBatchReaderReqPtr &request,
			const AosDocBatchReaderPtr &doc_reader,
			const AosRundataPtr &rdata);

	bool	procDataLocked(
			const AosBitmapObjPtr &bitmap,
			const u32 &client_id,
			const AosDocBatchReaderReqPtr &request,
			const AosDocBatchReaderPtr &doc_reader,
			const AosRundataPtr &rdata);

	bool	isFinished(
			const OmnString &scanner_id,
			const AosRundataPtr &rdata);

	bool	sendScannerStartedMsg(
			const OmnString &scanner_id,
			const AosRundataPtr &rdata);

	bool	sendScannerFinishedMsg(
			const OmnString &scanner_id,
			const AosRundataPtr &rdata);

	bool	procStartRead(
			const u32 client_id,
			const AosDocBatchReaderPtr &doc_reader,
			const AosRundataPtr &rdata);

	bool	waitOnReadyCreated(const AosDocBatchReaderReqPtr &ll);

	bool 	start(
			const OmnString &scanner_id,
			const u32 &client_id,
			const AosBuffPtr &cont,
			const AosRundataPtr &rdata);

	bool	addDataRequest(
			const AosDocBatchReaderReqPtr &request,
			const u64 &bitmap_size,
			const AosRundataPtr &rdata);

	bool	flushBitmapsToFiles(
			const AosRundataPtr &rdata);

	bool 	finished(
			const OmnString &scanner_id,
			const u32 &client_id,
			const AosBuffPtr &cont,
			const AosRundataPtr &rdata);

	bool 	procData(
			const OmnString &scanner_id,
			const u32 &client_id,
			const AosBuffPtr &cont,
			const AosRundataPtr &rdata);
};
#endif

#endif
