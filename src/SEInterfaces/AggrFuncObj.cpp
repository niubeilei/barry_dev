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
#include "SEInterfaces/AggrFuncObj.h"

#include "SEInterfaces/AggrFuncMgrObj.h"
#include "API/AosApi.h"
#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "Rundata/Rundata.h"
#include "SEInterfaces/JimoType.h"
#include "Thread/Mutex.h"

static OmnMutex sgLock;
static AosAggrFuncMgrObjPtr sgCreator;
static AosAggrFuncMgrObj* sgCreatorRaw = 0;
AosAggrFuncObjPtr AosAggrFuncObj::smObject=0;



AosAggrFuncObj::AosAggrFuncObj(const int version)
:
AosJimo(AosJimoType::eAggrFunc, version)
{
}

AosAggrFuncObj::~AosAggrFuncObj()
{
}


static bool CreateObjCreator(AosRundata *rdata)
{
	sgLock.lock();
	if (sgCreator)
	{
		sgLock.unlock();
		return true;
	}

	AosJimoPtr jimo = AosCreateJimoByClassname(rdata, "AosAggrFuncMgr", 1);
	if (!jimo)
	{
		sgLock.unlock();
		OmnAlarm << "Failed creating AggrFuncMgr" << enderr;
		return false;
	}

	if (jimo->getJimoType() != AosJimoType::eAggrFuncMgr)
	{
		sgLock.unlock();
		OmnAlarm << "Internal error: " << jimo->getJimoType() << enderr;
		return false;
	}

	AosAggrFuncMgrObj *obj = dynamic_cast<AosAggrFuncMgrObj*>(jimo.getPtr());
	if (!obj)
	{
		sgLock.unlock();
		OmnAlarm << "Internal error!" << enderr;
		return false;
	}
	sgCreator = obj;
	sgCreatorRaw = sgCreator.getPtr();
	sgLock.unlock();
	return true;
}


AosAggrFuncObjPtr
AosAggrFuncObj::pickAggrFuncStatic(
		const AosRundataPtr &rdata,
		const OmnString &func_name)
{
	if (!sgCreator) CreateObjCreator(rdata.getPtrNoLock());
	return sgCreator->pickAggrFunc(rdata, func_name);
}


bool
AosAggrFuncObj::isFuncValidStatic(
		const AosRundataPtr &rdata,
		const OmnString &func_name)
{
	if (!sgCreator) CreateObjCreator(rdata.getPtrNoLock());
	return sgCreator->isFuncValid(rdata, func_name);
}


