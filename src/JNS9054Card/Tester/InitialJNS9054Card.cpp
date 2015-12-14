////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: InitialJNS9054Card.cpp
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#include "JNS9054Card/Tester/InitialJNS9054Card.h"

#include "KernelInterface/CliProc.h"
#include "Tester/Test.h"
#include "Tester/TestMgr.h"


AosInitialJNS9054Card::AosInitialJNS9054Card(const OmnTestMgrPtr &testMgr)
{
	mTestMgr = testMgr;
	mName = "AosInitialJNS9054Card";
}


bool AosInitialJNS9054Card::start()
{

	initiate_JNS9054Card();

	return true;	

}

bool AosInitialJNS9054Card::initiate_JNS9054Card()
{
	aosRunCli("clear config",true);

	return true;
}

