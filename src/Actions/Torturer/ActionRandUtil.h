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
// 01/26/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_Actions_Torturer_ActionRandUtil_h
#define AOS_Actions_Torturer_ActionRandUtil_h

#include "Alarm/Alarm.h"
#include "Random/Ptrs.h"
#include "Rundata/Ptrs.h"
#include "SEUtil/Ptrs.h"
#include "SEInterfaces/Ptrs.h"
#include "Util/String.h"

class AosActionRandUtil
{
public:
	static OmnString pickActions(
						const OmnString &tagname, 
						const int level,
						const AosRandomRulePtr &rule,
						const AosRundataPtr &rdata);
};
#endif

