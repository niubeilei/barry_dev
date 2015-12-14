////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// Modification History:
// 2015/01/19 Created by Jozhi
////////////////////////////////////////////////////////////////////////////
#include "Job/JobRunTime.h"

AosJobRunTime::AosJobRunTime()
:
mLock(OmnNew OmnMutex())
{
}

AosJobRunTime::~AosJobRunTime()
{
}


AosValueRslt
AosJobRunTime::get(
		const OmnString &key,
		const AosRundataPtr &rdata)
{
	AosValueRslt value;
	mLock->lock();
	mVariableEnv->getValue(key, value);
	mLock->unlock();
	return value;
}

bool
AosJobRunTime::set(
		const OmnString &key, 
		const AosValueRslt &value,
		const AosRundataPtr &rdata)
{
	mLock->lock();
	mVariableEnv->setValue(key, value);
	mLock->unlock();
	return true;
}
