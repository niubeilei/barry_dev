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
// 06/14/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#if 0
#include "DataCacher/DataCacher.h"

#include "DataCacher/DataCacherSimple.h"
#include "Rundata/Rundata.h"
#include "Thread/Mutex.h"


static AosDataCacherPtr sgCachers[AosDataCacherType::eMax];
static OmnMutex			sgLock;
static OmnMutex			sgInitLock;
static bool				sgInited = false;

AosDataCacher::AosDataCacher(
		const OmnString &name, 
		const AosDataCacherType::E type, 
		const bool flag)
:
mType(type)
{
	if (flag) registerProc(name, this);
}


AosDataCacher::~AosDataCacher()
{
}


bool
AosDataCacher::init(const AosRundataPtr &rdata)
{
	// static AosDataCacherCompose		lsCompose(true);

	// Chen Ding, 05/31/2012
	sgInitLock.lock();
	AosDataCacherType::check();
	sgInited = true;
	sgInitLock.unlock();
	return true;
}


AosDataCacherPtr 
AosDataCacher::createDataCacherProtected(const AosXmlTagPtr &def, const AosRundataPtr &rdata)
{
	if (!sgInited) init(rdata);

	aos_assert_rr(def, rdata, 0);
	AosDataCacherType::E type = AosDataCacherType::toEnum(def->getAttrStr("proc_id"));
	aos_assert_rr(AosDataCacherType::isValid(type), rdata, 0);
	sgLock.lock();
	AosDataCacherObjPtr cacher = sgCachers[type];
	sgLock.unlock();
	return cahcer;
}


bool
AosDataCacher::registerProc(const OmnString &name, const AosDataCacherPtr &cacher)
{
	sgLock.lock();
	if (!AosDataCacherType::isValid(cacher->mType))
	{
		sgLock.unlock();
		OmnString errmsg = "Incorrect data cacher type: ";
		errmsg << cacher->mType;
		OmnAlarm << errmsg << enderr;
		return false;
	}

	if (sgCachers[cacher->mType])
	{
		sgLock.unlock();
		OmnString errmsg = "Proc already registered: ";
		errmsg << cacher->mType;
		OmnAlarm << errmsg << enderr;
		return false;
	}

	sgCachers[cacher->mType] = cacher;
	sgLock.unlock();
	return true;
}
#endif
