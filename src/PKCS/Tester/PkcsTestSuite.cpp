////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: PkcsTestSuite.cpp
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#include "PKCS/Tester/PkcsTestSuite.h"

#include "Tester/TestSuite.h"
#include "Tester/TestMgr.h"
#include "Tester/TestPkg.h"
#include "Util/OmnNew.h"
#include "PKCS/Tester/Base64Tester.h"
#include "PKCS/Tester/CertTester.h"
#include "PKCS/Tester/CertMgrTester.h"



OmnTestSuitePtr		
AosPkcsTestSuite::getSuite()
{
	OmnTestSuitePtr suite = OmnNew 
		OmnTestSuite("PkcsTestSuite", "SSL Test Suite");

    suite->addTestPkg(OmnNew AosBase64Tester());
    suite->addTestPkg(OmnNew AosCertTester());
    suite->addTestPkg(OmnNew AosCertMgrTester());
	return suite;
}

