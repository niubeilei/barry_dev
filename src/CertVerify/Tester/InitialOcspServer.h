////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: InitialOcspServer.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_CertVerify_Tester_InitialOcspServer_h
#define Omn_CertVerify_Tester_InitialOcspServer_h

#include "Tester/TestPkg.h"

class AosInitialOcspServer : public OmnTestPkg
{
private:

public:
	AosInitialOcspServer(const OmnTestMgrPtr &testMgr);
	~AosInitialOcspServer() {}

	virtual bool start();

private:
	bool initiate_OcspServer();

};

#endif

