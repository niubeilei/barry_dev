////////////////////////////////////////////////////////////////////////////
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
//
// Modification History:
// 03/11/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_BatchQuery_BatchQueryMgr_h
#define Aos_BatchQuery_BatchQueryMgr_h

#include "BatchQuery/Ptrs.h"
#include "Rundata/Ptrs.h"
#include "Thread/Ptrs.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "XmlUtil/Ptrs.h"
#include <queue>
using namespace std;

class AosBatchQueryMgr : public OmnRCObject
{
	OmnDefineRCObject;

private:
	enum
	{
		eDftMaxOustandingThreads = 50,
		eDftMaxIdleThreads = 10
	};

	OmnMutexPtr					mLock;
	OmnCondVarPtr				mCondVar;
	queue<AosBatchQueryPtr>		mQueryQueue;
	queue<AosBatchQueryThrdPtr>	mIdleThreads;
	int							mOutstandingThreads;
	u32							mMaxOustandingThreads;
	u32							mMaxIdleThreads;

public:
	AosBatchQueryMgr();
	~AosBatchQueryMgr();

	bool config(const AosXmlTagPtr &conf);
	bool start();
	bool stop();

	// ThreadedObj Interface
	// virtual bool    threadFunc(OmnThrdStatus::E &state, const OmnThreadPtr &thread);
	// virtual bool    signal(const int threadLogicId);
	// virtual bool    checkThread(OmnString &err, const int thrdLogicId) const;

	bool addBatchQuery(const AosBatchQueryPtr &query, const AosRundataPtr &rdata);
	AosBatchQueryPtr threadFinished(const AosBatchQueryThrdPtr &thread);
};
#endif

