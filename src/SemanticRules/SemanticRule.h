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
// This is the super class for all semantic rules.  
//
// Modification History:
// 11/24/2007: Created by Chen Ding
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Aos_Semantics_SemanticRule_h
#define Aos_Semantics_SemanticRule_h

#include "aosUtil/Types.h"
#include "Event/Ptrs.h"
#include "Event/EventListener.h"
#include "SemanticData/Ptrs.h"
#include "SemanticRules/RuleId.h"
#include "SemanticRules/Ptrs.h"
#include "Util/RCObject.h"
#include <string>
#include <list>

extern bool aos_evaluate_one_rule(const std::string &filename, 
				int lineno, 
				const AosSemanticRulePtr &rule, 
				std::list<u32> &error_ids);

#ifndef aos_evaluate_rule
#define aos_evaluate_rule(rule, error_list) 					\
	aos_evaluate_one_rule(__FILE__, __LINE__, rule, error_list)
#endif

typedef std::list<AosSemanticRulePtr> AosRuleList;

class AosSemanticRule : virtual public AosEventListener 
{
public:
	// 
	// This is the virtual function all semantic rules
	// must override. 
	//
	// Parameters:
	// data:   This is the semantics data. 
	// errmsg: If a rule failed the evaluation, this parameter will
	//         return the error message. 
	//
	// Return Values:
	// 0:   Semantic rule evaluation to false.
	// 1:   Semantic rule evaluation to true.
	// < 0: Errors were encountered during rule evaluation.
	//
	virtual int	evaluate(const AosVarContainerPtr &data, std::string &errmsg) = 0;
	virtual bool procEvent(const AosEventPtr &event, bool &cont) = 0;
	virtual AosRuleId::E	getRuleId() const = 0;
	virtual std::string		getName() const = 0;
	virtual bool			enable() = 0;
	virtual bool			disable() = 0;
	virtual bool			registerEvents() = 0;
	virtual bool			unregisterEvents() = 0;
	virtual bool			isIdentifiedByProgObj() const = 0;
	virtual u32				getHashKey() const = 0;
	virtual bool			isSameObj(const AosSemanticRulePtr &rhs) const = 0;
	virtual std::string		toString() const = 0;
};
#endif

