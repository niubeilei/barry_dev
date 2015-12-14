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
#ifndef Omn_TestUtil_BuffArrayVarTesterNew_h
#define Omn_TestUtil_BuffArrayVarTesterNew_h

#if 0
#include "Debug/Debug.h"
#include "Tester/TestPkg.h"
#include "Util/Buff.h"
#include "Util/BuffArrayVarNew.h"
#include "Util/SortNew.h"
#include "XmlUtil/SeXmlParser.h"



class AosBuffArrayVarTesterNew : public OmnTestPkg
{
private:
	int						mTries;
	int 					mRecordLen;
	int 					mDataLen;
	AosBuffArrayVarNewPtr	mBuffArrayVarNew;
	AosCompareFunPtr    	mCmpFun;

	vector<OmnString> 		mVector;
	AosRundata*				mRundata;

public:
	AosBuffArrayVarTesterNew();
	~AosBuffArrayVarTesterNew() {}
	virtual bool		start();

private:
	bool		basicTest();
	bool		append();
	bool		config();
	bool		createBuffArray();
	bool 		checkAppend();
	bool 		checkInsert();
	bool 		checkmodifyEntries();
};
#endif

#endif
