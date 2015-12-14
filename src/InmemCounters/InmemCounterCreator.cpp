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
// 09/23/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "InmemCounters/InmemCounterCreator.h"

#include "InmemCounters/InmemCounterU64.h"
#include "Rundata/Rundata.h"
#include "SEInterfaces/InmemCounterObj.h"


static AosInmemCounterObjPtr sgCachers[AosInmemCounterType::eMax];
static OmnMutex			sgLock;
static OmnMutex			sgInitLock;
static bool				sgInited = false;


bool
AosInmemCounterCreator::init(const AosRundataPtr &rdata)
{
	if (sgInited) return true;

 	static AosInmemCounterObjPtr lsFileCacher = OmnNew AosInmemCounterU64(true);

	sgInitLock.lock();
	AosInmemCounterType::check();
	sgInited = true;
	sgInitLock.unlock();
	return true;
}


AosInmemCounterObjPtr 
AosInmemCounterCreator::createCounter(const AosXmlTagPtr &def, const AosRundataPtr &rdata)
{
	if (!sgInited) init(rdata);

	aos_assert_rr(def, rdata, 0);
	AosInmemCounterType::E type = AosInmemCounterType::toEnum(def->getAttrStr(AOSTAG_TYPE));
	aos_assert_rr(AosInmemCounterType::isValid(type), rdata, 0);
	sgLock.lock();
	AosInmemCounterObjPtr counter = sgCachers[type];
	sgLock.unlock();
	return counter->clone(def, rdata);
}


bool
AosInmemCounterCreator::registerCounter(const OmnString &name, AosInmemCounterObj *counter)
{
	sgLock.lock();
	if (!AosInmemCounterType::isValid(counter->getType()))
	{
		sgLock.unlock();
		OmnString errmsg = "Incorrect counter type: ";
		errmsg << counter->getType();
		OmnAlarm << errmsg << enderr;
		return false;
	}

	if (sgCachers[counter->getType()])
	{
		sgLock.unlock();
		OmnString errmsg = "Cacher already registered: ";
		errmsg << counter->getType();
		OmnAlarm << errmsg << enderr;
		return false;
	}

	AosInmemCounterType::addName(name, counter->getType());
	sgCachers[counter->getType()] = counter;
	sgLock.unlock();
	return true;
}

