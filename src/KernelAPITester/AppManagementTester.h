////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: AppManagementTester.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_KernelAPITester_AppManagementTester_h
#define Omn_KernelAPITester_AppManagementTester_h

#include "Tester/TestPkg.h"


class OmnTestMgr;

class OmnAppManagementTester : public OmnTestPkg
{
public:
	OmnAppManagementTester();
	~OmnAppManagementTester();

	virtual bool		start();
};
#endif

