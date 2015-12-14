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
// 09/26/2009	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#if 0
#ifndef AOS_Query_QueryMgr_h
#define AOS_Query_QueryMgr_h

#include "Query/Ptrs.h"
#include "SingletonClass/SingletonObjId.h"
#include "SingletonClass/SingletonTplt.h"
#include "Thread/Ptrs.h"
#include "Thread/ThreadedObj.h"
#include "Util/File.h"
#include "XmlInterface/XmlRc.h"


OmnDefineSingletonClass(AosQueryMgrSingleton,
						AosQueryMgr,
						AosQueryMgrSelf,
						OmnSingletonObjId::eQueryMgr,
						"QueryMgr");



class AosQueryMgr : virtual public OmnThreadedObj
{
	OmnDefineRCObject;

public:
	enum 
	{
		eMaxQueries = 100,
		eIncSize = 10
	};

private:
	OmnMutexPtr		mLock;
	OmnCondVarPtr	mCondVar;
	OmnThreadPtr	mThread;
	AosQueryReqPtr	mQueries[eMaxQueries];
	i64				mNumQueries;
	i64				mStartIdx;
	i64				mCrtQid;
	i64				mCrtPos;
	i64				mCrtStartPos;
	i64				mTotalQueries;

public:
	AosQueryMgr();
	~AosQueryMgr();

    // OmnThreadedObj Interface
    virtual bool    threadFunc(OmnThrdStatus::E &state, 
						const OmnThreadPtr &thread);
    virtual bool    signal(const int threadLogicId);
    virtual bool    checkThread(OmnString &err, const int thrdLogicId) const;

    //
    // Singleton class interface
    //
    static AosQueryMgr *    	getSelf();
    virtual bool      	start();
    virtual bool        stop();
    virtual OmnString   getSysObjName() const {return "AosQueryMgr";}
    virtual OmnSingletonObjId::E  
						getSysObjId() const 
						{
							return OmnSingletonObjId::eQueryMgr;
						}
    virtual OmnRslt     config(const OmnXmlParserPtr &def);

	AosQueryTermObjPtr	getSession(const OmnString &session);
	AosQueryReqPtr	getQuery(const OmnString &querid);
	bool			queryFinished(const AosQueryReqPtr &query);

private:
};

#endif
#endif
