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
// 05/14/2011	Created by Chen Ding 
////////////////////////////////////////////////////////////////////////////
#include "DataFormat/Tester/TestSuite.h"

#include "XmlUtil/XmlTag.h"
#include "Tester/TestSuite.h"
#include "Tester/TestMgr.h"
#include "Util/OmnNew.h"
#include "DataFormat/Tester/DataFormatTester.h"


OmnTestSuitePtr		
AosDataFormatTestSuite::getSuite(const AosXmlTagPtr &testers)
{
	OmnTestSuitePtr suite = OmnNew OmnTestSuite("DataFormat", "Test Suite");
	suite->addTestPkg(OmnNew AosDataFormatTester());
	return suite;
}
