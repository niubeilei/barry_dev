////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: Alarm.h
// Description:
//   
//
// Modification History:
// 12/02/2007	Moved from aosUtil/Alarm.h by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Omn_alarm_AlarmUtil_h
#define Omn_alarm_AlarmUtil_h

#ifdef __cplusplus
extern "C" {
#endif

extern int aos_alarm_init(int flag);
extern char *aos_alarm_get_errmsg(const char *fmt, ...);
extern int aos_raise_alarm(const char *file, 
						  const int line, 
						  int level, 
						  int module, 
						  int id,
						  const char *msg);

extern void raiseAlarmFromAssert(
		const char *file,
		const int line,
		int level,
		int module,
		int id,
		const char *msg);

extern void AosAlarmBreakPoint(int pause_on_alarm);
#ifdef __cplusplus
}
#endif

enum 
{
	eAosAlarmNameMaxLen = 20,
	eAosAlarmNumLocalBuf = 10,
	eAosAlarmLocalBufSize = 1010
};

#endif

