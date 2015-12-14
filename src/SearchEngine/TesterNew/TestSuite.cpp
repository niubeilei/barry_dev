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
#include "SearchEngine/TesterNew/TestSuite.h"

#include "SearchEngine/TesterNew/DocMgrTester.h"
#include "SearchEngine/TesterNew/IILCompStrTester.h"
#include "SearchEngine/TesterNew/IILCompU64Tester.h"
#include "SearchEngine/TesterNew/IILMgrTester.h"
#include "SearchEngine/TesterNew/IILPrefixTester.h"
#include "SearchEngine/TesterNew/IILStrLikeTester.h"
#include "SearchEngine/TesterNew/IILStrPerfTester.h"
#include "SearchEngine/TesterNew/IILStrRETester.h"
#include "SearchEngine/TesterNew/IILStrRETester2.h"
#include "SearchEngine/TesterNew/IILU64RETester2.h"
#include "SearchEngine/TesterNew/IILStrTester.h"
#include "SearchEngine/TesterNew/IILStrUniqueTester.h"
#include "SearchEngine/TesterNew/IILU64RETester.h"
#include "SearchEngine/TesterNew/IILU64Tester.h"

//#include "SearchEngine/TesterNew/IILMgrTester.h"
//#include "SearchEngine/TesterNew/IILStrUniqueTester.h"
//#include "SearchEngine/TesterNew/IILStrPerfTester.h"


#include "IILMgr/IILStr.h"
#include "IILMgr/IILU64.h"
#include "Tester/TestSuite.h"
#include "Tester/TestMgr.h"
#include "Util/OmnNew.h"
#include "Util/File.h"


OmnTestSuitePtr		
AosSearchEngineTestSuite::getSuite(OmnString &type)
{
	OmnTestSuitePtr suite = OmnNew 
		OmnTestSuite("UtilTestSuite", "Util Library Test Suite");

	// 
	// Now add all the testers
	
	//IILStr Prefix
	if (strcmp(type, "strprefixtest") == 0)
	{
		suite->addTestPkg(OmnNew AosIILStrPerfTester());
	}
	
	//IILStrTester
	if (strcmp(type, "strtest") == 0)
	{
		suite->addTestPkg(OmnNew AosIILStrTester());
	}
	
	//IILU64Tester
	if (strcmp(type, "u64test") == 0)
	{
		 suite->addTestPkg(OmnNew AosIILU64Tester());
	}

	//IILStr Remove Entry
	if (strcmp(type, "strremoveiiltest") == 0)
	{
		 suite->addTestPkg(OmnNew AosIILStrRETester());
	}

	//IILStr Remove Entry 2
	if (strcmp(type, "strremoveiiltest2") == 0)
	{
		 suite->addTestPkg(OmnNew AosIILStrRETester2());
	}

	//IILU64 Remove Entry
	if (strcmp(type, "u64removeiiltest") == 0)
	{
		 suite->addTestPkg(OmnNew AosIILU64RETester());
	}

	//IILU64 Remove Entry 2
	if (strcmp(type, "u64removeiiltest2") == 0)
	{
		 suite->addTestPkg(OmnNew AosIILU64RETester2());
	}

	if(strcmp(type, "strlikeiiltest") == 0)
	{
 		suite->addTestPkg(OmnNew AosIILStrLikeTester());
	}
	
	if(strcmp(type, "struniquetest") == 0)
	{	
		suite->addTestPkg(OmnNew AosIILStrUniqueTester());
	}
	
	if(strcmp(type, "compstrtest") == 0)
	{	
		suite->addTestPkg(OmnNew AosIILCompStrTester());
	}
	
	if(strcmp(type, "compu64test") == 0)
	{	
		suite->addTestPkg(OmnNew AosIILCompU64Tester());
	}
	
	if(strcmp(type, "compu64test") == 0)
	{	
//		suite->addTestPkg(OmnNew AosIILCompU64Tester());
	}
	
	if(strcmp(type, "help") == 0 ||strcmp(type, "--help") == 0 )
	{
		cout << "param can be :\n"
			 << "strprefixtest    \n"
			 << "strtest          \n"
			 << "u64test          \n"
			 << "strremoveiiltest \n"
			 << "u64removeiiltest \n"
			 << "strlikeiiltest   \n"
			 << "struniquetest    \n"
			 << "compstrtest      \n"
			 << "compu64test      \n";
		exit(0);
	}
	return suite;


	// suite->addTestPkg(OmnNew AosIILMgrTester());
	// suite->addTestPkg(OmnNew AosDocMgrTester());
	// suite->addTestPkg(OmnNew AosIILPrefixTester());
	// suite->addTestPkg(OmnNew AosIILStrRETester());
	// suite->addTestPkg(OmnNew AosIILStrTester());
	// suite->addTestPkg(OmnNew AosIILU64Tester());


}
