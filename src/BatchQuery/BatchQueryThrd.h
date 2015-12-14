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
#ifndef Aos_BatchQuery_BatchQueryThrd_h
#define Aos_BatchQuery_BatchQueryThrd_h

#include "BatchQuery/Ptrs.h"
#include "Rundata/Ptrs.h"
#include "Thread/Ptrs.h"
#include "Thread/ThreadedObj.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "XmlUtil/Ptrs.h"

class AosBatchQueryThrd : public OmnThreadedObj
{
	OmnDefineRCObject;

public:

private:
	OmnMutexPtr			mLock;
	OmnCondVarPtr		mCondVar;
	OmnThreadPtr		mThread;
	AosBatchQueryMgrPtr	mQueryMgr;
	AosBatchQueryPtr	mQuery;

public:
	AosBatchQueryThrd(const AosBatchQueryMgrPtr &query_mgr);
	~AosBatchQueryThrd();

	// ThreadedObj Interface
	virtual bool    threadFunc(OmnThrdStatus::E &state, const OmnThreadPtr &thread);
	virtual bool    signal(const int threadLogicId);
	virtual bool    checkThread(OmnString &err, const int thrdLogicId) const;

	bool setQuery(const AosBatchQueryPtr &query);
	bool stop();

private:
	bool procOneQuery(const AosBatchQueryPtr &query);
	bool procQueryPriv(const AosBatchQueryPtr &query);
	bool setQuery(const AosBatchQueryPtr &query, const AosRundataPtr &rdata);
};
#endif

