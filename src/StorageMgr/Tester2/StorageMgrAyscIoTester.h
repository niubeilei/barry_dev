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
#include "Util/Opr.h"
#include "Tester/TestPkg.h"
#include "Thread/ThreadedObj.h"
#include "SEInterfaces/ActionCaller.h"
#include "DocFileMgr/Ptrs.h"
#include "DocFileMgr/RaidFile.h"
#include <dirent.h>


class AosStorageMgrAyscIoTester : public OmnTestPkg 
{

public:
	enum
	{
		eThreadNum = 20,
		//eMaxFileSize = 100000000000, // 50G
		eMaxFileSize = 100000000000, // 100G
		eMaxReadSize = 1000000000
		//eMaxReadSize = 5000000 
	};


private:
	AosBuffPtr 			mBuff;
	int64_t				mBuffLen;
	u32					mReadId;
	vector<u64>			mFileId;
	vector<int64_t>		mSize;
	AosBuffPtr          mDataBuff;


	int64_t				mCrtLastOffset;
	OmnLocalFilePtr		mFile;
	int					mCrtVidIdx;
	AosDataScannerObjPtr            mScanner;
	int					mIndex;
	u64					mStartDocid;
	int					mNumDocids;
	int					mRecordLen;
	u64 				mCheckNum;

public:
	AosStorageMgrAyscIoTester();
	~AosStorageMgrAyscIoTester();

	virtual bool		start();
	
private:
	bool 	basicTest(const AosRundataPtr &rdata);

	void	createData(const AosRundataPtr &rdata);

	bool	createFileLocked(const AosRundataPtr &rdata);

	bool	readyData(const AosRundataPtr &rdata);

	bool	readData(const AosRundataPtr &rdata);

	bool	readyData();

	bool	checkData(const AosBuffPtr &buff, u64 &check_num);

	bool	checkData(const AosBuffPtr &buff, const int64_t &check_num);

	AosBuffPtr readFile(
			const OmnString &fname,
			int64_t &seekPos,
			const u32 bytes_to_read,
			const AosRundataPtr &rdata);

	void addReq(const AosRundataPtr &rdata);

	bool asyncReadFile(const AosRundataPtr &rdata);
};
#endif
