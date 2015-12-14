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
// 05/21/2011	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_LogServer_LogServer_h
#define AOS_LogServer_LogServer_h

#include "LogSvr/Ptrs.h"
#include "Rundata/Ptrs.h"
#include "SEUtil/Ptrs.h"
#include "SingletonClass/SingletonObjId.h"
#include "SingletonClass/SingletonTplt.h"
#include "Thread/Ptrs.h"
#include "Thread/ThreadedObj.h"
#include "Util/Ptrs.h"
#include <queue>
#include <map>

using namespace std;


OmnDefineSingletonClass(AosLogSvrSingleton,
						AosLogSvr,
						AosLogSvrSelf,
						OmnSingletonObjId::eLogSvr,
						"LogServer");


class AosLogSvr : virtual public OmnThreadedObj
{
	OmnDefineRCObject;

public:
	enum
	{
		eIndexRecordSize = 16,
		eMaxLogsPerIndex = 100000000,		// 100 million

		eAosCurrentIndexSeqnoOffset = 0
	};

private:
	OmnMutexPtr				mLock;
	OmnCondVarPtr			mCondVar;
	queue<AosLogReqPtr> 	mQueue;
	OmnString				mLogDir;
	OmnFilePtr				mIndexFile;
	u64						mStartLogid;
	u32						mCrtSeqno;
	map<u32, OmnFilePtr>	mIdxFiles;

public:
	AosLogSvr();
	~AosLogSvr();

    // OmnThreadedObj Interface
    virtual bool    threadFunc(OmnThrdStatus::E &state, 
						const OmnThreadPtr &thread);
    virtual bool    signal(const int threadLogicId);
    virtual bool    checkThread(OmnString &err, const int thrdLogicId) const;

    //
    // Singleton class interface
    //
    static AosLogSvr *    	getSelf();
    virtual bool      	start();
    virtual bool        stop();
    virtual OmnString   getSysObjName() const {return "AosLogSvr";}
    virtual OmnSingletonObjId::E  getSysObjId() const 
						{
							return OmnSingletonObjId::eLogSvr;
						}
    virtual OmnRslt     config(const OmnXmlParserPtr &def);
	bool 	start(const AosXmlTagPtr &config);
	bool 	createLog(const AosXmlTagPtr &log, const AosRundataPtr &rdata);

private:
	u64  		getLogidPriv();
	OmnFilePtr	openIndexFile(const u32 seqno);
	bool		createLogPriv(const AosLogReqPtr &log);
};
#endif
