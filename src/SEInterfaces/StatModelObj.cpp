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
#include "SEInterfaces/StatModelObj.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "Thread/Mutex.h"
#include "Rundata/Rundata.h"


static OmnMutex sgLock;
static AosStatModelObj* sgObject = 0;



static bool CreateStatModelObject(AosRundata *rdata)
{
	sgLock.lock();
	if (sgObject)
	{
		sgLock.unlock();
		return true;
	}

	AosJimoPtr jimo = AosCreateJimoByClassname(rdata, "AosStatModel", 1);
	if (!jimo)
	{
		sgLock.unlock();
		AosLogError(rdata, false, "internal_error") << enderr;
		return false;
	}

	if (jimo->getJimoType() != AosJimoType::eStatModel)
	{
		sgLock.unlock();
		AosLogError(rdata, false, "internal_error") << enderr;
		return false;
	}

	sgObject = dynamic_cast<AosStatModelObj *>(jimo.getPtr());
	if (!sgObject)
	{
		sgLock.unlock();
		AosLogError(rdata, false, "internal_error") << enderr;
		return false;
	}

	return true;
}


AosStatModelObj::AosStatModelObj(const int version)
:
AosJimo(AosJimoType::eStatModel, version)
{
}


AosStatModelObj::~AosStatModelObj()
{
}


AosStatModelObjPtr
AosStatModelObj::createStatModelStatic(
		AosRundata *rdata, 
		const AosXmlTagPtr &def)
{
	if (!sgObject) CreateStatModelObject(rdata);
	aos_assert_rr(sgObject, rdata, 0);
	return sgObject->createStatModel(rdata, def);
}

