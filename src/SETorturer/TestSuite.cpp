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
// 08/28/2010 	Created by Chen Ding 
////////////////////////////////////////////////////////////////////////////
#include "SETorturer/TestSuite.h"

#include "SETorturer/SeTester.h"
#include "Tester/TestSuite.h"
#include "Tester/TestMgr.h"
#include "Thread/Mutex.h"
#include "Util/OmnNew.h"
#include "Util/File.h"


OmnTestSuitePtr		
AosSETorturerTestSuite::getSuite()
{
	OmnTestSuitePtr suite = OmnNew OmnTestSuite("SETorturerTestSuite", "");

	suite->addTestPkg(OmnNew AosSeTester());

	return suite;
}
