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
// 06/21/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_NetworkMgr_Testers_NetworkMgrTester_h
#define Aos_NetworkMgr_Testers_NetworkMgrTester_h

#include "Rundata/Ptrs.h"
#include "Tester/TestPkg.h"
#include "Util/Opr.h"
#include "Util/String.h"


class AosNetworkMgrTester : virtual public OmnTestPkg
{
private:
	AosRundataPtr 	mRundata;

public:
	AosNetworkMgrTester();
	~AosNetworkMgrTester() {};

	virtual bool start();

private:
	bool		basicTest();
};
#endif
