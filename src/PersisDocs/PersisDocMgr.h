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
// 01/20/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_PersisDocs_PersisDocMgr_h
#define AOS_PersisDocs_PersisDocMgr_h

#include "SEInterfaces/PersisDocMgrObj.h"
#include "SingletonClass/SingletonObjId.h"
#include "SingletonClass/SingletonTplt.h"
#include "Thread/ThreadedObj.h"
#include "Thread/Ptrs.h"


OmnDefineSingletonClass(AosPersisDocMgrSingleton,
						AosPersisDocMgr,
						AosPersisDocMgrSelf,
						OmnSingletonObjId::ePersisDocMgr,
						"PersisDocMgr");

class AosPersisDocMgr : virtual public OmnThreadedObj, public AosPersisDocMgrObj
{
	OmnDefineRCObject;

private:
	OmnMutexPtr		mLock;
	OmnCondVarPtr	mCondVar;

public:
	AosPersisDocMgr();
	~AosPersisDocMgr();

    // OmnThreadedObj Interface
    virtual bool    threadFunc(OmnThrdStatus::E &state, const OmnThreadPtr &thread);
    virtual bool    signal(const int threadLogicId);
    virtual bool    checkThread(OmnString &err, const int thrdLogicId) const;

    // Singleton class interface
    static AosPersisDocMgr * 	getSelf();
    virtual bool      			start();
    virtual bool        		stop();
    virtual bool				config(const AosXmlTagPtr &def);
};
#endif
