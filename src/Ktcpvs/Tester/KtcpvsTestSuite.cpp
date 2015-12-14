////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: KtcpvsTestSuite.cpp
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#include "Ktcpvs/Tester/KtcpvsTestSuite.h"

#include "Tester/TestSuite.h"
#include "Tester/TestMgr.h"
#include "Tester/TestPkg.h"
#include "Util/OmnNew.h"

#include "Ktcpvs/Tester/KtcpvsTester.h"


OmnTestSuitePtr		
OmnKtcpvsTestSuite::getSuite()
{
	OmnTestSuitePtr suite = OmnNew 
	OmnTestSuite("KtcpvsTestSuite", "Ktcpvs Library Test Suite");

	// 
	// Now add all the testers
	//
	suite->addTestPkg(OmnNew AosKtcpvsTester());

	return suite;
}

