////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: OcspTestSuite.cpp
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#include "CertVerify/Tester/OcspTestSuite.h"

#include "Tester/TestSuite.h"
#include "Tester/TestMgr.h"
#include "Tester/TestPkg.h"
#include "Util/OmnNew.h"

#include "CertVerify/Tester/OcspTester.h"
#include "CertVerify/Tester/OcspTestDriver.h"

OmnTestSuitePtr		
OmnOcspTestSuite::getSuite()
{
	OmnTestSuitePtr suite = OmnNew 
	OmnTestSuite("OcspTestSuite", "Ocsp Test Suite");

	// 
	// Now add all the testers
	//
	suite->addTestPkg(OmnNew AosOcspTester());

	return suite;
}

