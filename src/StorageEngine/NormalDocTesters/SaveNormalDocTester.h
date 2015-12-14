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
#ifndef Aos_StorageEngine_NormalDocTesters_SaveNormalDocTester_h
#define Aos_StorageEngine_NormalDocTesters_SaveNormalDocTester_h 

#include "StorageEngine/NormalDocTesters/Ptrs.h"
#include "DocFileMgr/Ptrs.h"
#include "StorageEngine/Ptrs.h"
#include "SEInterfaces/Ptrs.h"
#include "Rundata/Ptrs.h"
#include "Rundata/Rundata.h"
#include "XmlUtil/XmlTag.h"
#include "Util/Opr.h"
#include "Tester/TestPkg.h"
#include "Thread/ThreadedObj.h"
#include "Thread/ThrdShellProc.h"
#include "SEInterfaces/ActionCaller.h"
#include "DocFileMgr/Ptrs.h"
#include "DocFileMgr/RaidFile.h"
#include <dirent.h>


class AosSaveNormalDocTester : public OmnTestPkg , public OmnThreadedObj 
{

public:
	enum
	{
		eThreadNum = 20,
		//eMaxFileSize = 100000000000, // 50G
		eMaxFileSize = 3000000000, // 1G
		eMaxReadSize = 100000000
		//eMaxReadSize = 10000000 
	};
private:

	struct ReadData : public OmnThrdShellProc
	{
		OmnDefineRCObject;
		
		AosSaveNormalDocTesterPtr mCaller;

		int				mIdx;
		AosRundataPtr 	mRundata;

		ReadData(
				const AosRundataPtr &rdata, 
				const int idx, 
				const AosSaveNormalDocTesterPtr &caller)
		:
		OmnThrdShellProc("test"),
		mCaller(caller),
		mIdx(idx),
		mRundata(rdata)
		{
		}

		bool run()
		{
			mCaller->createData(mIdx, mRundata);
			mCaller->addReq(mIdx, mRundata);
			return true;
		}

		bool procFinished() {return true;}
	};


private:
	vector<u64>			mFileId;
	vector<bool>		mCreateFlag;
	static map<u64, bool>	smFlag;

	//vector<u64>			mDocids[10];
	u64					mStartDocid;
	OmnThreadPtr 		mThread;
//	OmnMutexPtr         mLock;   
//	OmnCondVarPtr       mCondVar;
	AosRundataPtr		mRundata;

public:
	AosSaveNormalDocTester();
	~AosSaveNormalDocTester();

	// OmnThreadedObj interface
	virtual bool    threadFunc(OmnThrdStatus::E &state, const OmnThreadPtr &thread);
	virtual bool    signal(const int threadLogicId);

	bool    checkThread(OmnString &err, const int thrdLogicId) const;

	virtual bool		start();
	
	void	createData(const int idx, const AosRundataPtr &rdata);

	static void update(const u64 &fileid)
	{
cout << "------------------------------------------update: " << fileid << endl;
		smFlag[fileid] = true;
	}
private:
	bool 	basicTest(const AosRundataPtr &rdata);

	OmnLocalFilePtr	createFileLocked(const int idx, const AosRundataPtr &rdata);

	AosBuffPtr	readyData(
			const int idx, 
			const int size,
			const AosRundataPtr &rdata);

	void addReq(const int idx, const AosRundataPtr &rdata);

	bool	checkDoc(const AosRundataPtr &rdata);
};
#endif
