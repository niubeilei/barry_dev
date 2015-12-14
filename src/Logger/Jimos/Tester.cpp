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
// Modification History:
// 2014/11/08 Created by Ken Lee
////////////////////////////////////////////////////////////////////////////
#if 0
#include "Logger/Jimos/LoggerBSON.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "BSON/BSON.h"
#include "Thread/Mutex.h"
#include "Util/OmnNew.h"

void test()
{
	AosLoggerObjPtr logger = AosLoggerObj::createLoggerByClassname(rdata, "AosLoggerBSON");
	aos_assert_r(logger, false);

	AosLog(rdata, logger) 
		<< AosFieldEnumName::eField << "xxx"
		<< AosFieldEnumName::eField << "xxx" << endlog;
}

#endif
