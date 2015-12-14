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
// 05/01/2008 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_MySQL_Tester_DbTester_h
#define Aos_MySQL_Tester_DbTester_h

#include "Tester/TestPkg.h"


class OmnDbTester : public OmnTestPkg
{
private:

public:
	OmnDbTester() {mName = "OmnDbTester";}
	~OmnDbTester() {}

	virtual bool		start();

private:
};


#endif

