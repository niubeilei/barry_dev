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
#ifndef Omn_alarm_AlarmIds_h
#define Omn_alarm_AlarmIds_h

enum AosAlarmLevel
{
	eAosAlarmLevel_Start = 0,

	eAosAlarmLevel_Warn,
	eAosAlarmLevel_Alarm,
	eAosAlarmLevel_Fatal,

	eAosAlarmLevel_End
};


enum AosAlarmIDs
{
	eAosAlarmID_Start = 0,

	eAosAlarm_InvalidAlarmId,

	eAosAlarm_CliErr,
	eAosAlarm_FailedToConnect,
	eAosAlarm_FailedToOpenFile,
	eAosAlarm_FailedToReadFile,
	eAosAlarm_FailedToSaveConfig,
	eAosAlarm_FailedToWriteConfig,
	eAosAlarm_General,
	eAosAlarm_MemErr,
	eAosAlarm_NotImplemented,
	eAosAlarm_NullPointer,
	eAosAlarm_OutOfBound,
	eAosAlarm_PermissionDenied,
	eAosAlarm_Platform,
	eAosAlarm_ProgErr,
	eAosAlarm_TestErr,
	eAosAlarm_UtilCommErr,
	eAosAlarm_VeryRare,

	eAosAlarmID_End
};
#endif

