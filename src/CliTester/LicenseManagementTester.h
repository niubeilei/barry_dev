////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: LicenseManagementTester.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_CliTester_LicenseManagementTester_h
#define Omn_CliTester_LicenseManagementTester_h

#include "Debug/Debug.h"
#include "Tester/TestPkg.h"


class AosLicenseManagementTester : public OmnTestPkg
{
private:

public:
	AosLicenseManagementTester()
	{
		mName = "AosLicenseManagementTester";
	}
	~AosLicenseManagementTester() {}

	virtual bool		start();

private:
	bool	testShowLicense();
	bool	testTurnFeature();
	bool	testVerifyLicense();
	bool	testImportLicense();
	bool	integrateTest();
	bool	capacityTest();
	
};
#endif

