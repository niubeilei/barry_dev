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
//
// Modification History:
// 2013/01/06 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_AppIdGens_Testers_AppIdGenU64Tester_h
#define Aos_AppIdGens_Testers_AppIdGenU64Tester_h

#include "Tester/TestPkg.h"


class AosAppIdGenU64Tester : public OmnTestPkg
{
private:

public:
	AosAppIdGenU64Tester() {mName = "AppIdGenU64";}
	~AosAppIdGenU64Tester() {}

	virtual bool		start();

private:
	bool	basicTest();
};
#endif

