////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: aosRule.cpp
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#include "aos_core/aosRule.h"

#include "aos/aosKernelAlarm.h"
#include "aosUtil/Slab.h"

#include <KernelSimu/string.h>


static struct AosSlab *sgSlab = 0;
static const int sgMaxEntry = 10000;

int aosRule_init()
{
	AosSlab_constructor("aosRule", sizeof(struct aosRule), sgMaxEntry, &sgSlab);
	if (!sgSlab)
	{
		return aosAlarm(eAosAlarmMemErr);
	}

	return 0;
}

struct aosRule * 
aosRule_constructor(aosRuleProc proc, 
					int type, 
					aosRuleDestructor destructor)
{
	struct aosRule *rule = (struct aosRule *)AosSlab_get(sgSlab);

	if (!proc)
	{
		aosAlarmR(eAosAlarm_aosRule_constructor1);
		return 0;
	}

	if (!rule)
	{
		aosAlarmR(eAosAlarm_aosRule_constructor2);
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
		rule->mHead.mDestructor = aosRule_destructor;
	}

	return rule;
};


int aosRule_destructor(struct aosRule *self)
{
	self->mHead.mNext = 0;
	self->mHead.mPrev = 0;
	self->mHead.mType = eAosRuleType_Idle;
	self->mHead.mProc = 0;
	self->mHead.mDestructor = 0;

	AosSlab_release(sgSlab, self);
	return 0;
}


int aosRule_getRuleName(int ruleId, char *name)
{
	switch (ruleId)
	{
	case eAosRuleType_BridgeRule:
		 strcpy(name, "BridgeRule");
		 break;

	case eAosMacCtlrRuleType_Filter:
		 strcpy(name, "MacFilter");
		 break;

	case eAosMacCtlrRuleType_NoAos:
		 strcpy(name, "MacNoAos");
		 break;

	case eAosMacCtlrRuleType_MacAos:
		 strcpy(name, "MacAos");
		 break;

	case eAosRuleType_Idle:
		 strcpy(name, "Idle");
		 break;

	default:
		 strcpy(name, "Unrecognized");
		 break;
	}

	return 0;
}

