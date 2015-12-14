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
// 2014/08/04 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Omn_TestUtil_BuffArrayVarTester_h
#define Omn_TestUtil_BuffArrayVarTester_h

//#if 0
#include "Debug/Debug.h"
#include "Tester/TestPkg.h"
#include "Util/Buff.h"
#include "Util/BuffArrayVar.h"
//#include "Util/Sort.h"
#include "XmlUtil/SeXmlParser.h"



class AosBuffArrayVarTester : public OmnTestPkg
{
private:
	int					mTries;
	int 				mRecordLen;
	int 				mDataLen;
	AosBuffArrayVarPtr 	mBuffArrayVar;
	AosCompareFunPtr    mCmpFun;
	vector<string> 		sgEntries;

public:
	AosBuffArrayVarTester();
	~AosBuffArrayVarTester() {}
	virtual bool		start();

private:
	bool		basicTest();
	bool		testInsert();
	bool		addEntries();
	bool 		addRecordEntry();
	bool		checkEntries();
	bool		checkSort();
	bool		checkmodifyEntries();
	bool		config();
	bool		createBuffArray();
	bool		testcase1();
	bool 		checkInsert();
};
//#endif

#endif
