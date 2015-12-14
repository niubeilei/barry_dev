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
// 01/17/2012	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_Conds_CondRandUtil_h
#define AOS_Conds_CondRandUtil_h

#include "Conds/Ptrs.h"
#include "Random/Ptrs.h"
#include "SEUtil/Ptrs.h"
#include "Rundata/Ptrs.h"
#include "Util/String.h"


class AosCondRandUtil
{

public:
	static OmnString pickCondition(
						const OmnString &tagname,
						const int level,
						AosConditionPtr &cond,
						const AosRandomRulePtr &rule,
						const AosRundataPtr &rdata)
	{
		return pickCondition(tagname, level, cond, "", "", rule, rdata);
	}
	static OmnString pickCondition(
						const OmnString &tagname,
						const int level,
						AosConditionPtr &cond,
						const OmnString &then_str, 
						const OmnString &else_str, 
						const AosRandomRulePtr &rule,
						const AosRundataPtr &rdata);

	static OmnString pickConditionWithActions(
						const OmnString &tagname,
						const int level,
						const OmnString &then_tagname, 
						const OmnString &else_tagname, 
						const AosRandomRulePtr &rule,
						const AosRundataPtr &rdata);

	static AosConditionPtr pickCond(const AosRundataPtr &rdata);
};
#endif
