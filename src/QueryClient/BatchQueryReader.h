////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
//
////////////////////////////////////////////////////////////////////////////
#if 0
#ifndef AOS_BatchQueryReader_BatchQueryReader_h
#define AOS_BatchQueryReader_BatchQueryReader_h

#include "SingletonClass/SingletonObjId.h"
#include "SingletonClass/SingletonTplt.h"
#include "Util/String.h"
#include "XmlInterface/XmlRc.h"
#include "Thread/ThreadedObj.h" 
#include "QueryClient/Ptrs.h" 
#include "Rundata/Rundata.h"
#include "Thread/Sem.h" 
#include "SEInterfaces/DocClientCaller.h" 
#include <map>
#include <deque>
#include <queue>
#include <list>

#include <sys/types.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
using namespace std;


OmnDefineSingletonClass(AosBatchQueryReaderSingleton,
						AosBatchQueryReader,
						AosBatchQueryReaderSelf,
						OmnSingletonObjId::eBatchQueryReader,
						"BatchQueryReader");

class AosBatchQueryReader :  public OmnThreadedObj, public AosDocClientCaller
{
	OmnDefineRCObject;

public:
	enum 
	{
		eMaxBuffSize = 1000000000,
		eSendThrd,
		eRecieveThrd,
		eAssemblingThrd,
		eMaxQueryQueueNums = 1000000, 
		eMaxDocsQueueSize = 10000000, 
//		eMaxBlockQueueSize = 10000000 
	};

	struct query_result
	{
		OmnString				queryid;
		OmnString				shuffer_idx;
		AosBitmapObjPtr 		bitmap;
		AosXmlTagPtr			fnames;	
	};

	struct query_docs
	{
		OmnString	queryid;
		AosBuffPtr	buff;
	};

	struct query_block
	{
		OmnString	queryid;
		int			blockidx;
		int			blocksize;
	};
private:
	int						mQueryQueueNums;
	int						mDocsQueueSize;
	int						mBlockQueueSize;
	int						mBuffSize;
	OmnMutexPtr				mBatchQueryLock;

	OmnMutexPtr				mSendLock;
	OmnCondVarPtr			mSendCondVar;
	OmnThreadPtr 			mSendThrd;

	OmnMutexPtr				mRecieveLock;
	OmnCondVarPtr			mRecieveCondVar;
	OmnThreadPtr 			mRecieveThrd;

	//start : the resource need to be locked
	map<OmnString, AosBatchQueryPtr>		mBatchQueries;
	queue<query_result>  			mQueryQueue;
	deque<query_docs>				mDocsQueue;
	bool							mThreadStatus1;
	bool							mThreadStatus2;
	//end : the resource need to be locked

	AosBatchQueryReader();
	~AosBatchQueryReader();

public:
    // Singleton class interface
    static AosBatchQueryReader* 	getSelf();
    virtual bool      	start();
    virtual bool        stop();
    virtual bool		config(const AosXmlTagPtr &def);

	// ThreadedObj interface
	virtual bool	threadFunc(OmnThrdStatus::E &state, 
							   const OmnThreadPtr &thread);
	virtual bool	signal(const int threadLogicId);
    //virtual bool    checkThread(OmnString &err, const int thrdLogicId) const;
    virtual bool    checkThread111(OmnString &err, const int thrdLogicId);
	//AosDocClientCaller interface
	virtual bool docClientCallback(const AosRundataPtr &rdata, const OmnString &queryid, const AosBuffPtr &buff, const bool finished);

	//self function
public:
	bool addBatchQuery(OmnString &queryid, AosBatchQueryPtr &batchquery, const AosRundataPtr &rdata);
	bool addQueryRslt(OmnString &queryid, OmnString &shuffer_idx, AosQueryRsltObjPtr &queryrslt, const AosXmlTagPtr &fnames, const AosRundataPtr &rdata);
	bool isFull(const AosRundataPtr &rdata);

private:
	bool send(OmnThrdStatus::E &state);
	bool recieve(OmnThrdStatus::E &state, AosRundataPtr &rdata);
	OmnString getShufferIdFromQueryId(const OmnString &queryid, OmnString &shuffer_idx);
	OmnString getQueryIdFromShufferId(const OmnString &shufferid);
	int	getBuffSize(const AosRundataPtr &rdata);

};

#endif
#endif

