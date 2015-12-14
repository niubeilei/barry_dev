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
// 01/17/2008: Copied from aosUtil by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef aos_util_rc_h
#define aos_util_rc_h

#include "aos/aosReturnCode.h"
#include "aos/aosReturnCode.h"

enum
{
	eAosRc_Error = 10,

	eAosRc_Consumed,
	eAosRc_ElemExist,
	eAosRc_FailedAddEntry,
	eAosRc_FailedOpenFile,
	eAosRc_FailedWriteFile,
	eAosRc_Fatal,
	eAosRc_InvalidEntry,
	eAosRc_KeyLenTooLong,
	eAosRc_MemoryCliErr,
	eAosRc_MemoryCatcherErr,
	eAosRc_NotProcessed,
	eAosRc_NullPtr,
	eAosRc_OutOfMemory,
	eAosRc_Processed,
	eAosRc_ProgramError,
	eAosRc_RuleEvalFalse,
	eAosRc_RunOutOfMemory,
	eAosRc_SemanticOperatorFailed,
	eAosRc_StrUtilErr,
	eAosRc_TooManyElem,
	eAosRc_TypeMismatch,
	eAosRc_VirtualFuncCalled,
};


/*
ifndef aos_util_alarm
define aos_util_alarm(format, x...)                                   	\
    aos_raise_alarm(__FILE__, __LINE__,                                 \
    	eAosAlarmLevel_Alarm, eAosMD_AosUtil, eAosAlarm_AosUtilErr, 	\
			aos_alarm_get_errmsg(format, ##x))
endif
*/


#endif

