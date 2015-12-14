////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: JNS9054CardTestSuite.cpp
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#include "JNS9054Card/Tester/JNS9054CardTestSuite.h"
#include "JNS9054Card/Tester/JNS9054CardTester.h"

#include "Tester/TestSuite.h"
#include "Tester/TestMgr.h"
#include "Tester/TestPkg.h"
#include "Util/OmnNew.h"

OmnTestSuitePtr		
OmnJNS9054CardTestSuite::getSuite()
{
	OmnTestSuitePtr suite = OmnNew 
	OmnTestSuite("JNS9054CardTestSuite", "JNS 9054 Card Test Suite");
	
	// 
	// Now add all the testers
	//
	suite->addTestPkg(OmnNew AosJNS9054CardTester());

	return suite;
}

