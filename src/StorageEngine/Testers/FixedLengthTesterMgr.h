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
// 2013/03/24 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_StorageEngine_Testers_FixedLengthTesterMgr_h
#define Aos_StorageEngine_Testers_FixedLengthTesterMgr_h

#include "StorageEngine/Ptrs.h"
#include "StorageEngine/Testers/Ptrs.h"
#include "Rundata/Ptrs.h"
#include "SEInterfaces/Ptrs.h"
#include "SEInterfaces/BmpExeCaller.h"
#include "Tester/TestPkg.h"
#include "Thread/Ptrs.h"
#include "Thread/Mutex.h"
#include "Thread/ThreadedObj.h"
#include "Thread/ThreadShellMgr.h"
#include "Thread/ThrdShellProc.h"
#include "XmlUtil/XmlTag.h"
#include "Util/Opr.h"
#include "Util/Ptrs.h"
#include <vector>
using namespace std;


class AosFixedLengthTesterMgr : public OmnTestPkg , public OmnThreadedObj
{
private:
	enum
	{
		eMaxThrds = 10,
		eDftRecordLength = 81,
		eDftTries = 10
	};

	int							mTries;
	int 						mRecordLength;
	AosXmlTagPtr				mRecordDoc;
	int							mNumThreads;
	OmnThreadPtr				mThread[eMaxThrds];
	OmnMutexPtr     			mLock[eMaxThrds];
	OmnCondVarPtr   			mCondVar[eMaxThrds];
	AosRundataPtr				mRundata[eMaxThrds];
	static AosFixedLengthTesterPtr		smFixedLength[eMaxThrds];
	u64							mJobId[eMaxThrds +1];
	static OmnMutexPtr					smTransLock;
	int							mVary[eMaxThrds];
public:
	AosFixedLengthTesterMgr();
	~AosFixedLengthTesterMgr();

	// OmnThreadedObj interface
	virtual bool    threadFunc(OmnThrdStatus::E &state, const OmnThreadPtr &thread);
	virtual bool    signal(const int threadLogicId);

	bool    checkThread(OmnString &err, const int thrdLogicId) const;

	virtual bool		start();

private:

	bool	basicTest(const AosRundataPtr &rdata);
	bool	init(const AosRundataPtr &rdata);
	bool	config();

	void	startThread(const AosRundataPtr &rdata);
	bool	createNormalDoc(const AosRundataPtr &rdata);
public:
	static void docFinished(const u64 &jobid, const u32 &serverid);
};
#endif
