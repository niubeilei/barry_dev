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
// 04/12/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Omn_TestUtil_BinarySearchTester_h
#define Omn_TestUtil_BinarySearchTester_h

#include "Tester/TestPkg.h"


class AosBinarySearchTester : public OmnTestPkg
{
private:

public:
	AosBinarySearchTester() {mName = "OmnSIVTester";}
	~AosBinarySearchTester() {}

	virtual bool		start();
	virtual bool		test1();
};


#endif

