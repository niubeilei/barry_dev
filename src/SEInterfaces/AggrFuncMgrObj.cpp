////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
//
// Modification History:
// 2013/11/24 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "SEInterfaces/AggrFuncMgrObj.h"

#include "API/AosApi.h"
#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "Rundata/Rundata.h"
#include "SEInterfaces/JimoType.h"
#include "Thread/Mutex.h"


AosAggrFuncMgrObj::AosAggrFuncMgrObj(const int version)
:
AosJimo(AosJimoType::eAggrFuncMgr, version)
{
}

AosAggrFuncMgrObj::~AosAggrFuncMgrObj()
{
}

