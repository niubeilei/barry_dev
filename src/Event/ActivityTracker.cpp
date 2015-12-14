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
//   
//
// Modification History:
// 12/06/2007: Created by Chen Ding
// 
////////////////////////////////////////////////////////////////////////////
#include "Event/ActivityTracker.h"

#include "Event/EventId.h"
#include "Util/a_string.h"



AosActivityTracker::AosActivityTracker()
	:
mKeepHistory(false)
{
}


AosActivityTracker::~AosActivityTracker()
{
}


bool	
AosActivityTracker::logActivity(const AosEventId::E id, 
					void *inst,
					const std::string &fname, 
					const int lineno, 
					std::string &errmsg)
{
	bool found = false;
	bool needToAdd = true;
	bool errFound = false;
	errmsg = "";
	if (AosEventId::isCancelingEvent((AosEventId::E)id))
	{
		// 
		// Check whether the event cancels an activity in the pending
		// list. If yes, remove that pending event. 
		//
		AosActivityItr itr;
		for (itr = mPendingAct.begin(); itr != mPendingAct.end(); itr++)
		{
			if (AosEventId::isPairEvent((AosEventId::E)id, 
										(AosEventId::E)(*itr).getEventId(), 
										inst, (*itr).getInst()))
			{
				mPendingAct.erase(itr);
				found = true;
				needToAdd = false;
				break;
			}
		}

		if (!found)
		{
			// 
			// It is a canceling event but found no matching event.
			//
			errmsg << "Event: " << AosEventId::toStr((AosEventId::E)id)
				<< " requires a matching event but not found!";
			errFound = true;
		}
	}

	if (needToAdd)
	{
		mPendingAct.push_back(AosActivity(id, fname, lineno, inst));
	}

	if (mKeepHistory)
	{
		mActLog.push_back(AosActivity(id, fname, lineno, inst));
	}

	return (errFound)?false:true;
}


bool	
AosActivityTracker::anyPendingActivity() const
{
	return mPendingAct.size();
}


// 
// It checks whether an event 'eventId' has occurred on 
// instance 'inst'. Return true if yes, otherwise, return false.
//
bool	
AosActivityTracker::checkActivity(const AosEventId::E eventId, void *inst)
{
	AosActivityItr itr;
	for (itr = mPendingAct.begin(); itr != mPendingAct.end(); itr++)
	{
		if ((*itr).getEventId() == (u32)eventId && (*itr).getInst() == inst)
		{
			return true;
		}
	}

	return false;
}


std::string		
AosActivityTracker::toString()
{
	std::string str;
	str << "Activity Tracker: " << this;
	AosActivityItr itr;
	for (itr = mPendingAct.begin(); itr != mPendingAct.end(); itr++)
	{
		str << "\n" << (*itr).toString();
	}

	return str;
}

