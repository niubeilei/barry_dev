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
#include "SEInterfaces/StatMeasureObj.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "Thread/Mutex.h"
#include "Rundata/Rundata.h"


static OmnMutex sgLock;
static AosStatMeasureObj* sgObject = 0;



static bool CreateStatMeasureObject(AosRundata *rdata)
{
	sgLock.lock();
	if (sgObject)
	{
		sgLock.unlock();
		return true;
	}

	AosJimoPtr jimo = AosCreateJimoByClassname(rdata, "AosStatMeasure", 1);
	if (!jimo)
	{
		sgLock.unlock();
		AosLogError(rdata, false, "internal_error") << enderr;
		return false;
	}

	if (jimo->getJimoType() != AosJimoType::eStatMeasure)
	{
		sgLock.unlock();
		AosLogError(rdata, false, "internal_error") << enderr;
		return false;
	}

	sgObject = dynamic_cast<AosStatMeasureObj *>(jimo.getPtr());
	if (!sgObject)
	{
		sgLock.unlock();
		AosLogError(rdata, false, "internal_error") << enderr;
		return false;
	}

	return true;
}


AosStatMeasureObj::AosStatMeasureObj(const int version)
:
AosJimo(AosJimoType::eStatMeasure, version)
{
}


AosStatMeasureObj::~AosStatMeasureObj()
{
}


AosStatMeasureObjPtr
AosStatMeasureObj::createStatMeasureStatic(
		AosRundata *rdata, 
		const AosXmlTagPtr &def)
{
	if (!sgObject) CreateStatMeasureObject(rdata);
	aos_assert_rr(sgObject, rdata, 0);
	return sgObject->createStatMeasure(rdata, def);
}

