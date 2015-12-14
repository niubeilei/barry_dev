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
#include "DataScanner/Testers1/TestSuite.h"

#include "DataScanner/Testers1/DocScannerTester.h"
#include "Rundata/Rundata.h"
#include "Tester/TestSuite.h"
#include "Tester/TestMgr.h"
#include "Util/OmnNew.h"
#include "Util/File.h"
#include "Util/AppendFile.h"
#include "XmlUtil/XmlTag.h"


OmnTestSuitePtr		
AosDataScannerTestSuite::getSuite(const AosXmlTagPtr  &testers)
{
	OmnTestSuitePtr suite = OmnNew OmnTestSuite("DataScanner", "Util Library Test Suite");

	if (!testers)
	{
		// Get all the testers
		suite->addTestPkg(OmnNew AosDocScannerTester());
		return suite;
	}

	AosXmlTagPtr tester = testers->getFirstChild();
	while (tester)
	{
		OmnString name = tester->getNodeText();
		cout<< "my start-------------" << name.data() <<endl;
		if (name == "docscanner")
		{
			suite->addTestPkg(OmnNew AosDocScannerTester());
		}
		tester = testers->getNextChild();
	}

	return suite;
}

