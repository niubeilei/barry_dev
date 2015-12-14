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
// 05/14/2012 Created by Chen Ding 
////////////////////////////////////////////////////////////////////////////
#include "DataStructs/Testers/TestSuite.h"

#include "DataStructs/Testers/Vt2dTester.h"
#include "Rundata/Rundata.h"
#include "Tester/TestSuite.h"
#include "Tester/TestMgr.h"
#include "Util/OmnNew.h"
#include "Util/File.h"
#include "Util/AppendFile.h"
#include "XmlUtil/XmlTag.h"


OmnTestSuitePtr		
AosDataStructsTestSuite::getSuite()
{
	OmnTestSuitePtr suite = OmnNew OmnTestSuite("DataStructs", "Util Library Test Suite");

	suite->addTestPkg(OmnNew AosVt2dTester());
	return suite;
}

