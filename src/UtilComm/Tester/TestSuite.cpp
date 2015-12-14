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

#include "UtilComm/Tester/UdpCommNewTester.h"
#include "UtilComm/Tester/TcpServerTester.h"
#include "Tester/TestSuite.h"
#include "Tester/TestMgr.h"
#include "Thread/Mutex.h"
#include "Util/OmnNew.h"
#include "Util/File.h"


OmnTestSuitePtr		
AosSEUtilTestSuite::getSuite()
{
	OmnTestSuitePtr suite = OmnNew OmnTestSuite("SEUtilTestSuite", "");

	//AosTcpServerTester* t = OmnNew AosTcpServerTester();
	// suite->addTestPkg(OmnNew AosTcpServerTester());
	suite->addTestPkg(OmnNew AosUdpCommTester());

	return suite;
}
