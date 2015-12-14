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
#ifndef Aos_StorageMgr_Tester_StorageMgrTester_h
#define Aos_StorageMgr_Tester_StorageMgrTester_h 

#include "DocFileMgr/Ptrs.h"
#include "StorageMgr/Ptrs.h"
#include "SEInterfaces/Ptrs.h"
#include "XmlUtil/XmlTag.h"
#include "Util/Opr.h"
#include "Tester/TestPkg.h"
#include "Thread/ThreadedObj.h"


class AosStorageMgrTester : public OmnTestPkg,
							virtual public OmnThreadedObj
{

public:
	enum
	{
		eDocFileMgrIdStart = 100,
		eCreateWeight = 40,
		eModifyWeight = 10,
		eDeleteWeight = 5,
		eReadWeight = 45,

		eMaxDocs = 200000,
		eMaxPatternLen = 20,
		eMinPatternLen = 2,
		eMaxRepeat = 20000,

		eRepeatLevel1 = 10,
		eRepeatLevel2 = 100,
		eRepeatLevel3 = 1000,
		eRepeatLevel4 = 10000,
		eRepeatLevel5 = eMaxRepeat,
		
		eLevel1Weight = 20,
		eLevel2Weight = 20,
		eLevel3Weight = 30,
		eLevel4Weight = 20,
		eLevel5Weight = 10,

		eVirtualServerNum = 2,
		eAppNum = 2,
		eTotalDBNum = eVirtualServerNum * eAppNum,
	
		eThreadNum = 20
	};

private:
	//AosStorageAppPtr			mStorageApp;

	OmnMutexPtr	mNumLocks[eTotalDBNum];
	OmnMutexPtr	mEntryLocks[eTotalDBNum][eMaxDocs];
	int 		mNumDocs[eTotalDBNum];
	u32*		mSeqno[eTotalDBNum];
	u32*		mOffset[eTotalDBNum];
	int*		mRepeat[eTotalDBNum];
	int*		mPatternLen[eTotalDBNum];
	char**		mPatterns[eTotalDBNum];

	OmnMutexPtr	mInfoLock;
	u32			mCreatedNum;
	u32			mModifiedNum;
	u32			mDeletedNum;
	u32			mReadNum;
	u32			mTotalCreatedNum;
	u32			mTotalModifiedNum;
	u32			mTotalDeletedNum;
	u32			mTotalReadNum;
public:
	AosStorageMgrTester();
	~AosStorageMgrTester();

	virtual bool		start();
	
	// ThreadedObj Interface
	virtual bool    threadFunc(OmnThrdStatus::E &state, const OmnThreadPtr &thread);
	virtual bool    checkThread(OmnString &err, const int thrdLogicId) const{ return true;};
	virtual bool    signal(const int threadLogicId){ return true; };

private:
	bool 	basicTest(
				const AosRundataPtr &rdata,
				const AosDevLocationPtr &location);
	void	init();

	bool	createDoc(
				const AosDocFileMgrObjPtr &docFileMgr,
				const AosRundataPtr &rdata);
	
	bool	modifyDoc(
				const AosDocFileMgrObjPtr &docFileMgr,
				const AosRundataPtr &rdata);
	
	bool	deleteDoc(
				const AosDocFileMgrObjPtr &docFileMgr,
				const AosRundataPtr &rdata);
	
	bool	readDoc(
				const AosDocFileMgrObjPtr &docFileMgr,
				const AosRundataPtr &rdata);
	
	void	setContents(
				char *data,
				const u32 repeat,
				const u32 pattern_len,
				char *pattern);

	u32		pickRepeat();
};
#endif
