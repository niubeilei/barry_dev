////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: aosMacRule.cpp
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#include "aos_core/aosMacRule.h"

#include "aos/aosKernelAlarm.h"
#include "aosUtil/Slab.h"


static struct AosSlab *sgSlab = 0;
static const int sgMaxEntry = 1010;

int aosMacRule_init()
{
	AosSlab_constructor("aosMacRule", sizeof(struct aosMacRule), 
		sgMaxEntry, &sgSlab);
	if (!sgSlab)
	{
		return aosAlarm(eAosAlarmMemErr);
	}

	return 0;
}


int aosMacRule_destructor(struct aosRule *self)
{
	self->mHead.mNext = 0;
	self->mHead.mPrev = 0;
	self->mHead.mType = eAosRuleType_Idle;
	self->mHead.mProc = 0;
	self->mHead.mDestructor = 0;

	AosSlab_release(sgSlab, self);
	return 0;
}


struct aosMacRule * 
aosMacRule_constructor(aosRuleProc proc, 
					   int type, 
					   aosRuleDestructor destructor)
{
	struct aosMacRule *rule;
	if (!proc)
	{
		aosAlarmR(eAosAlarm_aosMacRule_constructor1);
		return 0;
	}

	rule = (struct aosMacRule *)AosSlab_get(sgSlab);
	if (!rule)
	{
		aosAlarmR(eAosAlarm_aosMacRule_constructor2);
		return 0;
	}

	rule->mHead.mPrev = 0;
	rule->mHead.mNext = 0;
	rule->mHead.mType = type;
	rule->mHead.mRuleId = 0;
	rule->mHead.mProc = proc;

	if (destructor)
	{
		rule->mHead.mDestructor = destructor;
	}
	else
	{
		rule->mHead.mDestructor = aosMacRule_destructor;
	}

	return rule;
};


