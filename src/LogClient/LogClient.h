//////////////////////////////////////////////////////////////////////////
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
// Modification History:
// 04/13/2009: Created by Sharon Shen
// 01/01/2013: Rewritten by Chen Ding
//////////////////////////////////////////////////////////////////////////
#ifndef AOS_LogClient_LogClient_h
#define AOS_LogClient_LogClient_h

#include "LogUtil/Ptrs.h"
#include "Rundata/Ptrs.h"
#include "SEInterfaces/SeLogClientObj.h"
#include "SingletonClass/SingletonTplt.h"
#include "SingletonClass/SingletonObjId.h"
#include "Thread/ThreadedObj.h"
#include "Thread/Ptrs.h"
#include "TransClient/Ptrs.h"
#include "Util/CompUint.h"
#include "XmlUtil/Ptrs.h"
#include <queue>
using namespace std;


OmnDefineSingletonClass(AosSeLogClientSingleton,
						AosSeLogClient,
						AosSeLogClientSelf,
						OmnSingletonObjId::eSeLogClient,
						"SeLogClient");

class AosSeLogClient :  public AosSeLogClientObj, 
						public OmnThreadedObj
{
	OmnDefineRCObject;

private:
	typedef hash_map<const u64, AosLogEntryCtnrPtr, u64_hash, u64_cmp> map_t;
	typedef hash_map<const u64, AosLogEntryCtnrPtr, u64_hash, u64_cmp>::iterator mapitr_t;

	OmnMutexPtr				mLock;
	OmnCondVarPtr			mCondVar;
	OmnCondVarPtr			mFullCondVar;
	map_t					mMap;
	queue<AosLogEntryPtr>	mPendingQueue;
	AosTransModuleCltPtr	mTransClient;
	u64						mMaxQueueSize;
	int						mFullWaiters;
	u64						mCrtMax;
	AosLogEntryCtnrPtr		mHeader;

public:
	AosSeLogClient();
	~AosSeLogClient();

	// Singleton Class Interface
	static AosSeLogClient*    getSelf();
	virtual bool	start();
	virtual bool 	stop();
	virtual bool	config(const AosXmlTagPtr &def);

	// OmnThreadedObj interface
	virtual bool	threadFunc(OmnThrdStatus::E &state, const OmnThreadPtr &thread);
	virtual bool	signal(const int threadLogicId);
    virtual void    heartbeat(const int tid); 

	bool addLog( 	const AosLogEntryPtr &log_entry,
					const AosRundataPtr &rdata);

	bool rebuildLogEntry(
		    		const AosXmlTagPtr &logdoc,
		    		const u32 seqno,
		    		const u32 offset,
		    		const AosRundataPtr &rdata);

	bool rebuildLogEntry(
		        	const AosXmlTagPtr &logdoc,
					const AosRundataPtr &rdata);

	AosXmlTagPtr retrieveLog(
					const AosCompUint &logid,
					const AosRundataPtr &rdata);

	static void		startLog();
	void setTransClient(const AosTransClientPtr &client);

private:
	bool procOneEntry(const AosLogEntryPtr &entry);
	AosCompUint getNextDocid(
					const AosXmlTagPtr &container, 
					const AosRundataPtr &rdata);

};
#endif

