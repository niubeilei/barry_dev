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
// 2015/04/11 Created by Chen Ding 
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_SysIDOs_Testers_IDOTester_h
#define Aos_SysIDOs_Testers_IDOTester_h

#include "Tester/TestPkg.h"


class AosIDOTester : public OmnTestPkg
{
private:

public:
	AosIDOTester() {mName = "IDOTester";}
	~AosIDOTester() {}

	virtual bool		start();

private:
};


#endif

