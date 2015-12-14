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
// 0i6/15/2011	Created by Linda
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_DocLock_Torturer_DocLockTester_h
#define Aos_DocLock_Torturer_DocLockTester_h

#include "Util/Opr.h"
#include "Tester/TestPkg.h"
#include "Rundata/Rundata.h"
#include <map>

typedef map<u64, map<u64, u64> >				TestReadMap;
typedef map<u64, map<u64, u64> >::iterator		TestReadItr_t;
typedef map<u64, u64>							TestWriteMap;
typedef map<u64, u64>::iterator					TestWriteItr_t;
typedef map<u64, u64>							TestMap;
typedef map<u64, u64>::iterator					TestMapItr_t;
class AosDocLockTester : public OmnTestPkg
{
private:
	enum
	{
		eNormalTries = 100,
		eDocidMax = 20
	};

private:
	//u64						mValidDocid[eDocidMax];	
	//map<u64, map<u64, u64> >		mReadMap; //<docid,<userid, lockid>>
	//map<u64, u64>				mWriteMap; //<docid, lockid>
	TestReadMap						mReadMap;
	TestWriteMap					mWriteMap; 
	OmnMutexPtr						mLock;
public:
	AosDocLockTester();
	~AosDocLockTester() {}

	virtual bool		start();

private:
	bool 	basicTest();
	bool	testOneDocLock();
	u64		pickNewDocid();
	u64		pickDocid();
	u64		pickUserid();
	bool	testReadLock(const AosRundataPtr &rdata);
	bool	testWriteLock(const AosRundataPtr &rdata);
	bool	ReadLock(const AosRundataPtr &rdata);
	bool	ReadUnLock(const AosRundataPtr &rdata);
	bool	WriteLock(const AosRundataPtr &rdata);
	bool	WriteUnLock(const AosRundataPtr &rdata);
	bool	printRead(const AosRundataPtr &rdata);
	bool	printWrite(const AosRundataPtr &rdata);
public:
	bool	timeout(const AosRundataPtr &rdata);
	void	waitThread();
	void 	signalThread();
};


#endif

