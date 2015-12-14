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
// 2009/10/09	Created by Chen Ding 
////////////////////////////////////////////////////////////////////////////
#include "SEClient/Testers/TestSuite.h"

#include "SEClient/Testers/SearchEngTester.h"
#include "SEClient/Testers/CreateDocThrd.h"
#include "SEClient/Testers/ModifyDocThrd.h"
#include "SEClient/Testers/DeleteDocThrd.h"
#include "SEClient/Testers/QueryThrd.h"
#include "SEClient/Testers/DocThrd.h"
#include "SEClient/Testers/TestXmlDoc.h"
#include "Tester/TestSuite.h"
#include "Tester/TestMgr.h"
#include "Thread/Mutex.h"
#include "Util/OmnNew.h"
#include "Util/File.h"


OmnTestSuitePtr		
AosSEClientTestSuite::getSuite()
{
	OmnTestSuitePtr suite = OmnNew OmnTestSuite("SEClientTestSuite", "");

	suite->addTestPkg(OmnNew AosSearchEngTester());

	return suite;
}
