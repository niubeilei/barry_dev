////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: CrlTester.cpp
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#include "PKCSTester/crlTester.h"


#include "Debug/Debug.h"
#include "KernelInterface/CliProc.h"
#include "Tester/Test.h"
#include "Tester/TestMgr.h"
#include "Util/OmnNew.h"
#include "Util/Random.h"



bool OmnCrlTester::start()
{
	// 
	// Test default constructor
	//
	crlDecodingTest();
	cliTest();
	return true;
}


bool OmnCrlTester::crlDecodingTest()
{
	OmnString rslt;
	bool rt;

	OmnBeginTest << "Test CRL Decoding";
	mTcNameRoot = "CrlTest-";

	OmnString cmd = "show bridge";
	rt = OmnCliProc::getSelf()->runCli(cmd, rslt);
	OmnTC(OmnExpected<bool>(true), OmnActual<bool>(rt)) << cmd << endtc;

	return true;
}


bool OmnCrlTester::cliTest()
{
	return true;
}

