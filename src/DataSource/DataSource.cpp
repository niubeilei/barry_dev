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
// 07/14/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "DataSource/DataSource.h"

#include "Rundata/Rundata.h"
#include "Thread/Mutex.h"


static AosDataSourcePtr sgStores[AosDataSourceType::eMax];
static OmnMutex			sgLock;
static OmnMutex			sgInitLock;
static bool				sgInited = false;

AosDataSource::AosDataSource(
		const OmnString &name, 
		const AosDataSourceType::E type, 
		const bool flag)
:
mType(type)
{
	if (flag) registerStore(name, this);
}


AosDataSource::~AosDataSource()
{
}


bool
AosDataSource::init(const AosRundataPtr &rdata)
{
	// static AosDataSourceCompose		lsCompose(true);
	sgInitLock.lock();
	AosDataSourceType::check();
	sgInited = true;
	sgInitLock.unlock();
	return true;
}


AosDataSourcePtr 
AosDataSource::createDataSource(const AosXmlTagPtr &def, const AosRundataPtr &rdata)
{
	if (!sgInited) init(rdata);

	aos_assert_rr(def, rdata, 0);
	AosDataSourceType::E type = AosDataSourceType::toEnum(def->getAttrStr(AOSTAG_TYPE));
	aos_assert_rr(AosDataSourceType::isValid(type), rdata, 0);
	sgLock.lock();
	AosDataSourcePtr source = sgStores[type];
	sgLock.unlock();
	aos_assert_rr(source, rdata, 0);
	return source->clone(def, rdata);
}


bool
AosDataSource::registerStore(const OmnString &name, AosDataSource *store)
{
	sgLock.lock();
	if (!AosDataSourceType::isValid(store->mType))
	{
		sgLock.unlock();
		OmnAlarm << "Incorrect type: " << store->mType;
		return false;
	}

	if (sgStores[store->mType])
	{
		sgLock.unlock();
		OmnAlarm << "Already registered: " << store->mType;
		return false;
	}

	sgStores[store->mType] = store;
	bool rslt = AosDataSourceType::addName(store->mType, name);
	sgLock.unlock();
	aos_assert_r(rslt, false);
	return true;
}


bool
AosDataSource::copyMemberData(const AosDataSourcePtr &obj)
{
	return true;
}

