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
// 11/24/2007: Created by Chen Ding
// 
////////////////////////////////////////////////////////////////////////////
#include "SemanticRules/RuleCreator.h"

#include "Alarm/Alarm.h"
#include "SemanticRules/SR00028.h"
#include "SemanticRules/RuleMgr.h"
#include "Util/OmnNew.h"

AosSemanticRulePtr aos_create_rule_priv(
				const AosRuleId::E ruleId, 
				void *inst, 
				const std::string &parm1, 
				const std::string &parm2);

AosSemanticRulePtr 
aos_create_rule(const AosRuleId::E ruleId, 
				void *inst, 
				const std::string &parm1, 
				const std::string &parm2)
{
	AosSemanticRulePtr rule = aos_create_rule_priv(
			ruleId, inst, parm1, parm2);
	if (!rule)
	{
		OmnAlarm << "Failed to create rule" << enderr;
		return 0;
	}

	if (!AosRuleMgrSelf->addRule(rule))
	{
		OmnAlarm << "failed to add rule" << enderr;
		return 0;
	}

	rule->enable();
	return rule;
}


AosSemanticRulePtr
aos_create_rule_priv(
				const AosRuleId::E ruleId, 
				void *inst, 
				const std::string &parm1, 
				const std::string &parm2)
{
	AosSemanticRulePtr rule;
	switch (ruleId)
	{
	case AosRuleId::eSR00028:
		 rule = OmnNew AosSR00028(parm1, parm2, inst);
		 break;

	default:
		 OmnAlarm << "No creator yet: " << AosRuleId::toStr(ruleId)
			 << enderr;
		 return 0;
	}

	if (!rule)
	{
		OmnAlarm << "Run out of memory!" << enderr;
	}
	return rule;
}

