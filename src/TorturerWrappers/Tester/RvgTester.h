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
// 04/28/2008 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_TorturerObjs_Tester_RvgWrapperTester_h
#define Aos_TorturerObjs_Tester_RvgWrapperTester_h

#include "Tester/TestPkg.h"


class AosRvgWrapperTester : public OmnTestPkg
{
private:

public:
	AosRvgWrapperTester() {mName = "AosRvgWrapperTester";}
	~AosRvgWrapperTester() {}

	virtual bool		start();

private:
	bool	basicTest();
	bool	addRvg();
	bool	deleteRvg();
	bool	modifyRvg();
	bool 	removeAllRvgs();
	bool	queryRvgs();
};


#endif

