////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: UtilTestSuite.cpp
// Description:
//	This class defines the Util Library Test Suite. A Test Suite
//  consists of a number of Testers. This class will create
//  an instance of all the Util testers.    
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#include "Util/Tester/UtilTestSuite.h"

#include "Rundata/Rundata.h"
#include "Tester/TestSuite.h"
#include "Tester/TestMgr.h"
#include "Util/OmnNew.h"
#include "Util/Tester/StrSplitTester.h"
#include "Util/Tester/StringTester.h"
#include "Util/Tester/SmartIndexValListTester.h"
#include "Util/Tester/FileTester.h"
#include "Util/Tester/BuffArrayVarTester.h"
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
#include "Util/Tester/SPtrTester.h"
#include "Util/Tester/StrHashTester.h"
#include "Util/Tester/StrHashFixedTester.h"
#include "Util/Tester/Base64Tester.h"
#include "Util/Tester/UtUtilTester.h"
#include "Util/Tester/XmlDocCacheTester.h"
#include "Util/Tester/BinarySearchTester.h"
#include "Util/Tester/HashMapTester.h"
#include "Util/Tester/OmnNewTester.h"
#include "Util/Tester/BuffArrayTester.h"
#include "Util/Tester/BuffArrayTesterNew.h"
#include "Util/Tester/BuffArrayVarTester.h"
#include "Util/Tester/VarUnintTester.h"
#include "Util/Tester/VarIntTester.h"
#include "Util/Tester/VectorTester.h"


OmnTestSuitePtr		
OmnUtilTestSuite::getSuite()
{
	OmnTestSuitePtr suite = OmnNew OmnTestSuite("UtilTestSuite", "Util Library Test Suite");

	// 
	// Now add all the testers
	//
//	suite->addTestPkg(OmnNew OmnGenTableTester());
//	suite->addTestPkg(OmnNew OmnStrParserTester());
//	suite->addTestPkg(OmnNew OmnIpAddrTester());
//	suite->addTestPkg(OmnNew OmnRandomTester());
//	suite->addTestPkg(OmnNew OmnDynArrayTester());
//	suite->addTestPkg(OmnNew OmnStringTester());
//	suite->addTestPkg(OmnNew OmnSIVTester());
//	suite->addTestPkg(OmnNew OmnFileTester());
//	suite->addTestPkg(OmnNew OmnFileDescTester());
//	suite->addTestPkg(OmnNew OmnDirDescTester());
//	suite->addTestPkg(OmnNew OmnSrchStrTester());
//	suite->addTestPkg(OmnNew OmnSmtPListTester());
//	suite->addTestPkg(OmnNew OmnPtrHashTester());
//	suite->addTestPkg(OmnNew OmnRCObjectTester());
//	suite->addTestPkg(OmnNew OmnArray4Tester());
//	suite->addTestPkg(OmnNew OmnTArrayTester());
//	suite->addTestPkg(OmnNew OmnTSArrayTester());
//	suite->addTestPkg(OmnNew OmnCharPTreeTester());
//	suite->addTestPkg(OmnNew OmnFastLookupTester());
//	suite->addTestPkg(OmnNew OmnSlabTester());
//	suite->addTestPkg(OmnNew AosStrSplitTester());
//	suite->addTestPkg(OmnNew OmnHashStrTester());
//	suite->addTestPkg(OmnNew OmnHashStrValTester());
//	suite->addTestPkg(OmnNew OmnBuffTester());
//	suite->addTestPkg(OmnNew AosQueueTester());
//	suite->addTestPkg(OmnNew AosStrHashTester());
//	suite->addTestPkg(OmnNew AosStrHashFixedTester());
//	suite->addTestPkg(OmnNew AosBase64Tester());
//	suite->addTestPkg(OmnNew AosUtUtilTester());
//	suite->addTestPkg(OmnNew AosXmlDocCacheTester());
//	suite->addTestPkg(OmnNew AosBinarySearchTester());
//	suite->addTestPkg(OmnNew AosHashMapTester());
//	suite->addTestPkg(OmnNew AosSPtrTester());
//	suite->addTestPkg(OmnNew AosCacherTester());
//	suite->addTestPkg(OmnNew AosNewTester());
//	suite->addTestPkg(OmnNew AosBuffArrayTester());
//	suite->addTestPkg(OmnNew AosBuffArrayTesterNew());
	suite->addTestPkg(OmnNew AosBuffArrayVarTester());
//	suite->addTestPkg(OmnNew AosVarUnintTester());
//	suite->addTestPkg(OmnNew AosVarIntTester());
//	suite->addTestPkg(OmnNew AosVectorTester());
	return suite;
}
