////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: AlarmMgr.cpp
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
//	ErrId is used to identify alarms. 
// 
// mAlarmInfo::mLastPrintTime[] is used to record the last time an alarm was
// printed. When printing an alarm, this class will check this array. 
// If the alarm is printed too often, it will not print the alarm. If the
// alarm is printed, it will take the timestamp in this array. 
//
//	Singleton Dependency:
//		first, None, then OmnNetworkSelf   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////
#include "Alarm/AlarmMgr.h"

#include "Alarm/Alarm.h"
#include "Alarm/AlarmProc.h"
#include "API/AosApi.h"
#include "Debug/Debug.h"
#include "Debug/ExitHandler.h"
#include "Porting/Sleep.h"
#include "Porting/GetTime.h"
#include "Porting/Mutex.h"
#include "Thread/Mutex.h"
#include "Util/OmnNew.h"
#include "Util1/Time.h"
#include "XmlUtil/XmlTag.h"
#include "alarm_c/alarm_util.h"
#include "util_c/modules.h"
#include "Porting/Sleep.h"
#include <stdarg.h>
#include "aosUtil/AosPlatform.h"
#include <unistd.h>
#include <sys/types.h>
#include <signal.h>



static u32 sgAlarmId = 101;
static OmnMutexType	sgLock = OmnInitMutex(sgLock);
OmnAlarmMgr::igmap_t OmnAlarmMgr::smIgnoredAlarms;
OmnAlarmMgr::map_t OmnAlarmMgr::smAlarmTracker;

bool OmnAlarmMgr::mIsAlarmOn = false;
bool				OmnAlarmMgr::mNeedToSendAlarms = false;
OmnAlarmEntry		OmnAlarmMgr::mAlarms[eNumAlarms];
int					OmnAlarmMgr::mCrtAlarmIndex = 1;
int					OmnAlarmMgr::mAlarmFreq = OmnAlarmMgr::eAlarmFreq;
u32					OmnAlarmMgr::mTotalAlarms = 0;
bool				OmnAlarmMgr::mPauseOnAlarm = false;		// Chen Ding, 2013/03/29
OmnAlarmSenderPtr	OmnAlarmMgr::mAlarmSender;
OmnAlarmMgr::AlarmInfo	OmnAlarmMgr::mAlarmInfo[OmnErrId::eLastValidEntry+1];
int					OmnAlarmMgr::smMaxAlarms = 10;

static int sgMaxAlarmsBeforeExit = -1;

void
OmnAlarmMgr::setMaxAlarms(const int num)
{
	sgMaxAlarmsBeforeExit = num;
}


// Chen Ding, 2013/03/29
bool
OmnAlarmMgr::config()
{
	AosXmlTagPtr conf = AosGetConfig();
	if (!conf) return true;

	AosXmlTagPtr tag = conf->getFirstChild("alarm_mgr");
	if (!tag) return true;

	mPauseOnAlarm = tag->getAttrBool("pause_on_alarm", false);
	return true;
}


bool
OmnAlarmMgr::init()
{
	for (int i=0; i<OmnErrId::eLastValidEntry; i++)
	{
		mAlarmInfo[i].mTotal = 0;
		mAlarmInfo[i].mSinceLastReset = 0;
		mAlarmInfo[i].mLastPrintTime = -1;
		mAlarmInfo[i].mLastGetTime = -1;
	}

	mIsAlarmOn = true;
	return true;
}


OmnAlarmMgr::~OmnAlarmMgr()
{
}


OmnAlarmEntry &
OmnAlarmMgr::getEntry(const OmnString &file,
					  const int line,
					  const OmnErrId::E errId)
{
	static OmnAlarmEntry lsLocalAlarm;

	OmnMutexLock(sgLock);
	// 2014/10/28, Felicia, Query-460
	// if (OmnTime::getSecTick() - mAlarmInfo[errId].mLastGetTime >= (uint)mAlarmFreq)
	// {
		// mAlarmInfo[errId].mLastGetTime = OmnTime::getSecTick();
		int entryIdx = mCrtAlarmIndex; 
		OmnAlarmEntry &alarm = mAlarms[entryIdx];
		mCrtAlarmIndex++;
		if (mCrtAlarmIndex >= eNumAlarms)
		{
			// Index 0 is reserved
			mCrtAlarmIndex = 1;
		}
		OmnMutexUnlock(sgLock);
		return setAlarm(alarm, file, line, errId);
	// }
	
	// The alarm is raised too many. Do not get the alarm entry
	// OmnMutexUnlock(sgLock);
	// return setAlarm(lsLocalAlarm, file, line, errId);
}


