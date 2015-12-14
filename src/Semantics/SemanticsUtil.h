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
// Modification History:
// 11/28/2007: Created by Chen Ding
// 
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_Semantics_SemanticsUtil_h
#define Aos_Semantics_SemanticsUtil_h

#include "SemanticRules/Ptrs.h"
#include "SemanticData/Ptrs.h"
#include <list>

extern int AosEvaluateSemanticRules(std::list<AosSemanticRulePtr> &theList, 
							 const AosVarContainerPtr &semanticData, 
							 std::string &theErrmsg);
#endif

