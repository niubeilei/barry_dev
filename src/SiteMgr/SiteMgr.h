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
#ifndef AOS_SiteMgr_SiteMgr_h
#define AOS_SiteMgr_SiteMgr_h

#if 0
#include "SingletonClass/SingletonObjId.h"
#include "SingletonClass/SingletonTplt.h"
#include "Thread/ThreadedObj.h"
#include "Util/Queue.h"
#include "Util/String.h"
#include "UtilComm/Ptrs.h"
#include "XmlInterface/Server/Ptrs.h"


OmnDefineSingletonClass(AosSiteMgrSingleton,
						AosSiteMgr,
						AosSiteMgrSelf,
						OmnSingletonObjId::eSiteMgr,
						"SiteMgr");


class AosSiteMgr : virtual public OmnThreadedObj
{
	OmnDefineRCObject;

public:

private:
	OmnMutexPtr		mLock;
	OmnCondVarPtr	mCondVar;
	OmnThreadPtr	mThread;
	OmnString		mSiteId;

public:
	AosSiteMgr();
	~AosSiteMgr();

    // OmnThreadedObj Interface
    virtual bool    threadFunc(OmnThrdStatus::E &state, 
						const OmnThreadPtr &thread);
    virtual bool    signal(const int threadLogicId);
    virtual bool    checkThread(OmnString &err, const int thrdLogicId) const;

    //
    // Singleton class interface
    //
    static AosSiteMgr * getSelf();
    virtual bool      	start();
    virtual bool        stop();
    virtual OmnString   getSysObjName() const {return "AosSiteMgr";}
    virtual OmnSingletonObjId::E  
						getSysObjId() const 
						{
							return OmnSingletonObjId::eSiteMgr;
						}
    virtual OmnRslt     config(const OmnXmlParserPtr &def);

	OmnString	getSiteId() {return mSiteId;}
private:
};
#endif

#endif

