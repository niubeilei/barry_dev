////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
//
// Modification History:
// 	Created: 12/15/2008 by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "DataStore/Testers/TestSuite.h"

#include "DataStore/Testers/DataStoreTester.h"
#include "DataStore/Testers/GenericDbObjTester.h"
#include "Tester/TestSuite.h"
#include "Tester/TestMgr.h"
#include "Util/OmnNew.h"


OmnTestSuitePtr		
AosDataStoreTestSuite::getSuite()
{
	OmnTestSuitePtr suite = OmnNew OmnTestSuite("DataStoreTestSuite", "");

	// 
	// Now add all the testers
	//
	suite->addTestPkg(OmnNew AosDataStoreTester());
	// suite->addTestPkg(OmnNew AosGenericDbObjTester());

	return suite;
}
