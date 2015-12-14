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
// 12/09/2007: Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "SemanticRules/RuleId.h"

#include "alarm/Alarm.h"


static std::string sgNames[AosRuleId::eMaxEntry+1] = 
{
	"Invalid", 
	"Unknown",
	"SR0001", 
	"SR00027", 
	"SR00028", 
	"SR00031", 
	"SR00033", 
	"MaxEntry"
};

std::string		
AosRuleId::toStr(const AosRuleId::E code)
{
	aos_assert_r(code >= 0 && code < eMaxEntry, "OutOfRange");
	return sgNames[code];
}


AosRuleId::E		
AosRuleId::toEnum(const std::string &name)
{
	for (int i=0; i<eMaxEntry; i++)
	{
		if (name == sgNames[i]) return (AosRuleId::E)i;
	}

	return eInvalid;
}


