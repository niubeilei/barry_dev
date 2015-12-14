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
// 2013/05/18 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Omn_DocMgr_Tester_DocMgrTester_h
#define Omn_DocMgr_Tester_DocMgrTester_h

#include "Debug/Debug.h"
#include "DocMgr/Ptrs.h"
#include "Rundata/Ptrs.h"
#include "Tester/TestPkg.h"
#include "Thread/Ptrs.h"
#include "Thread/ThrdShellProc.h"
#include "XmlUtil/Ptrs.h"
#include <vector>
using namespace std;


class AosDocMgrTester : public OmnTestPkg, public OmnThrdShellProc
{
private:
	enum
	{
		eMaxNewDocs = 100
	};

	int							mNumRunners;
	int							mId;
	vector<AosXmlTagPtr>		mNewDocs;
	u32							mMaxNewDocs;
	int							mGetTries;

	static OmnMutexPtr			smLock;
	static AosRundataPtr		smRundata;
	static u32					smNumDocs;
	static vector<AosXmlTagPtr>	smDocs;
	static vector<bool>			smDocStatus;
	static u64					smDocid;
	static AosDocMgrPtr			smDocMgr;

public:
	AosDocMgrTester();
	AosDocMgrTester(const int id);
	~AosDocMgrTester();

	virtual bool		start();

	// ThrdShellProc interface
	virtual bool		run();
	virtual bool		procFinished();


private:
	bool init();
	AosXmlTagPtr pickDoc(int &idx);
	bool functionalTesting();
	bool threadTesting();
	bool testAddDoc();
	bool threadAddDoc();
	bool testDeleteDoc();
	bool threadDeleteDoc();
	bool testGetDoc();
	bool testGetOneDoc();
};
#endif

