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
#include "EventMgr/EventCreator.h"

#include "Rundata/Rundata.h"
#include "SEInterfaces/EventObj.h"
#include "SEInterfaces/TaskDataObj.h"
#include "Thread/Mutex.h"
#include "Util/Buff.h"


static AosEventObjPtr 	sgEvents[AosEventType::eMax];
static OmnMutex			sgLock;
static OmnMutex			sgInitLock;
static bool				sgInited = false;


bool
AosEventCreator::init(const AosRundataPtr &rdata)
{
	if (sgInited) return true;

 	// static AosEventObjPtr lsFileCacher = OmnNew AosCacherFile(true);

	sgInitLock.lock();
	AosEventType::check();
	sgInited = true;
	sgInitLock.unlock();
	return true;
}


AosEventObjPtr 
AosEventCreator::createEvent(const AosXmlTagPtr &def, const AosRundataPtr &rdata)
{
	if (!sgInited) init(rdata);

	aos_assert_rr(def, rdata, 0);
	AosEventType::E type = AosEventType::toEnum(def->getAttrStr(AOSTAG_TYPE));
	aos_assert_rr(AosEventType::isValid(type), rdata, 0);
	sgLock.lock();
	AosEventObjPtr event = sgEvents[type];
	sgLock.unlock();
	return event->clone(def, rdata);
}


AosEventObjPtr 
AosEventCreator::serializeFrom(const AosBuffPtr &buff, const AosRundataPtr &rdata)
{
	if (!sgInited) init(rdata);

	AosEventType::E type = (AosEventType::E)buff->getU32(-1);
	aos_assert_rr(AosEventType::isValid(type), rdata, 0);
	sgLock.lock();
	AosEventObjPtr event= sgEvents[type]->clone();
	sgLock.unlock();
	event->serializeFrom(buff, rdata);
	return event;
}


bool
AosEventCreator::registerEvent(const OmnString &name, AosEventObj *event)
{
	sgLock.lock();
	if (!AosEventType::isValid(event->getType()))
	{
		sgLock.unlock();
		OmnString errmsg = "Incorrect data event type: ";
		errmsg << event->getType();
		OmnAlarm << errmsg << enderr;
		return false;
	}

	if (sgEvents[event->getType()])
	{
		sgLock.unlock();
		OmnString errmsg = "Event already registered: ";
		errmsg << event->getType();
		OmnAlarm << errmsg << enderr;
		return false;
	}

	sgEvents[event->getType()] = event;
	sgLock.unlock();
	return true;
}

