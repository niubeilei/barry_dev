////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// Description:
// This is a utility to select docs.
//
// Modification History:
// 04/26/2011	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "TimeSelector/TimeSelector.h"

#include "alarm_c/alarm.h"
#include "MultiLang/LangTermIds.h"
#include "Random/RandomRule.h"
#include "Rundata/Rundata.h"
#include "SEInterfaces/DocClientObj.h"
#include "Thread/Mutex.h"
#include "XmlUtil/XmlTag.h"

extern AosTimeSelectorPtr	sgTimeSelectors[AosTimeSelector::eMax+1];
static OmnMutex				sgLock;
AosDocClientObjPtr AosTimeSelector::smDocClient;

#include "TimeSelector/AllTimeSelectors.h"

AosTimeSelector::AosTimeSelector(
		const OmnString &name,
		const AosTimeSelectorType::E type, 
		const bool reg)
:
mType(type),
mLock(OmnNew OmnMutex())
{
	AosTimeSelectorPtr thisptr(this, false);
	if (reg) registerSelector(name, thisptr);
}


AosTimeSelector::~AosTimeSelector()
{
}


bool
AosTimeSelector::registerSelector(const OmnString &name, const AosTimeSelectorPtr &selector)
{
	AosTimeSelectorType::E type = selector->getType();
	if (!AosTimeSelectorType::isValid(type))
	{
		OmnAlarm << "Invalid Doc Selector id: " << type << enderr;
		return false;
	}
	mLock->lock();
	if (sgTimeSelectors[type])
	{
		mLock->unlock();
		OmnAlarm << "Doc Selector already registered: " << name << ":" << type << enderr;
		return false;
	}

	sgTimeSelectors[type] = selector;
	AosTimeSelectorType::addName(name, type);
	mLock->unlock();
	return true;
}


AosTimeSelectorPtr
AosTimeSelector::getTimeSelector(
		const AosTimeSelectorType::E type, 
		const AosRundataPtr &rdata)
{
	if (!AosTimeSelectorType::isValid(type))
	{
		AosSetErrorUser(rdata, "unrecognized_action") << type << enderr;
		return 0;
	}

	AosTimeSelectorPtr selector = sgTimeSelectors[type];

	if (!selector)
	{
		AosSetErrorUser(rdata, "action_not_defined") << type << enderr;
		return 0;
	}

	return selector;
}

bool
AosTimeSelector::getTimeStatic(
		AosValueRslt &value,
		const AosXmlTagPtr &sdoc, 
		const AosRundataPtr &rdata)
{
	// AOSMONITORLOG_ENTER(rdata);

	// This function selects docs. 
	if (!sdoc)
	{
		AosSetError(rdata, AOSLT_SDOC_IS_NULL);
		OmnAlarm << rdata->getErrmsg() << enderr;
		// AOSMONITORLOG_FINISH(rdata);
		return 0;
	}

	AosTimeSelectorType::E type = AosTimeSelectorType::toEnum(
		sdoc->getAttrStr(AOSTAG_TIMESEL_TYPE));
	if (!AosTimeSelectorType::isValid(type))
	{
		AosSetError(rdata, AOSLT_INVALID_DOC_SELECTOR) << ": "
			<< AOSLT_DOC_SELECTOR_NAME << ": "
			<< sdoc->getAttrStr(AOSTAG_TIMESEL_TYPE);
		OmnAlarm << rdata->getErrmsg() << enderr;
		// AOSMONITORLOG_FINISH(rdata);
		return 0;
	}

	AosTimeSelectorPtr selector = getTimeSelector(type, rdata);
	aos_assert_r(selector, 0);
	return selector->run(value, sdoc, rdata);
}

