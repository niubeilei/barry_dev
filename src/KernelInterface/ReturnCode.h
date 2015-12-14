////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: ReturnCode.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_KernelInterface_ReturnCode_h
#define Omn_KernelInterface_ReturnCode_h

#include "aosUtil/Alarm.h"
#include "aosUtil/Modules.h"


#ifndef aos_cli_alarm
#define aos_cli_alarm(format, x...) aos_raise_alarm(__FILE__, __LINE__, \
	eAosAlarmLevel_Alarm, eAosMD_CLI, eAosAlarm_CliErr, \
	aos_alarm_get_errmsg((format), ##x))
#endif

#endif

