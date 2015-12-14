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
#ifndef AOS_SEUtil_ProgressMonitor_h
#define AOS_SEUtil_ProgressMonitor_h

#include "Thread/ThrdShellProc.h"
#include "Util/String.h"
#include "Porting/Sleep.h"


class AosProgressMonitor: virtual public OmnThrdShellProc
{
	OmnDefineRCObject;

private:
	bool		mStopped;
	OmnString	mEvent;
	int 		mEventStart;
	int			mTimer;

public:
	AosProgressMonitor();

	virtual bool        run();
	virtual bool        procFinished();

	bool	startEvent(const OmnString &event, const int timer_sec);
	bool 	start();
	void	stop();

};
#endif
