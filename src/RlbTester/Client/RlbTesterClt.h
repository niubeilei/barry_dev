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
// 2013/07/01	Created by Ketty 
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_RlbTester_Client_RlbTesterClt_h
#define Aos_RlbTester_Client_RlbTesterClt_h 

#include "Rundata/Ptrs.h"
#include "RlbTester/Client/Ptrs.h"
#include "Thread/ThreadedObj.h"
#include "Util/LRUCache.h"
#include "Util/Ptrs.h"

#include <map>
using namespace std;
	
class AosRlbTesterClt: public OmnThreadedObj
{
	OmnDefineRCObject;
	
	struct LockGen 
	{
		OmnMutexPtr getData(const u64 docid)
		{
			return OmnNew OmnMutex();
		}

		OmnMutexPtr getData(const u64 docid, const OmnMutexPtr &lock)
		{
			return lock;
		}
	};
	
	static AosLRUCache<u64, OmnMutexPtr> smLockPool;
	static LockGen smLockGen;

public:
	enum
	{
		//eThreadNum = 10,
		eThreadNum = 1,
		eMaxFileSize = 1000000000, //4G
		eMaxFileNum = 1000,

		eCreateWeight = 10,
		eModifyWeight = 40,
		eDeleteWeight = 0,
		eReadWeight = 40,
		
		// when comfig change. this will change.
		//eSvrNum = 2,
		
		eSizeLevel0 = 30,
		eSizeLevel1 = 200,      // 30-200
		eSizeLevel2 = 1000,     // 200-1k
		eSizeLevel3 = 5000,     // 1k-5k
		eSizeLevel4 = 30000,    // 5k-30k
	
		eLevel1Weight = 50,
		eLevel2Weight = 20,
		eLevel3Weight = 10,
		eLevel4Weight = 5,
		eLevelNum = 4,
		eMaxLevel = 100,

	};
	
private:
	OmnMutexPtr	 			mLock;
	//bool					mSvrsStart[eSvrNum];
	map<u32, AosTesterCubeGrpInfoPtr>	mCubeGrpInfo;
	map<u64, OmnFilePtr>	mFiles;
		
	OmnThreadPtr			mBasicThrds[eThreadNum];
	OmnThreadPtr			mMonitorThrd;
	u64						mMaxFileSize;
	u64						mMaxReadSize;
	bool					mShowLog;

	static int 	smMinSizes[eMaxLevel];
	static int 	smMaxSizes[eMaxLevel];
	static int	smWeights[eMaxLevel];
			
public:
	AosRlbTesterClt();
	~AosRlbTesterClt();

	bool		start();
	
	// ThreadedObj Interface
	virtual bool    threadFunc(OmnThrdStatus::E &state, const OmnThreadPtr &thread);
	virtual bool    checkThread(OmnString &err, const int thrdLogicId) const{ return true;};
	virtual bool    signal(const int threadLogicId){ return true; };

private:
	//bool 	startFmtSvr(const int svr_id);
	bool 	monitorThrdFunc(OmnThrdStatus::E &state, const OmnThreadPtr &thread);
	//bool 	killProc();
	//bool 	startProc();
	AosTesterCubeGrpInfoPtr randGetCubeGrpInfo();

	bool 	basicThrdFunc(OmnThrdStatus::E &state, const OmnThreadPtr &thread);
	bool 	basicTest();

	bool 	createFile(const AosRundataPtr &rdata);
	bool 	modifyFile(const AosRundataPtr &rdata);
	bool 	deleteFile(const AosRundataPtr &rdata);
	bool 	readData(const AosRundataPtr &rdata);

	bool 	addFileToLocal(const u64 file_id, const OmnString fname);
	bool 	deleteFileFromLocal(const u64 file_id);

	bool 	randGetFile(OmnFilePtr &file, u64 &file_id);
	bool 	randGetSvrId(const u32 cube_grp, int &svr_id, u32 & proc_id);
	
	AosBuffPtr randData(u32 &pattern, u32 &repeat);
	u64		randGetOffset(const u64 crt_file_len, const u64 data_len);
	
};
#endif
