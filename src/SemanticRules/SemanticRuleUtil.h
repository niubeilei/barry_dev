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
// For the definition of SR0001, please refer to the document.  
//
// Modification History:
// 12/01/2007: Created by Chen Ding
// 
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_SemanticRule_SemanticRuleUtil_h
#define Aos_SemanticRule_SemanticRuleUtil_h

#include "SemanticRules/Ptrs.h"
#include <list>

typedef std::list<AosSemanticRulePtr> AosRuleList;
typedef std::list<AosSemanticRulePtr>::iterator AosRuleListItr;
typedef AosSemanticRulePtr (*aos_rule_creator_func)();

extern bool aos_register_rule_creator(
					const std::string &name, 
					aos_rule_creator_func func);
#endif

