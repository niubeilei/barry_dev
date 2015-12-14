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
// Modification History:
// 2012/11/13 Created by Ken Lee
////////////////////////////////////////////////////////////////////////////
#include "DataJoin/Tester/TestSuite.h"

#include "DataJoin/Tester/DataJoinEngineTester.h"
#include "Tester/TestSuite.h"
#include "Tester/TestMgr.h"
#include "Util/OmnNew.h"

	
AosDataJoinTestSuite::AosDataJoinTestSuite()
{
}


AosDataJoinTestSuite::~AosDataJoinTestSuite()
{
}

OmnTestSuitePtr		
AosDataJoinTestSuite::getSuite()
{
	OmnTestSuitePtr suite = OmnNew OmnTestSuite("DataJoin", "DataJoin Test Suite");

	suite->addTestPkg(OmnNew AosDataJoinEngineTester());
	return suite;
}

