////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: ApplicationProxyTester.cpp
// Description:
//   
//
// Modification History:
// 	11/16/2006	Created by Chen Ding
//
////////////////////////////////////////////////////////////////////////////
#include "TorturerExamples/Tester/SimpleTorturer.h"

#include "Tester/Test.h"
#include "Tester/TestMgr.h"



AosSimpleTorturer::AosSimpleTorturer()
{
}


bool AosSimpleTorturer::start()
{
	OmnBeginTest << "Begin the Simple Torturer Testing";
	mTcNameRoot = "Simple_Torturer";

	OmnTC(OmnExpected<int>(10), OmnActual<int>(11)) << "This is an error" << endtc;

	return true;
}
