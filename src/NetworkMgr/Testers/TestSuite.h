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
#ifndef Aos_NetworkMgr_Testers_TestSuite_h
#define Aos_NetworkMgr_Testers_TestSuite_h

#include "Rundata/Ptrs.h"
#include "Tester/Ptrs.h"
#include "Util/String.h"
#include "XmlUtil/Ptrs.h"


class AosNetworkMgrTestSuite
{
private:
	OmnTestSuitePtr		mSuite;

public:
	AosNetworkMgrTestSuite() {}
	~AosNetworkMgrTestSuite() {}

	static OmnTestSuitePtr		getSuite(OmnString &type);
};
#endif

