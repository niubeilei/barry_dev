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
// 01/11/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "Conds/Torturer/CondRandUtil.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "Conds/AllConds.h"
#include "Conds/Condition.h"
#include "Conds/CondTypes.h"
#include "Random/RandomUtil.h"
#include "Random/RandomRule.h"
#include "Rundata/Rundata.h"
#include "SEInterfaces/Ptrs.h"
#include "SEInterfaces/ActionObj.h"
#include "SEInterfaces/TaskObj.h"
#include "Thread/Mutex.h"
#include "Util/HashUtil.h"
#include "ValueSel/ValueRslt.h"
#include "XmlUtil/XmlTag.h"


AosConditionPtr
AosCondRandUtil::pickCond(const AosRundataPtr &rdata)
{
	// This function randomly creates a new condition.
	AosCondType::E type = AosCondType::getRandomType();
	AosConditionPtr cond = AosCondition::getCondition(type, rdata);
	aos_assert_r(cond, 0);
	return cond->clone(rdata);
}


OmnString 
AosCondRandUtil::pickCondition(
		const OmnString &tagname,
		const int level,
		AosConditionPtr &condition,
		const OmnString &then_str, 
		const OmnString &else_str, 
		const AosRandomRulePtr &rule,
		const AosRundataPtr &rdata)
{
	condition = 0;
	AosConditionPtr cond = pickCond(rdata);
	aos_assert_r(cond, "");
	OmnString str = cond->getXmlStr(tagname, level, then_str, else_str, rule, rdata);
	condition = cond->clone(rdata);
	return str;
}


OmnString 
AosCondRandUtil::pickConditionWithActions(
		const OmnString &tagname,
		const int level,
		const OmnString &then_tagname, 
		const OmnString &else_tagname, 
		const AosRandomRulePtr &rule,
		const AosRundataPtr &rdata)
{
	/*
	// This function generates a condition with randomly created actions. 
	// Since conditions and actions can be recursively embedded into 
	// each other, 'level' is used to prevent infinite embedding. When
	// 'level' is zero, it will not generate actions anymore.
	AosConditionPtr cond = pickCond(rdata);
	aos_assert_r(cond, "");

	OmnString then_actions, else_actions;
	if (level > 0)
	{
		if (OmnRandom::percent(30))
		{
			OmnString tname = then_tagname;
			if (tname == "") tname = "thenactions";
			AosActionObjPtr action_obj = AosActionObj::getActionObj();
			aos_assert_r(action_obj, "");
			then_actions = action_obj->pickActions(tname, level-1, rule, rdata);
		}

		if (OmnRandom::percent(30))
		{
			OmnString tname = then_tagname;
			if (tname == "") tname = "elseactions";
			AosActionObjPtr action_obj = AosActionObj::getActionObj();
			aos_assert_r(action_obj, "");
			else_actions = action_obj->pickActions(tname, level-1, rule, rdata);
		}
	}

	return cond->getXmlStr(tagname, level, then_actions, else_actions, rule, rdata);
	*/
	OmnNotImplementedYet;
	return false;
}

