////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: AlarmSender.h
// Description:
//	This is an interface used by AlarmMgr to send alarms to outside.
//  If we want to send alarms to outside, it should implement this 
//  interface and register with OmnAlarmMgr.   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_Alarm_AlarmSender_h
#define Omn_Alarm_AlarmSender_h

#include "Alarm/Ptrs.h"
#include "Util/Object.h"

class OmnAlarmSender : virtual public OmnObject
{
public:
	OmnAlarmSender();
	~OmnAlarmSender();

	virtual bool	sendAlarm(const OmnAlarmEntry &alarm);
};

#endif
