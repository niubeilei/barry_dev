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

#ifndef Aos_aosUtil_ReturnCode_h
#define Aos_aosUtil_ReturnCode_h

#include "aos/aosReturnCode.h"

enum
{
	eAosRc_InvalidBase64Input = eAosRc_AosUtilStart,

	eAosRc_ElemExist,
	eAosRc_FailedAddEntry,
	eAosRc_Fatal,
	eAosRc_KeyLenTooLong,
	eAosRc_MemoryCliErr,
	eAosRc_MemoryCatcherErr,
	eAosRc_NullPtr,
	eAosRc_OutOfMemory,
	eAosRc_ProgramError,
	eAosRc_RuleEvalFalse,
	eAosRc_RunOutOfMemory,
	eAosRc_SemanticOperatorFailed,
	eAosRc_StrUtilErr,
	eAosRc_TooManyElem,
	eAosRc_TypeMismatch,
	eAosRc_VirtualFuncCalled,
};


#ifndef aos_util_alarm
#define aos_util_alarm(format, x...)                                   	\
    aos_raise_alarm(__FILE__, __LINE__,                                 \
    	eAosAlarmLevel_Alarm, eAosMD_AosUtil, eAosAlarm_AosUtilErr, 	\
			aos_alarm_get_errmsg(format, ##x))
#endif


#endif

