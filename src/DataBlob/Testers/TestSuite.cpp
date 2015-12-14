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
// 06/15/2012 Created by Chen Ding 
////////////////////////////////////////////////////////////////////////////
#include "DataBlob/Testers/TestSuite.h"

#include "DataBlob/Ptrs.h"
#include "DataBlob/Testers/BlobTester.h"
#include "Rundata/Rundata.h"
#include "Tester/TestSuite.h"
#include "Tester/TestMgr.h"
#include "Util/OmnNew.h"
#include "Util/File.h"
#include "Util/AppendFile.h"
#include "XmlUtil/XmlTag.h"


OmnTestSuitePtr		
AosDataBlobTestSuite::getSuite(const OmnString &type)
{
	OmnTestSuitePtr suite = OmnNew OmnTestSuite("DataBlob", "");

	suite->addTestPkg(OmnNew AosBlobTester());

	if(strcmp(type.data(), "help") == 0 ||strcmp(type.data(), "--help") == 0 )
	{
		cout << "Testers can be :\n"
			 << "    all\n"
			 << "    stru64\n";
		exit(0);
	}
	return suite;
}

