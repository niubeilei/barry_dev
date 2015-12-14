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
// 2013/03/26 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_Selector_SelectorMgr_h
#define AOS_Selector_SelectorMgr_h

#include "Rundata/Ptrs.h"
#include "SingletonClass/SingletonObjId.h"
#include "SingletonClass/SingletonTplt.h"
#include "Util/RCObject.h"
#include "Util/RCOBjImp.h"
#include <queue>
using namespace std;

OmnDefineSingletonClass(AosSelectorMgrSingleton,
						AosSelectorMgr,
						AosSelectorMgrSelf,
						OmnSingletonObjId::eSelectorMgr,
						"SelectorMgr");

class AosSelectorMgr : public OmnThreadedObj
{
	OmnDefineRCObject;

private:
	struct Entry
	{
		OmnString		query;
		AosRundataPtr	rundata;
		AosDQProcPtr	proc;
	};

	OmnMutexPtr		mLock;
	OmnCondVarPtr	mCondVar;
	queue<Entry>	mRequests;

public:
	AosSelectorMgr();
	~AosSelectorMgr();

    // Singleton class interface
    static AosSelectorMgr *    	getSelf();
    virtual bool      	start();
    virtual bool        stop();
    virtual bool		config(const AosXmlTagPtr &def);

	// OmnThreadedObj interface
	virtual bool threadFunc(OmnThrdStatus::E &, const OmnThreadPtr &);
	virtual bool signal(const int threadLogicId);
	virtual bool checkThread(OmnString &errmsg, const int tid) const;
	
	bool run(const AosRundataPtr &rdata, 
				const OmnString &query,
				const AosDQProcPtr &proc);

private:
};

inline AosSelectorMgr *AosGetSelectorMgr() {return AosSelectorMgr::getSelf();}
#endif