OmnAlarmEntry &
OmnAlarmMgr::setAlarm(
		OmnAlarmEntry &alarm, 
		const OmnString &file, 
		const int line, 
		const OmnErrId::E errId)
{
	// Construct the time string
	char buf[101];
	OmnGetTimeCharStr(buf);

	char buff1[20];
	aos_lltoa(OmnTime::getSecTick(), buff1);
	sprintf(&buf[strlen(buf)], ":%s", buff1);

	alarm.setSeqno(sgAlarmId++);
	alarm.setErrId(errId);
	alarm.setFile(file.data());
	alarm.setLine(line);
	alarm.setTime(buf);
	alarm.setErrMsg("");

	return alarm;
}


bool	
OmnAlarmMgr::closeEntry(const OmnAlarmEntry &theEntry)
{
	// It is the time to close the event entry. If 'entryId' is not correct, 
	// it is a serious program error. Generate a special alarm.
	// Check whether we need to raise the alarm. Some alarms are controlled
	// on the frequency at which the alarms can be raised. 
	//
	OmnErrId::E errId = theEntry.getErrId();
	if (!OmnErrId::isValid(errId))
	{
		// This is a serious problem. Should never happen. We will change 
		// it to OmnErrId::eIncorrect.
		errId = OmnErrId::eIncorrect;
	}

	int tick = OmnTime::getSecTick();
	OmnMutexLock(sgLock);
	OmnString key = theEntry.getFilename();
	key << "_" << theEntry.getLine();
	mapitr_t itr = smAlarmTracker.find(key);
	bool needToRaise = false;
	int num_alarms = 1;
	if (itr == smAlarmTracker.end())
	{
		smAlarmTracker[key] = Entry(tick, 1);
		needToRaise = true;
	}
	else
	{
		if (itr->second.tick != tick)
		{
			itr->second.tick = tick;
			itr->second.num = 0;
		}
		num_alarms = ++(itr->second.num);
		if (num_alarms < smMaxAlarms)
		{
			needToRaise = true;
		}
	}
	OmnMutexUnlock(sgLock);

	if (needToRaise)
	{
		char buff[10000];
		theEntry.toString(num_alarms, buff, 10000);
		//barry 2015/11/11
		//cout << buff << endl;
		OmnScreen << buff << endl;

		// Save the alarm to a file.
		OmnString docstr = theEntry.toXml(num_alarms);
		OmnAlarmProc::getSelf()->saveAlarmToFile(docstr);
	}	

	// if (sgMaxAlarmsBeforeExit > 0 && mTotalAlarms++ > (u32)sgMaxAlarmsBeforeExit) 
	// {
	// 	OmnExitApp("Too many alarms");
	// }
	return false;
}


bool
OmnAlarmMgr::ignoreAlarm(
		const char *fname, 
		const int line)
{
	OmnMutexLock(sgLock);
	OmnString key = fname;
	key << "_" << line;
	igitr_t itr = smIgnoredAlarms.find(key);
	if (itr == smIgnoredAlarms.end())
	{
		OmnMutexUnlock(sgLock);
		return false;
	}

	OmnMutexUnlock(sgLock);
	return true;
}


bool
OmnAlarmMgr::setIgnoredAlarm(
		const OmnString &fname, 
		const int line)
{
	OmnMutexLock(sgLock);
	smIgnoredAlarms[fname] = line;
	OmnMutexUnlock(sgLock);
	return true;
}


bool
OmnAlarmMgr::removeIgnoredAlarm(
		const OmnString &fname, 
		const int line)
{
	OmnMutexLock(sgLock);
	smIgnoredAlarms.erase(fname);
	OmnMutexUnlock(sgLock);
	return true;
}


bool
OmnAlarmMgr::setIgnoredAlarms(const OmnString &ignored_alarms)
{
	// 'alarms' is in the form:
	// 	"fname:line,fname:line,..."
	
	const char *alarms = ignored_alarms.data();
	int len = ignored_alarms.length();
	int idx = 0;
	while (idx < len)
	{
		int name_start = idx;
		while (idx < len && alarms[idx] != ':') idx++;
		
		if (alarms[idx] != ':') break;

		int name_end = idx-1;
		if (name_end <= name_start)
		{
			cout << __FILE__ << ":" << __LINE__ << "************" 
				<< name_start << ":" << name_end;
			return false;
		}

		idx++;
		int line_start = idx;
		while (idx < len && alarms[idx] != ',') idx++;

		int line_end = idx - 1;
		if (line_end <= line_start)
		{
			cout << __FILE__ << ":" << __LINE__ << "************" 
				<< name_start << ":" << name_end;
			return false;
		}

		OmnString key(&alarms[name_start], name_end - name_start+1);
		int mm = atoi(&alarms[line_start]);
		key << "_" << mm;

		OmnMutexLock(sgLock);
		smIgnoredAlarms[key] = mm;
		OmnMutexUnlock(sgLock);

		OmnScreen << "Add ignored alarm: " << key << endl;
		idx++;
	}

	return true;
}


