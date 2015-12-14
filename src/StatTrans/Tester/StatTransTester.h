////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// Modification History:
// 07/12/2014 Created by Phil
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_StatTrans_Tester_StatTransTester_h
#define Aos_StatTrans_Tester_StatTransTester_h

#include "Tester/TestPkg.h"
#include "Rundata/Rundata.h"

class AosStatTransTester  : public OmnTestPkg
{
	OmnString 		mTransName;
	AosXmlTagPtr 	mTransConf;
	AosRundataPtr	mRundata;

public:
	AosStatTransTester();
	~AosStatTransTester();

	void setConf(AosXmlTagPtr conf) { mTransConf = conf; }
	bool start();
	bool testGetStatDocsTrans(AosXmlTagPtr &conf);
	
};

#endif
