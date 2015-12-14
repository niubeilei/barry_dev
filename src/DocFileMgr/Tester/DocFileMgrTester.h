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
// 2011/06/03	Created by Ketty 
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_DocFileMgr_Tester_DocFileMgrTester_h
#define Aos_DocFileMgr_Tester_DocFileMgrTester_h 

#include "DocFileMgr/Ptrs.h"
#include "DfmUtil/DfmConfig.h"
#include "DocFileMgr/DfmUtil.h"
#include "DocFileMgr/Tester/Ptrs.h"
#include "Util/Opr.h"
#include "Tester/TestPkg.h"
#include "Thread/ThreadedObj.h"

class AosDocFileMgrTester : public OmnTestPkg,
							public OmnThreadedObj
{

public:
	enum
	{
		//eThreadNum = 10,
		eThreadNum = 1,
	
		eSnapNum = 10,

		eAddWeight = 50,
		eModifyWeight = 20,
		eDeleteWeight = 5,
		eReadWeight = 30,
		
		eAddToSnapshotWt = 50,
		eModifyFromSnapshotWt = 20,
		eDeleteFromSnapshotWt = 5,
		eReadFromSnapshotWt = 30,

		eAddSnapWeight = 60,
		eRollbackWeight = 5,
		eCommitWeight = 35,
		eMergeWeight = 15,

	};

private:
	AosRundataPtr mRdata;
	OmnMutexPtr	 mLock;
	OmnFilePtr	 mFile;
	OmnCondVarPtr	mCondVar;
	AosDfmConfig mDfmConf;
	AosDfmInfoPtr mDfmInfo;
		
	bool		mThrdStoped[eThreadNum];

public:
	AosDocFileMgrTester();
	~AosDocFileMgrTester();

	virtual bool		start();
	
	// ThreadedObj Interface
	virtual bool    threadFunc(OmnThrdStatus::E &state, const OmnThreadPtr &thread);
	virtual bool    checkThread(OmnString &err, const int thrdLogicId) const{ return true;};
	virtual bool    signal(const int threadLogicId){ return true; };

private:
	void	init();
	bool 	basicTest();
	bool 	normProc();
	bool 	snapProc();

	bool 	killThrdFunc(
			  OmnThrdStatus::E &state,
			  const OmnThreadPtr &thread);
	bool 	basicThrdFunc(
			  OmnThrdStatus::E &state,
			  const OmnThreadPtr &thread);
	
	bool 	snapShotTest();
	bool	addSnapShot(const AosRundataPtr &rdata);
	bool 	removeSnapShot( const AosRundataPtr &rdata);
	bool 	rollback(const AosRundataPtr &rdata);
	bool 	commit(const AosRundataPtr &rdata);
	bool 	merge(const AosRundataPtr &rdata);

	bool 	needKill();
	bool 	readyKill();
	bool 	saveToFile();
	
};
#endif
