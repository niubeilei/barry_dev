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
#ifndef Aos_SemanticRules_RuleCreator_h
#define Aos_SemanticRules_RuleCreator_h

#include "SemanticRules/Ptrs.h"
#include "SemanticRules/RuleId.h"
#include <string>

extern AosSemanticRulePtr aos_create_rule(const AosRuleId::E ruleId, 
						   	void *inst, 
						   	const std::string &parm1, 
						   	const std::string &parm2);

#endif
