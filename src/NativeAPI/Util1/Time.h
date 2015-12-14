////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: Time.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_Util1_Time_h
#define Omn_Util1_Time_h

#include "aosUtil/Types.h"
#include "Porting/LongTypes.h"
#include "Porting/TimeOfDay.h"
#include "Porting/GetTime.h"
#include "Thread/Ptrs.h"
#include "Util/Locale.h"
#include "Util1/TimerObj.h"
#include "Util1/Ptrs.h"

#ifndef OmnTimestampStart
#define OmnTimestampStart timeval OmnTimestampSnapshot; OmnGetTimeOfDay(&OmnTimestampSnapshot); 
#endif

#ifndef OmnTimestampEnd
#define OmnTimestampEnd(x) timeval OmnTimestampSnapshotEnd; OmnGetTimeOfDay(&OmnTimestampSnapshotEnd); \
			OmnTrace << x << ": " \
				<< OmnTimestampSnapshotEnd.tv_usec - OmnTimestampSnapshot.tv_usec << endl;
#endif

extern unsigned long volatile jiffies;

static inline u32 aos_get_system_ms()
{
	struct timeval tv;
	gettimeofday(&tv, 0);
	return (u32)tv.tv_sec*1000+tv.tv_usec/1000;
}

class OmnTime
{
private:
	enum
	{
		eSecondsInOneDay = 60*60*24
	};

	OmnThreadPtr	mSecTickThread;
	OmnMutexPtr		mLock;
	OmnCondVarPtr	mCondVar;

	static u64			smStartSec;
	static i64			smCrtSec;
	static u64			smCrtDay;
	static u64			smSecTick;
	static u64			smTimestamp;
	static u64			smAccumulatedMsec;
	static uint			smLastSamplingMsec;
	static OmnString	smTimeStr;

	// 
	// No one should create any objects of this class
	//
	OmnTime();
	~OmnTime();

public:
	static OmnTime *	getSelf();
	static OmnString	crttime();
	static bool			getRealtime(uint &sec, uint &usec);
	static i64			getCrtSec() {return smCrtSec;}
	static u32			getCrtDay() {return smCrtDay;}
	static u32			getCrtSecond();
	static u64			getSecTick() {return smSecTick;}
	static void			setStartSec(const uint s) {smStartSec = s;}
	static u64			getStartSec() {return smStartSec;}
	static OmnString	getTimeStr() {return smTimeStr;}
	static const char *	getTimeCharStr() {return smTimeStr.data();}
	bool				start();
	bool				stop();

	// Chen Ding, 2013/01/27
	// static void setTime(const u64 sec)
	static void updateTime()
	{
		smTimestamp = OmnGetTimestamp();
		smCrtSec = OmnSystemRelativeSec1();
		smCrtDay = smCrtSec / eSecondsInOneDay;
		smSecTick = smCrtSec - smStartSec;
		smTimeStr = OmnGetTime(AosLocale::eUSA);
	}
};

#endif
