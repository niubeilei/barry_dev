////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// Modification History:
// 12/17/2009	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_SiteMgr_SyncServer_h
#define AOS_SiteMgr_SyncServer_h
#if 0
#include "SingletonClass/SingletonObjId.h"
#include "SingletonClass/SingletonTplt.h"
#include "Thread/ThreadedObj.h"
#include "Util/Queue.h"
#include "Util/String.h"
#include "UtilComm/Ptrs.h"
#include "XmlInterface/Server/Ptrs.h"


OmnDefineSingletonClass(AosSyncServerSingleton,
						AosSyncServer,
						AosSyncServerSelf,
						OmnSingletonObjId::eSyncServer,
						"SyncServer");


class AosSyncServer : virtual public OmnThreadedObj
{
	OmnDefineRCObject;

public:
	enum
	{
		eQueueInitSize = 1000,
		eQueueIncSize = 100,
		eQueueMaxSize = 10000,
	};

	enum ReqType
	{
	};

private:
	OmnMutexPtr		mLock;
	OmnCondVarPtr	mCondVar;
	OmnThreadPtr	mThread;

	OmnQueue<OmnConnBuffPtr, eQueueInitSize, eQueueIncSize, eQueueMaxSize>	mRequests;

public:
	AosSyncServer();
	~AosSyncServer();

    // OmnThreadedObj Interface
    virtual bool    threadFunc(OmnThrdStatus::E &state, 
						const OmnThreadPtr &thread);
    virtual bool    signal(const int threadLogicId);
    virtual bool    checkThread(OmnString &err, const int thrdLogicId) const;

    //
    // Singleton class interface
    //
    static AosSyncServer *    	getSelf();
    virtual bool      	start();
    virtual bool        stop();
    virtual OmnString   getSysObjName() const {return "AosSyncServer";}
    virtual OmnSingletonObjId::E  
						getSysObjId() const 
						{
							return OmnSingletonObjId::eDocServer;
						}
    virtual OmnRslt     config(const OmnXmlParserPtr &def);

	bool		getDataidFromServer(const OmnString &idname, 
					const AosSyncServerReqCbPtr &caller);
	OmnString	getDataidBlock(int &num);
	bool		addRequest(const OmnConnBuffPtr &buff);
	bool 		sendRequest(const OmnString &req);

private:
};

#endif
#endif
