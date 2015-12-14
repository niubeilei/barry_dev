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
#include "DataScanner/Testers/TestSuite.h"

#include "DataScanner/Testers/FileScannerTester.h"
#include "Rundata/Rundata.h"
#include "Tester/TestSuite.h"
#include "Tester/TestMgr.h"
#include "Util/OmnNew.h"
#include "Util/File.h"
#include "Util/AppendFile.h"
#include "XmlUtil/XmlTag.h"


OmnTestSuitePtr		
AosDataScannerTestSuite::getSuite(OmnString &type)
{
	OmnTestSuitePtr suite = OmnNew OmnTestSuite("DataScanner", "Util Library Test Suite");

	if (strcmp(type.data(), "all") == 0)
	{
	}

	if (strcmp(type.data(), "joiniil") == 0)
	{
	}
	
	if (strcmp(type.data(), "joiniil") == 0)
	{
	}
	
	if (strcmp(type.data(), "importdata") == 0)
	{
	}

	if (strcmp(type.data(), "filescanner") == 0)
	{
		suite->addTestPkg(OmnNew AosFileScannerTester());
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

