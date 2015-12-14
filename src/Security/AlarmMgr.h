////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: AlarmMgr.h
// Description:
//	This is a singleton class. If a class wants to raise an alarm, it
//  should use this class to raise the alarm.   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_Security_AlarmMgr_h
#define Omn_Security_AlarmMgr_h

#include "Util/Obj.h"


class OmnAlarmMgr : public virtual OmnObj
{
private:

public:
	OmnAlarmMgr();
	~OmnAlarmMgr();

	int			alarm(const OmnString &file, 
					  const int line,
					  const OmnAlarmId::E alarmId,
					  const OmnString &errmsg);

	bool		clearAlarm(const OmnString &file,
					  const int line, 
					  const int alarmId,
					  const OmnString &msg = "");
};
#endif
