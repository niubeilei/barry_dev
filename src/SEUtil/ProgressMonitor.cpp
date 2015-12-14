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
// 2014/10/29 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "SEUtil/ProgressMonitor.h"

#include "Thread/ThreadPool.h"


AosProgressMonitor::AosProgressMonitor()
:
OmnThrdShellProc("progress_monitor"),
mStopped(false),
mEvent(""),
mEventStart(-1),
mTimer(-1)
{
}


bool        
AosProgressMonitor::run()
{
	while (!mStopped)
	{
		OmnSleep(1);
		int sec = OmnGetSecond();
		if (sec - mEventStart > mTimer)
		{
			OmnAlarm << "Event Timeout: " << mEvent << ". Timer: " << mTimer << enderr;
			return true;
		}
	}
	return true;
}


bool        
AosProgressMonitor::procFinished()
{
	return true;
}


bool
AosProgressMonitor::startEvent(
		const OmnString &event, 
		const int timer_sec)
{
	mEvent = event;
	mEventStart = OmnGetSecond();
	mTimer = timer_sec;
	//cout << "Start Function: " << event << ", Timer: " << timer_sec << endl;
	return true;
}


bool
AosProgressMonitor::start()
{
	OmnThrdShellProcPtr thisptr(this, false);
	return OmnThreadPool::runProcAsync(thisptr);
}


void
AosProgressMonitor::stop()
{
	mStopped = true;
}
