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
#ifndef Omn_UtilComm_ReturnCode_h
#define Omn_UtilComm_ReturnCode_h

#include "aos/aosReturnCode.h"

enum
{
	eAosRc_TooManyClients = eAosRc_UtilCommStart,
};

#ifndef aos_utilcomm_alarm
#define aos_utilcomm_alarm(format, x...) \
	aos_raise_alarm(__FILE__, __LINE__, eAosAlarmLevel_Alarm,  \
		eAosMD_UtilComm, eAosAlarm_UtilCommErr, \
		aos_alarm_get_errmsg(format, ##x))
#endif


#endif

