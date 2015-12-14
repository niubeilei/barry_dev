////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: InitialOcspServer.cpp
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#include "CertVerify/Tester/InitialOcspServer.h"

#include "KernelInterface/CliProc.h"
#include "Tester/Test.h"
#include "Tester/TestMgr.h"


AosInitialOcspServer::AosInitialOcspServer(const OmnTestMgrPtr &testMgr)
{
	mTestMgr = testMgr;
	mName = "AosInitOcspServer";
}


bool AosInitialOcspServer::start()
{

	initiate_OcspServer();

	return true;	

}

bool AosInitialOcspServer::initiate_OcspServer()
{
	aosRunCli("clear config",true);
	aosRunCli("ocsp set addr 192.168.1.142 3904",true);
	aosRunCli("ocsp status on",true);

	return true;
}

