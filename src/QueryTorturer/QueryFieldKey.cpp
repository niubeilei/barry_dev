////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// RediKeyibution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// Description:
//
// Modification History:
// 2013/09/08	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "QueryTorturer/QueryFieldKey.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "API/AosApi.h"
#include "QueryTorturer/SmartQueryTester.h"
#include "QueryTorturer/SmartStatisticsTester.h"
#include "QueryTorturer/QueryTesterUtil.h"
#include "Random/RandomUtil.h"
#include "Tester/TestMgr.h"
#include "Util/File.h"


AosQueryFieldKey::AosQueryFieldKey(
		const int field_idx, 
		const bool is_time_field)
:
AosQueryFieldStr(field_idx, is_time_field)
{
	if (!init())
	{
		OmnAlarm << "Failed creating field" << enderr;
		OmnThrowException("internal error");
		return;
	}
}


AosQueryFieldKey::~AosQueryFieldKey()
{
}


