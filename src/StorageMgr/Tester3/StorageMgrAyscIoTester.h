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
#ifndef Aos_StorageMgr_Tester_StorageMgrAyscIoTester_h
#define Aos_StorageMgr_Tester_StorageMgrAyscIoTester_h 

#include "DocFileMgr/Ptrs.h"
#include "StorageMgr/Ptrs.h"
#include "SEInterfaces/Ptrs.h"
#include "XmlUtil/XmlTag.h"
#include "Rundata/Rundata.h"
#include "Util/Opr.h"
#include "Util/Ptrs.h"
#include "Tester/TestPkg.h"
#include "Thread/ThreadedObj.h"
#include "Thread/ThreadedObj.h"
#include "Thread/ThrdShellProc.h"
#include "SEInterfaces/ActionCaller.h"
#include "SEInterfaces/FileReadListener.h"
#include "DocFileMgr/Ptrs.h"
#include "DocFileMgr/RaidFile.h"
#include "StorageMgr/Tester2/Ptrs.h"
#include <dirent.h>


class AosStorageMgrAyscIoTester : public OmnTestPkg ,public AosFileReadListener
{

public:
	enum
	{
		eThreadNum = 20,
		//eMaxFileSize = 100000000000, // 100G
		eMaxFileSize = 2000000000, // 2G 
		eMaxReadSize = 1000000000,
		eMaxWriteSize = 100000000 // 100M
	};

	struct ReadData : public OmnThrdShellProc
	{
		OmnDefineRCObject;
		
		AosStorageMgrAyscIoTesterPtr mCaller;

		int				mIdx;
		AosRundataPtr 	mRundata;
		bool 			mCreateData;

		ReadData(
				const AosRundataPtr &rdata, 
				const int idx, 
				const AosStorageMgrAyscIoTesterPtr &caller,
				const bool create = false)
		:
		OmnThrdShellProc("test"),
		mCaller(caller),
		mIdx(idx),
		mRundata(rdata),
		mCreateData(create)
		{
		}

		bool run()
		{
			if (mCreateData)
			{
				mCaller->createData(mIdx, mRundata); 
				return true;
			}
			mCaller->readData(mIdx, mRundata);
			return true;
		}

		bool procFinished() {return true;}
	};


private:
	vector<u64>			mFileId;
	vector<int64_t>		mSize;
	AosBuffPtr          mDataBuff;
	OmnMutexPtr			mLock;


	int					mCrtVidIdx;
	AosRundataPtr		mRundata;
	vector<AosVirtualFileObjPtr> mVfs;

public:
	AosStorageMgrAyscIoTester();
	~AosStorageMgrAyscIoTester();

	virtual bool		start();
	
private:
	bool 	basicTest(const AosRundataPtr &rdata);

	void	createData(const int index, const AosRundataPtr &rdata);

	OmnLocalFilePtr	createFileLocked(
			const AosRundataPtr &rdata, 
			int64_t &crt_last_offset,
			const int &index);

	bool	readData(const int idx, const AosRundataPtr &rdata);

	bool	createVfsObj(const int idx, const AosRundataPtr &rdata);

	AosBuffPtr	readyData(int64_t &check_num);

	bool	checkData(const AosBuffPtr &buff, const int64_t &check_num);

	// AosFileReadListener Interface
	virtual void fileReadCallBack(const u64 &reqId, const int64_t &expected_size, const bool &finished);
};
#endif
