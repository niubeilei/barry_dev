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
#ifndef Aos_Event_ActivityTracker_h
#define Aos_Event_ActivityTracker_h

#include "Event/EventId.h"
#include "Event/Activity.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"


class AosActivityTracker : virtual public OmnRCObject
{
	OmnDefineRCObject;

private:
	AosActivityList		mPendingAct;
	AosActivityList		mActLog;
	bool				mKeepHistory;

public:
	AosActivityTracker();
	~AosActivityTracker();

	bool	logActivity(const AosEventId::E id, 
					void *inst,
					const std::string &fname, 
					const int lineno, 
					std::string &errmsg);
	bool	anyPendingActivity() const;
	bool	checkActivity(const AosEventId::E eventId, void *inst);

	std::string		toString();
};

#endif

