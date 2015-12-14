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
//   
//
// Modification History:
// 2010/12/28	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "Proggie/ReqDistr/Reqid.h"
#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"


#define AOSREQID_NOTIFYUSER			"notifyuser"
#define AOSREQID_VERIFYUSER 		"verifyusr"


AosReqid::E 
AosReqid::toEnum(const OmnString &name)
{
	const char *data = name.data();

	switch (data[0])
	{
	case 'n':
		 if (name == AOSREQID_NOTIFYUSER) return eNotifyUser;
		 break;

	case 'v':
		 if (name == AOSREQID_VERIFYUSER) return eUserVerification;
		 break;

	default:
		 break;
	}

	OmnAlarm << "Unrecognized reqid: " << name << enderr;
	return eInvalid;
}

