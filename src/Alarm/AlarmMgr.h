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
//  This is a singleton class. It provides alarm management for this
//  module. 
//
//  When a function triggers an alarm, it should create an alarm and
//  add the alarm to this class. This class maintains all the alarms
//  it collected. For every alarm it receives, it sends the alarm to 
//  the PNMS Alarm Manager, which provides a system-wide alarm 
//  management.   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_Alarm_AlarmMgr_h
#define Omn_Alarm_AlarmMgr_h

#include "Alarm/Alarm.h"
#include "Alarm/AlarmSender.h"
#include "aosUtil/Types.h"
#include "Util/HashUtil.h"


class OmnAlarmMgr
{
public:
	struct Entry
	{
		int tick;
		int num;

		Entry()
		:
		tick(0),
		num(0)
		{
		}

		Entry(int t, int n)
		:
		tick(t),
		num(n)
		{
		}
	};

	typedef hash_map<const OmnString, int, Omn_Str_hash, compare_str> igmap_t;
	typedef hash_map<const OmnString, int, Omn_Str_hash, compare_str>::iterator igitr_t;
	typedef hash_map<const OmnString, Entry, Omn_Str_hash, compare_str> map_t;
	typedef hash_map<const OmnString, Entry, Omn_Str_hash, compare_str>::iterator mapitr_t;

	struct AlarmInfo
	{
		int64_t		mTotal;				// Statistics: # of total occurance
		int64_t		mSinceLastReset;	// Statistics: # of occurance since last set
		int64_t 	mLastPrintTime;		// Used to control when to write to log
		int64_t 	mLastGetTime;		// Used to control when to write to log

		AlarmInfo()
			:
		mTotal(0),
		mSinceLastReset(0),
		mLastPrintTime(0),
		mLastGetTime(0)
		{
		}
	};

private:
	enum
	{
		eNumAlarms = 5000,		// 2014/10/28, Felicia, Query-460
		eAlarmFreq = 1			// 1 alarm per eAlarmFreq seconds
	};

	static bool					mNeedToSendAlarms;
	static OmnAlarmEntry		mAlarms[eNumAlarms];
	static int					mCrtAlarmIndex;
	static int					mAlarmFreq;
	static OmnAlarmSenderPtr	mAlarmSender;
	static OmnAlarmLoggerPtr	mAlarmLogger;

	static AlarmInfo			mAlarmInfo[OmnErrId::eLastValidEntry+1];
	static bool					mIsAlarmOn;
	static u32					mTotalAlarms;
	static bool					mPauseOnAlarm;		// Chen Ding, 2013/03/29
	static igmap_t				smIgnoredAlarms;
	static map_t				smAlarmTracker;
	static int					smMaxAlarms;

private:
	OmnAlarmMgr();
	~OmnAlarmMgr();

public:
	static bool	isAlarmOn() {return mIsAlarmOn;}
	static bool init();
	static void	setMaxAlarms(const int num);

	static OmnAlarmEntry &	getEntry(const OmnString &file, 
							 const int line,
							 const OmnErrId::E alarmId);
	
	static bool		closeEntry(const OmnAlarmEntry &entry);
	static bool		setAlarmSender(const OmnAlarmSender &sender);
	static OmnAlarmEntry & setAlarm(
						OmnAlarmEntry &alarm, 
						const OmnString &file, 
						const int line, 
						const OmnErrId::E errId);
	static bool config();
	static bool pauseOnAlarm() {return mPauseOnAlarm;}

	// Chen Ding, 2013/05/19
	static bool ignoreAlarm(const char *fname, const int line);
	static bool setIgnoredAlarms(const OmnString &alarms);
	static bool setIgnoredAlarm(const OmnString &fname, const int line);
	static bool removeIgnoredAlarm(const OmnString &fname, const int line);
	static OmnString getIgnoredAlarms();
	static bool pauseOnAlarm(const char *file, const int line);
};

#endif

