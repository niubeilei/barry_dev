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
// 09/29/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "SEInterfaces/EventObj.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "Debug/Except.h"
#include "Rundata/Rundata.h"


AosEventCreatorObjPtr AosEventObj::smCreator;

AosEventObj::AosEventObj(
		const OmnString &name, 
		const AosEventType::E type, 
		const bool flag)
:
mType(type)
{
	if (flag)
	{
		if (!registerEvent(name, this))
		{
			OmnThrowException("failed_registering");
			return;
		}
	}
}


AosEventObj::AosEventObj()
{
}

AosEventObj::~AosEventObj()
{
}


AosEventObjPtr 
AosEventObj::createEvent(const AosXmlTagPtr &def, const AosRundataPtr &rdata)
{
	aos_assert_rr(smCreator, rdata, NULL);
	return smCreator->createEvent(def, rdata);
}
	

AosEventObjPtr 
AosEventObj::serializeFromStatic(const AosBuffPtr &def, const AosRundataPtr &rdata)
{
	aos_assert_rr(smCreator, rdata, NULL);
	return smCreator->serializeFrom(def, rdata);
}


bool
AosEventObj::registerEvent(const OmnString &name, AosEventObj *cacher)
{
	aos_assert_r(smCreator, false);
	return smCreator->registerEvent(name, cacher);
}

