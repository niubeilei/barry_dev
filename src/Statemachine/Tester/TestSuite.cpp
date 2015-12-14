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
// 01/06/2012	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "Util/Tester/StmcTestSuite.h"

#include "Tester/TestSuite.h"
#include "Tester/TestMgr.h"
#include "Util/OmnNew.h"
#include "Util/Tester/StrSplitTester.h"
#include "Util/Tester/StringTester.h"
#include "Util/Tester/SmartIndexValListTester.h"
#include "Util/Tester/FileTester.h"
#include "Util/Tester/FileDescTester.h"
#include "Util/Tester/DirDescTester.h"
#include "Util/Tester/SrchStrTester.h"
#include "Util/Tester/SmtPListTester.h"
#include "Util/Tester/PtrHashTester.h"
#include "Util/Tester/RCObjectTester.h"
#include "Util/Tester/Array4Tester.h"
#include "Util/Tester/TArrayTester.h"
#include "Util/Tester/TSArrayTester.h"
#include "Util/Tester/DynArrayTester.h"
#include "Util/Tester/StrParserTester.h"
#include "Util/Tester/FastLookupTester.h"
#include "Util/Tester/RandomTester.h"
#include "Util/Tester/IpAddrTester.h"
#include "Util/Tester/GenTableTester.h"
#include "Util/Tester/HashStrTester.h"
#include "Util/Tester/HashStrValTester.h"
#include "Util/Tester/BuffTester.h"
#include "Util/Tester/QueueTester.h"
#include "Util/Tester/StrHashTester.h"
#include "Util/Tester/StrHashFixedTester.h"
#include "Util/Tester/Base64Tester.h"
#include "Util/Tester/UtUtilTester.h"
#include "Util/Tester/XmlDocCacheTester.h"


OmnTestSuitePtr		
AosStmcTestSuite::getSuite()
{
	OmnTestSuitePtr suite = OmnNew OmnTestSuite("StmcTestSuite", "");

	// suite->addTestPkg(OmnNew AosXmlDocCacheTester());

	return suite;
}
