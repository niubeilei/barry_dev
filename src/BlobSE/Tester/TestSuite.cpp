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
// 2015-1-8 Created by White
////////////////////////////////////////////////////////////////////////////
#include "BlobSE/Tester/TestSuite.h"
#include "BlobSE/Tester/BlobSETester.h"
#include "DfmUtil/DfmDoc.h"
#include "Rundata/Rundata.h"
#include "Tester/TestSuite.h"
#include "Tester/TestMgr.h"
#include "Util/OmnNew.h"
#include "Util/File.h"


OmnTestSuitePtr		
AosBlobSETestSuite::getSuite()
{
	OmnTestSuitePtr suite = OmnNew OmnTestSuite("BlobSETestSuite", "BlobSE Test Suite");

	// Now add all the testers
	suite->addTestPkg(OmnNew AosBlobSETester());

	return suite;
}
