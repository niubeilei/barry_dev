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
// 07/26/2010	Created by Chen Ding 
////////////////////////////////////////////////////////////////////////////
#include "UtilComm/Tester/TestSuite.h"

#include "SEClient/TcpClientTester/TcpClientTester.h"
#include "Tester/TestSuite.h"
#include "Tester/TestMgr.h"
#include "Thread/Mutex.h"
#include "Util/OmnNew.h"
#include "Util/File.h"


OmnTestSuitePtr		
AosSEUtilTestSuite::getSuite()
{
	OmnTestSuitePtr suite = OmnNew OmnTestSuite("TcpClientTest", "");

	suite->addTestPkg(OmnNew AosTcpClientTester());

	return suite;
}
