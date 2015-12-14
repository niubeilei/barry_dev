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
#ifndef Aos_Statemachine_State_h
#define Aos_Statemachine_State_h

#include "Rundata/Ptrs.h"
#include "Statemachine/Ptrs.h"
#include "Statemachine/StateId.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/String.h"



class AosState : virtual public OmnRCObject
{
	OmnDefineRCObject;

public:

protected:
	OmnString			mStateName;
	AosStateId::E		mStateType;	

public:
	AosState(const OmnString &name, const AosStateId::E type, const bool regflag);
	~AosState();

	virtual AosStatePtr 	clone() = 0;
	virtual bool  run(const AosRundataPtr &rdata) = 0;

	AosStatePtr getState(const OmnString &state_id);

private:
	bool 	registerState(const AosStatePtr &state);
};
#endif

