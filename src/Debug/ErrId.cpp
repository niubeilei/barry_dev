////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: ErrId.cpp
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#include "Debug/ErrId.h"


// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// !!!!!!!!   IMPORTANT   !!!!!!!!!!!!!!!!!!
// This function returns a local pointer. The caller should not
// change the contents, nor delete the pointers. The caller should
// finish the use of the pointer immediately. If it wants to keep
// the pointer, it should use OmnString to make a copy of it.
//
// static char * sgOmnEcgNames[] = 
// {
// 	"NoError",
// 	"Invalid"
// };

const char * const
OmnErrId::toStr(const E code)
{
	switch (code)
	{
	case eFirstValidEntry:
		 return "FirstValidEntry";

	case eNoError:
		 return "NoError";

	case eInvalid: 
		 return "Invalid";

	case eUnknown:
		 return "Unknown";

	case eGeneral:
		 return "General";

	case eAssertFail:
		 return "AssertFail";

	case eException:
		 return "Exception";

	case eAlarm:
		 return "Alarm";

	case eAlarmInvalid:
		 return "AlarmInvalid";

	case eAlarmMutexError:
		 return "AlarmMutexError";

	case eAlarmSingletonError:
		 return "AlarmSingletonError";

	case eAlarmAssertFailed:
		 return "AlarmAssertFail";

	case eAlarmProgramError:
		 return "AlarmProgramError";

	case eAlarmHeartbeatError:
		 return "AlarmHeartbeatError";

	case eAlarmConfigError:
		 return "AlarmConfigError";

	case eAlarmNetworkError:
		 return "AlarmNetworkError";

	case eDbError:
		 return "DbError";

	case eDNSNotConfigured:
		 return "DNSNotConfigured";

	case eFailedToAddToQueue:
		 return "FailedToAddToQueue";

	case eFailedToCreateTimerSocket:
		 return "FailedToCreateTimerSocket";

	case eFailedToSend:
		 return "FailedToSend";

	case eInvalidRecvDn:
		 return "InvalidRecvDn";

	case eObjectNotFound:
		 return "ObjectNotFound";

	case eProgError:
		 return "ProgError";

	case eStreamFailed:
		 return "StreamFailed";

	case eWarning:
		 return "Warning";

	case eWarnConfigError:
		 return "WarnConfigError";

	case eWarnDatabaseError:
		 return "WarnDatabaseError";

	case eWarnProgramError:
		 return "WarnProgramError";

	case eWarnCheckFail:
		 return "WarnCheckFail";

	case eWarnFileError:
		 return "WarnFileError";

	case eWarnCommError:
		 return "WarnCommError";

	case eWarnSyntaxError:
		 return "WarnSyntaxError";

	case eCommError:
		 return "CommError";

	case eSocketReadingTimeout:
		 return "SocketReadingtimeout";

	case eLastValidEntry:
		 return "LastValidEntry";

	case eIOError:
		 return "Io Error";

	default:
		 return "Unrecognized";
	}

	return "ShouldNotHappen";
}

