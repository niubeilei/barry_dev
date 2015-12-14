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
// 09/06/2010	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef AosSengTester_DocLockTester_h
#define AosSengTester_DocLockTester_h

#include "SengTorturer/SengTester.h"
#include "Tester/TestMgr.h"
#include "Thread/Ptrs.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include <map>

typedef map<u64, map<u64, u64> >                TestReadMap;
typedef map<u64, map<u64, u64> >::iterator      TestReadItr_t;
typedef map<u64, u64>                           TestWriteMap;
typedef map<u64, u64>::iterator                 TestWriteItr_t;
typedef map<u64, u64>                           TestMap;
typedef map<u64, u64>::iterator                 TestMapItr_t;

class AosDocLockTester : virtual public AosSengTester
{
private:
	// Weights
	u64 							mLocalDocid;
	u64								mServerDocid;
	AosXmlTagPtr 					mServerDoc;
	OmnString 						mObjid;
	TestReadMap                     mReadMap;
	TestWriteMap                    mWriteMap;
	AosXmlTagPtr 					mRawDoc;
	AosXmlTagPtr					mResp;
	int								mNum;

public:
	AosDocLockTester(const bool regflag);
	AosDocLockTester();
	~AosDocLockTester();

	virtual bool test();
	AosSengTesterPtr clone()
			{
				return OmnNew AosDocLockTester();
			}
private:
	bool	pickDoc();
	bool	ReadLock();
	bool	WriteLock();
	bool	ReadUnLock();
	bool	WriteUnLock();
	bool	sendDocLockReq(const OmnString &req);
	bool	runLock();
	bool 	ReadLockReq();
	bool 	WriteLockReq();
	bool 	ReadUnLockReq(const u64 &lockid);
	bool 	WriteUnLockReq(const u64 &lockid);
	bool 	CheckReadLock();
	bool 	CheckReadUnLock();
	bool 	CheckWriteUnLock();
	bool 	CheckWriteLock();

};
#endif

