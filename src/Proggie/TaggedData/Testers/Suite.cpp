////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: UtilTestSuite.cpp
// Description:
//	This class defines the Util Library Test Suite. A Test Suite
//  consists of a number of Testers. This class will create
//  an instance of all the Util testers.    
//
// Modification History:
// 08/01/2009:	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "Proggie/TaggedData/Testers/Suite.h"

#include "Tester/TestSuite.h"
#include "Tester/TestMgr.h"
#include "Util/OmnNew.h"
#include "Proggie/TaggedData/Testers/TaggedData.h"


OmnTestSuitePtr		
AosTaggedDataTestSuite::getSuite()
{
	OmnTestSuitePtr suite = OmnNew OmnTestSuite("TaggedData", 
			"Tagged Data Test Suite");

	// 
	// Now add all the testers
	//
	suite->addTestPkg(OmnNew AosTaggedDataTester());
	return suite;
}
