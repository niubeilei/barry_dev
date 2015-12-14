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
// 08/09/2011	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_SearchEngine_WorkMgr_h
#define AOS_SearchEngine_WorkMgr_h

#include "SingletonClass/SingletonObjId.h"
#include "SingletonClass/SingletonTplt.h"
#include "Thread/Ptrs.h"
#include "Util/RCObject.h"


OmnDefineSingletonClass(AosWorkMgrSingleton,
						AosWorkMgr,
						AosWorkMgrSelf,
						OmnSingletonObjId::eWorkMgr,
						"WorkMgr");


class AosWorkMgr : virtual public OmnRCObject
{
	OmnDefineRCObject;

private:
	OmnMutexPtr			mLock;
	OmnCondVarPtr		mCondVar;

	AosWorkMgr();
	~AosWorkMgr();

public:
    // Singleton class interface
    static AosWorkMgr *    	getSelf();
    virtual bool   	start();
    virtual bool    stop();
    virtual bool	config(const AosXmlTagPtr &def);

	bool addContainerMemberTask(
			const OmnString &ctnr_objid,
			const OmnString &aname, 
			const bool value_unique,
			const bool docid_unique, 
			const AosRundataPtr &rdata);
};
#endif
