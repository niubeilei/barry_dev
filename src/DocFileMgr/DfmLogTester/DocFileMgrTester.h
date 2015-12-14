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
#ifndef Aos_DocFileMgr_DfmLogTester_DocFileMgrTester_h
#define Aos_DocFileMgr_DfmLogTester_DocFileMgrTester_h 

#include "DocFileMgr/Ptrs.h"
#include "DfmUtil/DfmConfig.h"
#include "DocFileMgr/DfmUtil.h"
#include "DocFileMgr/DfmLogTester/Ptrs.h"
#include "Util/Opr.h"
#include "Tester/TestPkg.h"
#include "Thread/ThreadedObj.h"

class AosDocFileMgrTester : public OmnTestPkg,
							public OmnThreadedObj
{

public:
	enum
	{
		eSiteNum = 1,
		eThreadNum = 8,
	
		//eDfmType = AosDfmType::eSnapShotDfm,
		eDfmType = AosDfmType::eNormDfm,
		eSnapNum = 10,

		eCreateWeight = 60,
		eModifyWeight = 20,
		eDeleteWeight = 10,
		eReadWeight = 20,
		
		eAddSnapWeight = 50,
		eRmSnapWeight = 20,
		eRbWeight = 10,
		eCommitWeight = 30,
		//eRmSnapWeight = 0,
		//eRbWeight = 0,
		//eCommitWeight = 0,

	};

private:
	OmnMutexPtr	 mLock;
	OmnFilePtr	 mFile;
	OmnCondVarPtr	mCondVar;
	AosDfmConfig mDfmConf;
	u64			mCrtMaxDocid;
	map<u64, AosDfmInfoPtr>	mDfmInfo;
		
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
	bool	createDoc(const AosRundataPtr &rdata);
	bool	modifyDoc(const AosRundataPtr &rdata);
	bool	deleteDoc(const AosRundataPtr &rdata);
	bool	readDoc(const AosRundataPtr &rdata);

	bool 	killThrdFunc(
			  OmnThrdStatus::E &state,
			  const OmnThreadPtr &thread);
	bool 	basicThrdFunc(
			  OmnThrdStatus::E &state,
			  const OmnThreadPtr &thread);
	
	bool 	snapShotTest();
	bool	addSnapShot(
			const AosRundataPtr &rdata,
			const AosDfmInfoPtr &dfm_info);
	bool 	removeSnapShot(
			const AosRundataPtr &rdata,
			const AosDfmInfoPtr &dfm_info);
	bool 	rollBack(
			const AosRundataPtr &rdata,
			const AosDfmInfoPtr &dfm_info);
	bool 	commit(
			const AosRundataPtr &rdata,
			const AosDfmInfoPtr &dfm_info);

	AosDfmInfoPtr randGetDfmInfo();
	AosDfmInfoPtr getDfmInfo(const u64 docid);
	
	bool 	needKill();
	bool 	readyKill(const AosDfmInfoPtr &dfm_info);
	bool 	saveToFile();
	
};
#endif
