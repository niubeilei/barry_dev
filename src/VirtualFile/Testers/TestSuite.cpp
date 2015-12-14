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
#include "VirtualFile/Testers/TestSuite.h"

#include "Rundata/Rundata.h"
#include "Tester/TestSuite.h"
#include "Tester/TestMgr.h"
#include "Util/OmnNew.h"
#include "Util/File.h"
#include "Util/AppendFile.h"
#include "VirtualFile/Testers/VirtualFileTester.h"
#include "XmlUtil/XmlTag.h"


OmnTestSuitePtr		
AosVirtualFileTestSuite::getSuite(OmnString &type)
{
	OmnTestSuitePtr suite = OmnNew OmnTestSuite("Actions", "File split by xulei ");

//	if (strcmp(type.data(), "all") == 0)
//	{
		//suite->addTestPkg(OmnNew AosActJoinIILTester());
		// suite->addTestPkg(OmnNew AosActUnicomTester());
		suite->addTestPkg(OmnNew AosVirtualFileTester());
//	}

	if(strcmp(type.data(), "help") == 0 ||strcmp(type.data(), "--help") == 0 )
	{
		cout << "Testers can be :\n"
			 << "    all\n"
			 << "    virtual_file\n";
		exit(0);
	}
	return suite;
}

