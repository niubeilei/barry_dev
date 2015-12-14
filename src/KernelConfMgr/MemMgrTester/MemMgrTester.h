////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: MemMgrTester.h
// Description:
//   
//
// Modification History:Mem
// 
////////////////////////////////////////////////////////////////////////////
#ifndef Omn_SystemCli_MemMgrTester_MemMgrTester_h
#define Omn_SystemCli_MemMgrTester_MemMgrTester_h

#include "Debug/Debug.h"
#include "Tester/TestPkg.h"
#include "Util/String.h"
#include "Util1/TimerObj.h"
#include "UtilComm/Ptrs.h"
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>

#define MAX_INT 2147483647


class AosMemMgrTester : public OmnTestPkg,public OmnTimerObj
{
private:
	enum CliId
	{
		eMemMgrSetRecord			=  1,
		eMemMgrRecordStatus         =  2,
		eMemMgrShowMem              =  3,
		eMemMgrShowProcess          =  4,
		eMemMgrBusyThreshold        =  5,
		eMemMgrNormalThreshold      =  6,
		eMemMgrShowThreshold		=  7
	};

public:
	AosMemMgrTester();
	~AosMemMgrTester() {}
	
	static int mRecordInterval;
	static int mRecordTime;
	static int mOnTime;
	static int mOffTime;
	static bool	mStatus;
	static int mBusyTime;
	static int mBusyUsage;
	static int mNormalTime;
	static int mNormalUsage;
	virtual bool		start();
	
private:
	bool	basicTest();
	bool	runMemMgrSetRecord();
	bool	runMemMgrRecordStatus();
	bool	runMemMgrShowMem();
	bool	runMemMgrShowProces();
	bool	runMemMgrShowThreshold();
	bool	runMemMgrBusyThreshold();
	bool	runMemMgrNormalThreshold();
	
	bool	OmnGetTimeStr(OmnString &timeStr, time_t &time);
	bool 	genRecordTime(OmnString &time, int &intTime, bool &isCorrect);
	bool 	genRecordInterval(OmnString &sInterval, int &interval, bool &isCorrect);
	bool	genTime(OmnString &time, bool &isCorrect);
	bool	genStartEndTime(OmnString &startTime, OmnString &endTime, \
							time_t &sTime, time_t &eTime, bool &isCorrect);
	bool	genInterval(time_t sTime, time_t eTime, OmnString &interval,\
						 int &intInterval, bool &isCorrect);
	bool	genUsage(OmnString &usage, int &intUsage, bool &isCorrect);
	void	upgradeTime(int);
	

    virtual void        timeout(const int timerId,
                                const OmnString &timerName,
                                void *parm);

};
	
	extern int sgCount;
#endif

