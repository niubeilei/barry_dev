////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: TimerPackTester.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Aos_AosUtil_Tester_TimerPackTester_h
#define Aos_AosUtil_Tester_TimerPackTester_h

#include "aosUtil/List.h"
#include "aosUtil/TimerPack.h"
#include "Debug/Debug.h"
#include "Tester/TestPkg.h"


class AosTimerPackTester : public OmnTestPkg
{
private:
	int					mNumTries;
	aos_list_head		mTimerList;
	int 				mfreq_msec;
	AosTimerPackFunc 	mPackFunc;
//	AosTimerPack_t 	* 	mTestPack;

	enum
	{
		eAddOperation=60,
		eDeleteOperation=85,
		eModifyOperation=95
	}; 

	enum
	{
		eTv1Selector=7
	}; 

	enum
	{
		eNoTimersEachTime=99+1
	}; 


public:
	AosTimerPackTester()
	{
		mName = "AosTimerPackTester";
	}
	~AosTimerPackTester() {}

	virtual bool		start();

	//void 	AosTimerPackTestCallback(struct aos_list_head * data_list);

private:
	bool	basicTest();
	bool	tortureTest();
	bool 	doOperations();
	bool	checkResults();
	bool	addTimers();
	bool	deleteTimers();
	bool	modifyTimers();
	bool	createNewPack();
};
#endif

