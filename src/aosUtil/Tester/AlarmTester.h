////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: AlarmTester.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_AosUtil_Tester_AlarmTester_h
#define Omn_AosUtil_Tester_AlarmTester_h

#include "Tester/TestPkg.h"

struct aosAsnObjId;

class OmnTestMgr;

class AosAlarmTester : public OmnTestPkg
{
public:
	AosAlarmTester();
	~AosAlarmTester();

	virtual bool		start();

private:
	bool	normalTests();
	bool	abnormalTests();
};
#endif

