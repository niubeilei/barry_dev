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
// 2013/02/15	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_WordIDMgr_Testers_WordIDMgrTester_h
#define Aos_WordIDMgr_Testers_WordIDMgrTester_h

#include "Random/Ptrs.h"
#include "Rundata/Ptrs.h"
#include "SEInterfaces/Ptrs.h"
#include "SEInterfaces/IILObj.h"
#include "Tester/TestPkg.h"
#include "Thread/Ptrs.h"
#include "WordIDMgr/Ptrs.h"
#include "Util/HashUtil.h"
#include "Util/Opr.h"
#include "Util/Ptrs.h"
#include <vector>
#include <list>
using namespace std;

class AosWordIDMgrTester : public OmnTestPkg
{

private:
	OmnMutexPtr				mLock;
	AosRundataPtr			mRundata;
	u64						mTries;
	vector<OmnString>		mRandStr;
	vector<u64>				mOrigID;
	AosWordIDMgrPtr			mWordMgr;
	vector<OmnString>		mWordArray1;
	vector<u64>				mIDArray1;
public:
	AosWordIDMgrTester();
	~AosWordIDMgrTester();

	bool start();

private:
	bool addWordPfmTest();
	bool getWordIDPfmTest();
	bool getWordPfmTest();
	bool compWordByIDPfmTest();
	bool normalTest();

	bool addWordTest(u64& i);
	bool getWordIDTest();
	bool getWordTest();
	bool compWordByIDTest();
	bool saveLoadTest();
	bool syncTest();


	OmnString random_str();


};

#endif

