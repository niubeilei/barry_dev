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
#ifndef Aos_AppProc_ReturnCode_h
#define Aos_AppProc_ReturnCode_h

#include "aos/aosReturnCode.h"
#include "aosUtil/Alarm.h"

enum eAosAppProcReturnCode
{
	eAosRc_AppProcReinit = eAosRc_AppProcStart+1,
	eAosRc_AppProcMsgError,
	eAosRc_AppProcPoolMem,
	eAosRc_AppProcReqError,
	eAosRc_AutoLoginNotConfig,
	eAosRc_AutoLoginNotTurnedOn,
	eAosRc_FailedToInsert,
	eAosRc_InvalidFlag,
	eAosRc_InvalidPasswd,
	eAosRc_InvalidUsername,
	eAosRc_MissingAddr,
	eAosRc_MissingParms,
	eAosRc_NotLoginUrl,
	eAosRc_ReqDesFailed,
	eAosRc_ReqDesSuccess,
	eAosRc_ServiceNotFound,
	eAosRc_UrlTooLong,
};


enum eAosAppProcAlarmCode
{
	eAosAlarm_AppProcPoolMem = eAosAlarm_AppProcStart + 1,
	eAosAlarm_AppProcMsgError,
	eAosAlarm_AppProcReqError,
};

#endif

