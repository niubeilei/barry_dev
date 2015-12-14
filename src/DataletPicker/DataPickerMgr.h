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
// 2013/03/21 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_IILCache_DataPickerMgr_h
#define AOS_IILCache_DataPickerMgr_h

#include "Rundata/Ptrs.h"
#include "SingletonClass/SingletonObjId.h"
#include "SingletonClass/SingletonTplt.h"
#include "Util/RCObject.h"
#include "Util/RCOBjImp.h"
#include <queue>
using namespace std;

OmnDefineSingletonClass(AosDataPickerMgrSingleton,
						AosDataPickerMgr,
						AosDataPickerMgrSelf,
						OmnSingletonObjId::eDataPickerMgr,
						"DataPickerMgr");

class AosDataPickerMgr : public OmnThreadedObj
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
	AosDataPickerMgr();
	~AosDataPickerMgr();

    // Singleton class interface
    static AosDataPickerMgr *    	getSelf();
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

inline AosDataPickerMgr *AosGetDataPickerMgr() {return AosDataPickerMgr::getSelf();}
#endif

