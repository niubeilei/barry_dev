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
#include "Book/BookType.h"

#include "alarm_c/alarm.h"


static std::string sgNames[AosBookType::eMaximum];
static bool sgNameInit = false;

static void initNames()
{
	sgNames[AosBookType::eUnknown] 		= "Unknown";
	sgNames[AosBookType::eWebbook] 		= "Webbook";
	sgNames[AosBookType::eSpreadsheet] 	= "Spreadsheet";
	sgNames[AosBookType::eMaximum] 		= "Maximum";
}


std::string 
AosBookType::toStr(AosBookType::E type)
{
	if (!sgNameInit) initNames();

	if (type < eUnknown || type > eMaximum) return "invalid";
	return sgNames[type];
}


AosBookType::E 
AosBookType::toEnum(const char *name)
{
	aos_assert_r(name, eUnknown);

	for (int i=eUnknown+1; i<eMaximum; i++)
	{
		if (strcmp(name, sgNames[i].data()) == 0) return (AosBookType::E)i;
	}

	return eUnknown;
}
