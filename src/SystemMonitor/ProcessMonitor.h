////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
//
// Modification History:
// 	Created: 05/09/2010 by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_SystemMonitor_ProcessMonitor_h
#define AOS_SystemMonitor_ProcessMonitor_h

#include "SingletonClass/SingletonObjId.h"
#include "SingletonClass/SingletonTplt.h"
#include "Thread/ThreadedObj.h"
#include "Util/String.h"
#include "Util1/Timer.h"
#include "Timer/Timer.h"
#include "Timer/TimerObj.h"
#include "Timer/Ptrs.h"


OmnDefineSingletonClass(AosProcessMonitorSingleton,
						AosProcessMonitor,
						AosProcessMonitorSelf,
						OmnSingletonObjId::eProcessMonitor,
						"ProcessMonitor");

class AosProcessMonitor : public OmnTimerObj, public OmnThreadedObj, virtual public OmnRCObject
{
	OmnDefineRCObject;

public:
	enum
	{
		eDefTimeFreq = 1, //5 seconds 2013-01-31 00:00:00, 2013-01-31 00:00:05, 2013-01-31 00:00:10 ...
		eHour	= 2, //2013-01-31 00:00:00, 2013-01-31 01:00:00, 2013-01-31 02:00:00 ...
		eDay	= 3 //2013-01-31 00:00:00, 2013-02-01 00:00:00, 2013-02-02 00:00:00 ...
	};
	 
	OmnString mHostname;

	int 			mHourTime;
	int 			mDayTime;
	bool			mStoped;
	OmnThreadPtr 	mThread;

private:
	int				mTimerid;
	AosProcessMonitor();
	~AosProcessMonitor();
public:
    // Singleton class interface
    static AosProcessMonitor* 	getSelf();
    virtual bool      	start();
    virtual bool        stop();
    virtual bool		config(const AosXmlTagPtr &def);

	virtual bool threadFunc(OmnThrdStatus::E &state, const OmnThreadPtr &thread);
	virtual bool signal(const int threadLogicId) {return true; }
	virtual bool checkThread(OmnString &errmsg, const int tid) const {return true; }
	// TimerObj Interface
	virtual void 	timeout(const int timerid, const OmnString &timerName, void *parm);

private:
	bool monitor(const int type, const OmnString &ctime);
	bool getMonitorType(bool &daytype, bool &munitetype, bool &secondtype, const OmnString &ctime);
	bool addLog(const OmnString &ctime);
};
#endif