OmnString
OmnAlarmMgr::getIgnoredAlarms()
{
	OmnString results = "<Contents>";

	OmnMutexLock(sgLock);
	igitr_t itr = smIgnoredAlarms.begin();
	for (; itr != smIgnoredAlarms.end(); itr++)
	{
		results << "<entry filename=\"" << itr->first << "\" line=\"" 
			<< itr->second << "\"/>";
	}
	OmnMutexUnlock(sgLock);
	results << "</Contents>";
	return results;
}


bool
OmnAlarmMgr::pauseOnAlarm(
		const char *file, 
		const int line)
{
	if (!mPauseOnAlarm) return true;

	if (ignoreAlarm(file, line)) return true;

	int pause_on_alarm = 1;
	while (pause_on_alarm == 1)
	{
		OmnSleep(5);
	}

	return true;
}


void raiseAlarmFromAssert(
		const char *file,      
		const int line, 
		int level, 
		int module, 
		int id,
		const char *msg)
{
	OmnAlarmEntry entry = OmnAlarmMgr::getEntry(file, line, OmnErrId::eAssertFail);
	OmnString name = OmnAlarmProc::getSelf()->getModuleName();
	unsigned int thrdid = OmnGetCurrentThreadId();
	entry.setThreadId(thrdid);
	entry.setModuleName(name);
	OmnAlarmMgr::closeEntry(entry);
}


// static const char *sgAosAlarmErrHeader[eAosAlarmLevel_End+1] = 
// {
// 	"",
// 	"********** Warn *********",
// 	"********** Alarm *********",
// 	"********** Fatal *********",
// 	""
// };

/*
void AosAlarmBreakPoint(int pause_on_alarm)
{
	// int ppid = getppid();
	// int pid = getpid();
	// printf("ppid: %d, pid: %d\n", ppid, pid);
	getppid();
	getpid();
	
	while (pause_on_alarm) 
	{ 
		printf("%s\n", "alarm sleep!");
		OmnSleep(5);                     
	} 
}
*/


/*
// The caller should never modify the contents of the buffer
// returned by this function.
char *aos_alarm_get_errmsg(const char *fmt, ...)
{
	static int  slBufIndex = 0;
	static char slBuffer[eAosAlarmNumLocalBuf][eAosAlarmLocalBufSize];
	unsigned int index = (slBufIndex++) & 0x07;

	va_list args;

	va_start(args, fmt);
	vsprintf(slBuffer[index], fmt, args);
	va_end(args);

	return slBuffer[index];
}


int aos_raise_alarm(const char *file, 
					const int line, 
					int level, 
					int module, 
					int id,
					const char *msg)
{

	char c = '\0';
	int changed = 0;
	char local[eAosAlarmLocalBufSize+101];

	if (level <= eAosAlarmLevel_Start || level >= eAosAlarmLevel_End)
	{
		level = eAosAlarmLevel_Alarm;
	}

	if (module <= eAosModule_Start || module >= eAosModule_End)
	{
		module = eAosModule_Start;
	}

	if (id <= eAosAlarmID_Start || id >= eAosAlarmID_End)
	{
		id = eAosAlarm_InvalidAlarmId;
	}

	if (strlen(msg) >= eAosAlarmLocalBufSize)
	{
		char *tmp = (char *)msg;
		changed = 1;
		c = tmp[eAosAlarmLocalBufSize];
		tmp[eAosAlarmLocalBufSize] = 0;
	}

	// sprintf(local, 
	// 	"%s"
	// 	"\nLocation: <%s:%d>"
	// 	"\nLevel: %d"
	// 	"\nModule: %s"
	// 	"\nAlarm ID: %d"
	// 	"\nError Message: %s"
	// 	"\n**********************\n", 
	// 	sgAosAlarmErrHeader[level], file, line, level, 
	//		aos_get_module_name(module), id, msg);
	sprintf(local, 
		"%s"
		"\nLocation: <%s:%d>"
		"\nLevel: %d"
		"\nModule: %s"
		"\nAlarm ID: %d"
		"\nError Message: %s"
		"\n**********************\n", 
		sgAosAlarmErrHeader[level], file, line, level, 
			"Module", id, msg);

	if (changed)
	{
		char *tmp = (char *)msg;
		tmp[eAosAlarmLocalBufSize] = c;
	}

	// aos_printk(local);
	printf("%s", local);

	OmnAlarmMgr::pauseOnAlarm(file, line);
	return -id;
}
*/
