////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: Asn1TestSuite.cpp
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#include "Asn1Tester/Asn1TestSuite.h"

#include "Tester/TestSuite.h"
#include "Tester/TestMgr.h"
#include "Asn1Tester/Asn1CodecTester.h"
#include "Asn1Tester/CertificateTester.h"
#include "Util/OmnNew.h"


OmnAsn1TestSuite::OmnAsn1TestSuite()
{
}


OmnAsn1TestSuite::~OmnAsn1TestSuite()
{
}


OmnTestSuitePtr		
OmnAsn1TestSuite::getSuite()
{
	OmnTestSuitePtr suite = OmnNew OmnTestSuite("Asn1TestSuite", "Asn1 Test Suite");
	suite.setDelFlag(false);

	// 
	// Now add all the testers
	//
	suite->addTestPkg(OmnNew OmnAsn1CodecTester());
	suite->addTestPkg(OmnNew OmnCertificateTester());

	return suite;
}
