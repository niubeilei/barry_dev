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
#include "Dataset/Testers/TestSuite.h"

#include "Dataset/Testers/DatasetTester.h"
#include "Rundata/Rundata.h"
#include "Tester/TestSuite.h"
#include "Tester/TestMgr.h"
#include "Util/OmnNew.h"
#include "Util/File.h"
#include "Util/AppendFile.h"
#include "XmlUtil/XmlTag.h"


OmnTestSuitePtr		
AosDatasetTestSuite::getSuite()
{
	OmnTestSuitePtr suite = OmnNew OmnTestSuite("Dataset", "Util Library Test Suite");

	//if (strcmp(type.data(), "filescanner") == 0)
	//{
		suite->addTestPkg(OmnNew AosDatasetTester());
	//}


	return suite;
}

