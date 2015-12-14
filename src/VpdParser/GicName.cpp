////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
//
// Modification History:
// 	Created: 12/14/2008 by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "VpdParser/GicName.h"

#include "alarm_c/alarm.h"


static OmnString sgNames[AosGicName::eMaximum];
static bool sgNameInit = false;

static void initNames()
{
	sgNames[AosGicName::eUnknown] 		= "gic_unknown";
	sgNames[AosGicName::eInput] 		= "gic_input";
	sgNames[AosGicName::eNameValue] 	= "gic_namevalue";
	sgNames[AosGicName::ePrompt] 		= "gic_prompt";
	sgNames[AosGicName::eMaximum] 		= "gic_maximum";
}


OmnString
AosGicName::toStr(AosGicName::E code)
{
	if (!sgNameInit) initNames();

	if (code < eUnknown || code > eMaximum) return "invalid";
	return sgNames[code];
}


AosGicName::E 
AosGicName::toEnum(const char *name)
{
	// The names of all gics shall start with 'gic_'
	aos_assert_r(name, eUnknown);

	aos_assert_r(name[0] == 'g' &&
		name[1] == 'i' && name[2] == 'c' && name[3] == '_', AosGicName::eUnknown);

	switch (name[4])
	{
	case 'i':
		 if (strcmp(&name[4], "input") == 0) return eInput;
		 break;

	case 'n':
		 if (strcmp(&name[4], "namevalue") == 0) return eNameValue;
		 break;

	case 'p':
		 if (strcmp(&name[4], "prompt") == 0) return ePrompt;
		 break;
	}
	return eUnknown;
}
