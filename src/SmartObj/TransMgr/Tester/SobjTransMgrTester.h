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
// 02/26/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_SmartObj_TransMgr_Tester_SobjTransMgrTester_h
#define Aos_SmartObj_TransMgr_Tester_SobjTransMgrTester_h

#include "Tester/TestPkg.h"


class SobjTransMgrTester : public OmnTestPkg
{
private:

public:
	SobjTransMgrTester() {mName = "SobjTransMgrTester";}
	~SobjTransMgrTester() {}

	virtual bool		start();

private:
	bool basicTest();
};
#endif

