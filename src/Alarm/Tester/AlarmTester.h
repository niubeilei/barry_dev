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
// 05/14/2011	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_Alarm_Tester_AlarmTester_h
#define Aos_Alarm_Tester_AlarmTester_h

#include "Tester/TestPkg.h"
#include "Alarm/Ptrs.h"
#include "Alarm/Alarm.h"


class AosAlarmTester : public OmnTestPkg
{
private:
	enum
	{
		eNormalTries = 101
	};
public:
	AosAlarmTester();
	~AosAlarmTester() {}

	virtual bool	start();

private:
	bool basicTest();
};


#endif

