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
// 2015/02/06 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "SEInterfaces/StatisticsObj.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "Thread/Mutex.h"
#include "Rundata/Rundata.h"


static OmnMutex sgLock;
static AosStatisticsObj* sgObject = 0;



static bool CreateStatisticsObject(AosRundata *rdata)
{
	sgLock.lock();
	if (sgObject)
	{
		sgLock.unlock();
		return true;
	}

	AosJimoPtr jimo = AosCreateJimoByClassname(rdata, "AosStatistics", 1);
	if (!jimo)
	{
		sgLock.unlock();
		AosLogError(rdata, false, "internal_error") << enderr;
		return false;
	}

	if (jimo->getJimoType() != AosJimoType::eStatistics)
	{
		sgLock.unlock();
		AosLogError(rdata, false, "internal_error") << enderr;
		return false;
	}

	sgObject = dynamic_cast<AosStatisticsObj *>(jimo.getPtr());
	if (!sgObject)
	{
		sgLock.unlock();
		AosLogError(rdata, false, "internal_error") << enderr;
		return false;
	}

	return true;
}


AosStatisticsObj::AosStatisticsObj(const int version)
:
AosJimo(AosJimoType::eStatistics, version)
{
}


AosStatisticsObj::~AosStatisticsObj()
{
}


AosStatisticsObjPtr
AosStatisticsObj::createStatisticsStatic(
		AosRundata *rdata, 
		const AosXmlTagPtr &def)
{
	if (!sgObject) CreateStatisticsObject(rdata);
	aos_assert_rr(sgObject, rdata, 0);
	return sgObject->createStatistics(rdata, def);
}

