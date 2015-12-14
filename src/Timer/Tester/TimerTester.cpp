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
#include "Timer/Tester/TimerTester.h"

#include "Timer/TimerMgr.h"


AosTimerTester::AosTimerTester()
:
mTotal(0),
mAdd(0),
mCancel(0)
{
	mTimerMap.clear();
}

AosTimerTester::~AosTimerTester()
{
}

bool
AosTimerTester::start()
{
	cout << " Timer Test start ..." << endl;
	basicTest();
	return true;
}


bool
AosTimerTester::timeout(const u64 &timerid, const AosRundataPtr &rdata)
{
	map<u64, AosRundataPtr>::iterator itr = mTimerMap.find(timerid);
	if(itr != mTimerMap.end())
	{
		mTimerMap.erase(itr);
//OmnScreen << "timeout callback, timerid:" << timerid << endl;
		return true;
	}
	else
	{
		OmnAlarm << "timerid:" << timerid << enderr;
		return false;
	}
}


bool 
AosTimerTester::basicTest()
{
	bool rslt;
	int tries = 5000000;
	int opr, mOpr;
	for (int i=0; i<tries; i++)
	{
		if(mTotal % 100 == 0)
		{
			sleep(1);
			OmnScreen << "mTotal:" << mTotal << ", mAdd:" << mAdd << ", mCancel:" << mCancel << endl;
		}
		mTotal++;
		
		opr = rand() % 100;
		mOpr = (opr < 70) ? eCreateTimer : eCancelTimer;
		switch (mOpr)
		{
		case eCreateTimer:
			 mAdd++;
			 rslt = createTimer();
			 break;

		case eCancelTimer:
			 mCancel++;
			 rslt = cancelTimer();
			 break;

		default:
			 OmnAlarm << "Unrecognized operation: " << mOpr << enderr;
			 break;
		}
		//AosTC(rslt) << endtc;
	}
	return true;
}


bool
AosTimerTester::createTimer()
{
	u32 second;
	int opr = rand() % 100;
	if(opr < 0)
	{
		second = 0;
	}
	else if (opr <= 50)
	{
		second = rand() % 255 + 1;
	}
	else if (opr <= 80)
	{
		second = rand() % (256 * 255) + 256;
	}
	else
	{
		second = rand() + 256 * 256;
	}

	u64 timerid, sdocid;
	AosRundataPtr rdata = OmnNew AosRundata();
	OmnString udata;
	bool rslt = AosTimerMgr::getSelf()->addTimer(sdocid, second, timerid, udata, this, rdata);
	if(!rslt)
	{
		OmnAlarm << "second:" << second << ", timerid:" << timerid << enderr;
		return false;
	}
	
	mTimerMap.insert(make_pair(timerid, rdata));		
	return true;
}


bool
AosTimerTester::cancelTimer()
{
	int size = mTimerMap.size();
	int i = rand() % (size + 1), j = 0;
	map<u64, AosRundataPtr>::iterator itr;
	for(itr = mTimerMap.begin(); itr != mTimerMap.end(); itr++)
	{
		if(i == j) break;
		j++;
	}
	
	if(itr == mTimerMap.end())
	{
		return false;
	}

	u64 timerid = itr->first;
	AosRundataPtr rdata = itr->second;

	u32 endTime = (u32) timerid;
	u32 second = OmnGetSecond();
	if(endTime < second + 2)
	{
		return true;
	}

	bool rslt = AosTimerMgr::getSelf()->cancelTimer(timerid, rdata);
	if(!rslt)
	{
		OmnAlarm << "cancel error, timerid:" << timerid << enderr;
		return false;
	}

	mTimerMap.erase(itr);
	return true;
}


