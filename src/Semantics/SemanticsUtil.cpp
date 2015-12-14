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
// This file defines semantics related utility functions.
//
// Modification History:
// 11/28/2007: Created by Chen Ding
// 
////////////////////////////////////////////////////////////////////////////
#include "Semantics/SemanticsUtil.h"

#include "Alarm/Alarm.h"
#include "aos/aosReturnCode.h"
#include "SemanticRules/SemanticRule.h"
#include "Semantics/SemanticsReturnCodes.h"


int AosEvaluateSemanticRules(std::list<AosSemanticRulePtr> &theList, 
							 const AosVarContainerPtr &semanticData, 
							 std::string &theErrmsg)
{
	bool semanticsGood = true;
	bool continueEvaluate = true;

	std::list<AosSemanticRulePtr>::iterator itr;
	for (itr = theList.begin(); continueEvaluate && itr != theList.end(); itr++)
	{
		std::string errmsg;
		AosSemanticRulePtr ptr = *itr;
		int rslt = ptr->evaluate(semanticData, errmsg);
		switch (rslt)
		{
		case eAosRc_RuleEvaluateTrue:
			 // 
			 // The rule evaluated correctly.
			 //
			 continue;
			 break;

		case eAosRc_RuleEvaluateContinue:
			 // 
			 // The rule evaluated false, but the rule evaluation can be
			 // continued.
			 //
			 semanticsGood = false;
			 theErrmsg += errmsg;
			 break;

		case eAosRc_RuleEvaluateStop:
		case eAosRc_RuleEvaluateAbortCall:
		case eAosRc_RuleEvaluateAbortApp:
			 // 
			 // The rule evaluated false and the rule evaluation should be
			 // stopped.
			 //
			 semanticsGood = false;
			 theErrmsg += errmsg;
			 continueEvaluate = false;
			 return rslt;

		default:
			 OmnAlarm << "Unrecognized error code for rule evaluation: "
				 << rslt << enderr;
			 return eAosRc_ProgErr;
		}
	}

	// 
	// This means all rules were evaluated correctly. 
	//
	if (semanticsGood) return eAosRc_RuleEvaluateTrue;

	return eAosRc_RuleEvaluateFalse;
}

