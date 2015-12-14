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
// 06/21/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "NetworkMgr/Testers/TestSuite.h"

#include "NetworkMgr/Ptrs.h"
#include "NetworkMgr/Testers/NetworkMgrTester.h"
#include "Rundata/Rundata.h"
#include "Tester/TestSuite.h"
#include "Tester/TestMgr.h"
#include "Util/OmnNew.h"
#include "Util/File.h"
#include "Util/AppendFile.h"
#include "XmlUtil/XmlTag.h"


OmnTestSuitePtr		
AosNetworkMgrTestSuite::getSuite(OmnString &type)
{
	OmnTestSuitePtr suite = OmnNew OmnTestSuite("NetworkMgr", "Test Suite");

	// if (strcmp(type.data(), "all") == 0)
	// {
		suite->addTestPkg(OmnNew AosNetworkMgrTester());
	// }

	// if (strcmp(type.data(), "joiniil") == 0)
	// {
	// 	suite->addTestPkg(OmnNew AosActJoinIILTester());
	// }
	
	if(strcmp(type.data(), "help") == 0 ||strcmp(type.data(), "--help") == 0 )
	{
		cout << "Testers can be :\n"
			 << "    all\n"
			 << "    xxx\n";
		exit(0);
	}
	return suite;
}

