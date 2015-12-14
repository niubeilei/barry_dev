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
// Modification History:
// 06/16/2011	Created by Ken Lee
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_Timer_Tester_TimerTester_h
#define Aos_Timer_Tester_TimerTester_h 

#include "Tester/TestPkg.h"
#include "Tester/TestMgr.h"
#include "Rundata/Rundata.h"
#include "Timer/TimerObj.h"
#include "Util/Opr.h"

#include <map>

class AosTimerTester : public OmnTestPkg,
					   virtual public AosTimerObj
{
	enum
	{
		eCreateTimer,
		eCancelTimer
	};

	int 					mTotal;
	int 					mAdd;
	int 					mCancel;
	map<u64, AosRundataPtr> mTimerMap;

public:
	AosTimerTester();
	~AosTimerTester();

	virtual bool 	start();
	virtual bool 	timeout(const u64 &timerid, const AosRundataPtr &rdata);

private:

	bool 	basicTest();
	bool	createTimer();
	bool	cancelTimer();
};

#endif
