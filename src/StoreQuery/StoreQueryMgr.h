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
#ifndef AOS_StoreQueryMgr_StoreQueryMgr_h
#define AOS_StoreQueryMgr_StoreQueryMgr_h

#include "SingletonClass/SingletonObjId.h"
#include "SingletonClass/SingletonTplt.h"
#include "Util/String.h"
#include "XmlInterface/XmlRc.h"
#include "Thread/ThreadedObj.h" 
#include "StoreQuery/Ptrs.h" 
#include "Rundata/Rundata.h"
#include "Thread/Sem.h" 
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


OmnDefineSingletonClass(AosStoreQueryMgrSingleton,
						AosStoreQueryMgr,
						AosStoreQueryMgrSelf,
						OmnSingletonObjId::eStoreQueryMgr,
						"StoreQueryMgr");

class AosStoreQueryMgr :  public OmnThreadedObj
{
	OmnDefineRCObject;

public:
	enum 
	{
		eQueryThrd,
		eMaxBlockNums = 1000
	};

private:
	OmnMutexPtr				mQueryLock;
	OmnCondVarPtr			mQueryCondVar;
	OmnThreadPtr 			mQueryThrd;

	//start : the resource need to be locked
	queue<AosStoreQueryPtr>	mQueries;
	//end : the resource need to be locked

	AosStoreQueryMgr();
	~AosStoreQueryMgr();

public:
    // Singleton class interface
    static AosStoreQueryMgr* 	getSelf();
    virtual bool      	start();
    virtual bool        stop();
    virtual bool		config(const AosXmlTagPtr &def);

	// ThreadedObj interface
	virtual bool	threadFunc(OmnThrdStatus::E &state, 
							   const OmnThreadPtr &thread);
	virtual bool	signal(const int threadLogicId);
//    virtual bool    checkThread(OmnString &err, const int thrdLogicId) const;

	//self function
public:
	bool procQuery(const AosStoreQueryPtr &query);
	bool addQuery(const AosXmlTagPtr &query, const AosRundataPtr &rdata);

};

#endif

