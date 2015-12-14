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
#include "Actions/Testers/TestSuite.h"

#include "Actions/Ptrs.h"
// #include "Actions/Testers/ActFileSplitTester.h"
#include "Actions/Testers/ActAddBlockTester.h"
#include "Actions/Testers/ActGroupTester.h"
#include "Actions/Testers/ActJoinIILTester.h"
#include "Actions/Testers/ActImportTester.h"
#include "Actions/Testers/ActSumUniqTester.h"

// #include "Actions/Testers/ActCommandTarTester.h"

#include "Rundata/Rundata.h"
#include "Tester/TestSuite.h"
#include "Tester/TestMgr.h"
#include "Util/OmnNew.h"
#include "Util/File.h"
#include "Util/AppendFile.h"
#include "XmlUtil/XmlTag.h"


OmnTestSuitePtr		
AosActionsTestSuite::getSuite(OmnString &type)
{
	OmnTestSuitePtr suite = OmnNew OmnTestSuite("Actions", "File split by xulei ");

//	if (strcmp(type.data(), "all") == 0)
//	{
		//suite->addTestPkg(OmnNew AosActJoinIILTester());
		// suite->addTestPkg(OmnNew AosActUnicomTester());
		// suite->addTestPkg(OmnNew ActCommandTarTester());
//	}

	if (strcmp(type.data(), "addblock") == 0)
	{
		 suite->addTestPkg(OmnNew AosActAddBlockTester());
		 return suite;
	}
	
	if (strcmp(type.data(), "joiniil") == 0)
	{
		suite->addTestPkg(OmnNew AosActJoinIILTester());
	}
	
	if (strcmp(type.data(), "importdata") == 0)
	{
		suite->addTestPkg(OmnNew AosActImportTester());
	}

	if (strcmp(type.data(), "sumuniq") == 0)
	{
		suite->addTestPkg(OmnNew AosActSumUniqTester());
	}

	if(strcmp(type.data(), "help") == 0 ||strcmp(type.data(), "--help") == 0 )
	{
		cout << "Testers can be :\n"
			 << "    all\n"
			 << "    joiniil\n";
		exit(0);
	}
	return suite;
}

