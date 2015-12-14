////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: AlarmMsg.h
// Description:
//	When a device raises an alarm, it should send an alarm message to 
//  a network management tool. This is the message to send alarms.   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_Security_AlarmMsg_h
#define Omn_Security_AlarmMsg_h

#include "Util/Obj.h"


class OmnAlarmMsg : public virtual OmnObj
{
public:
	OmnAlarmMsg(const int alarmId,
		const OmnAlarmId::E alarmType,
		const OmnString &deviceId,
		const OmnString &file,
		const int line,
		const OmnString &errmsg);
	~OmnAlarmMsg();
};
#endif
