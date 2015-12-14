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
// 07/31/2011	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "Statemachine/State.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "Debug/Except.h"
#include "Thread/Mutex.h"


static AosStatePtr		sgStates[AosStateId::eMax];
static OmnMutex			sgLock;

AosState::AosState(const OmnString &name, const AosStateId::E type, const bool regflag)
:
mStateName(name),
mStateType(type)
{
	if (regflag) 
	{
		AosStatePtr thisptr(this, false);
		if (!registerState(thisptr))
		{
			OmnString errmsg = "Failed registering state: ";
			errmsg << name << ":" << (int)type;
			OmnThrowException(errmsg);
		}
	}
}


AosState::~AosState()
{
}


bool
AosState::registerState(const AosStatePtr &state)
{
	sgLock.lock();
	if (!AosStateId::isValid(state->mStateType))
	{
		sgLock.unlock();
		OmnAlarm << "Incorrect state id: " << state->mStateType << enderr;
		return false;
	}

	if (sgStates[state->mStateType])
	{
		sgLock.unlock();
		OmnAlarm << "State already registered: " << state->mStateType << enderr;
		return false;
	}

	sgStates[state->mStateType] = state;
	bool rslt = AosStateId::addName(state->mStateName, state->mStateType);
	sgLock.unlock();
	return rslt;
}



AosStatePtr
AosState::getState(const OmnString &state_id)
{
	sgLock.lock();
	AosStateId::E id = AosStateId::toEnum(state_id);
	if (!AosStateId::isValid(id))
	{
		sgLock.unlock();
		OmnAlarm << "Unrecognized smart doc id: " << state_id << enderr;
		return 0;
	}
	
	AosStatePtr state = sgStates[id];
	sgLock.unlock();
	aos_assert_r(state, 0);
	return state;
}

