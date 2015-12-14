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
// 05/15/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "SEInterfaces/DataScannerType.h"

#include "Alarm/Alarm.h"


static bool sgChecked = false;

static OmnString	sgNames[AosDataScannerType::eMax];

bool
AosDataScannerType::check()
{
	if (sgChecked) return true;
	for (int i=eInvalid+1; i<eMax; i++)
	{
		OmnString name = sgNames[i];
		if (name == "")
		{
			OmnAlarm << "Data blob not registered: " << i << enderr;
			continue;
		}

		if (toEnum(name) != (E)i)
		{
			OmnAlarm << "Data blob not registered correctly: " 
				<< i << ":" << name << enderr;
			continue;
		}
	}

	sgChecked = true;
	return true;
}


bool
AosDataScannerType::addName(const OmnString &name, const E code)
{
	sgNames[code] = name;
	return true;
}
