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
// This class implements the common functions for all semantic rules.
//
// Modification History:
// 12/04/2007: Created by Chen Ding
// 
////////////////////////////////////////////////////////////////////////////
#include "SemanticRules/SemanticRuleCommon.h"

#include "Alarm/Alarm.h"
#include "Debug/ErrorMgr.h"
#include "Event/Event.h"
#include "Event/ActivityTracker.h"
#include "Event/EventListener.h"
#include "Semantics/SemanticsRuntime.h"


AosSemanticRuleCommon::AosSemanticRuleCommon(
		const AosRuleId::E ruleId, 
		const std::string &name, 
		void *userData,
		u32 dataLen,
		const bool identifiedByProgObj)
:
mRuleId(ruleId),
mName(name), 
mUserData(userData), 
mDataLen(dataLen),
mStatus(false),
mActivityLogging(false),
mIdentifiedByProgObj(identifiedByProgObj)
{
}


AosSemanticRuleCommon::~AosSemanticRuleCommon()
{
	disable();
}


// 
// This function turns on a semantic rule. Before turning on 
// a semantic rule, it needs to register all the events with 
// the event manager. 
//
// If anything goes wrong, it returns a negative error code. 
// Otherwise, it returns 0.
//
bool		
AosSemanticRuleCommon::enable()
{
	if (mStatus) return true;

	AosEventListenerPtr thisPtr(this, false);
	AosEventList::iterator itr;
	for (itr = mEvents.begin(); itr != mEvents.end(); itr++)
	{
		if (!AosSemanticsRuntimeSelf->
				registerEvent(thisPtr, (*itr)->getEventId(), mUserData, mDataLen))
		{
			OmnAlarm << "Failed to register for event: " 
				<< (*itr)->getEventName() 
				<< " for rule: " << getName() << enderr;
			return false;
		}
	}

	return true;
}


bool		
AosSemanticRuleCommon::disable()
{
	if (!mStatus) return true;

	AosSemanticRulePtr thisPtr(this, false);
	AosEventList::iterator itr;
	for (itr = mEvents.begin(); itr != mEvents.end(); itr++)
	{
		if (!AosSemanticsRuntimeSelf->unregisterEvent(thisPtr, (*itr)->getEventId(), mUserData, mDataLen))
		{
			OmnAlarm << "Failed to register for event: " 
				<< (*itr)->getEventName() 
				<< " for rule: " << getName() << enderr;
			return false;
		}
	}

	return true;
}


void
AosSemanticRuleCommon::logActivity(const AosEventId::E eventId, 
							 const AosEventPtr &event) 
{
	std::string errmsg;
	if (!mActivities.logActivity(eventId, mUserData, 
				event->getFilename(), event->getLineno(), errmsg))
	{
		std::string ee = mName;
		ee << ": " << errmsg;
		AosRaiseError(ee);
	}
}


bool		
AosSemanticRuleCommon::registerEvents()
{
	return AosEventRegistrant::registerEvents();
}


bool		
AosSemanticRuleCommon::unregisterEvents()
{
	return AosEventRegistrant::unregisterEvents();
}


u32		
AosSemanticRuleCommon::getHashKey() const
{
	if (isIdentifiedByProgObj())
	{
		return (u32)mUserData;
	}

	return (u32)mUserData;
}


bool	
AosSemanticRuleCommon::isSameObj(const AosSemanticRulePtr &rhs) const
{
	if (isIdentifiedByProgObj())
	{
		return mUserData == ((AosSemanticRuleCommon*)rhs.getPtr())->mUserData;
	}

	return this == rhs.getPtr();
}


bool
AosSemanticRuleCommon::isIdentifiedByProgObj() const
{
	return mIdentifiedByProgObj;
}


std::string	
AosSemanticRuleCommon::toString() const
{
	std::string str;
	str << "=========== Semantic Rule: " << mName << " ============";
	str << "\nRule ID:               " << AosRuleId::toStr(mRuleId)
		<< "\nUserData:              " << mUserData
		<< "\nDataLen:               " << mDataLen
		<< "\nNo of Events:          " << mEvents.size()
		<< "\nNo of Allowed:         " << mAllowedActors.size()
		<< "\nNo of Disallowed:      " << mDisallowedActors.size()
		<< "\nDefault Policy:        " << mDefaultPolicy
		<< "\nActivity Logging:      " << mActivityLogging
		<< "\nIdentified by ProgObj: " << mIdentifiedByProgObj;
	return str;
}


void
AosSemanticRuleCommon::addEvent(const AosEventId::E eventId, 
				void *userData, 
				const u32 dataLen)
{
	AosEventRegistrant::addEntry(eventId, userData, dataLen);
}


void
AosSemanticRuleCommon::removeEvent(const AosEventId::E eventId, 
				void *userData, 
				const u32 dataLen)
{
	AosEventRegistrant::removeEntry(eventId, userData, dataLen);
}

