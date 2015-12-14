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
// 07/14/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "DataSource/DataSourceType.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"


OmnString AosDataSourceType::smNames[AosDataSourceType::eMax];

bool
AosDataSourceType::check()
{
	bool correct = true;
	for (int i=eInvalid+1; i<eMax; i++)
	{
		OmnString name = smNames[i];
		if (name == "")
		{
			OmnAlarm << "Type not registered: " << i << enderr;
			correct = false;
		}
		else
		{
			E code = toEnum(name);
			if (code != (E)i)
			{
				OmnAlarm << "Registration inconsistent: " << i << ":" << name << enderr;
				correct = false;
			}
		}
	}

	aos_assert_r(correct, false);
	return true;
}


bool
AosDataSourceType::addName(const E code, const OmnString &name)
{
	aos_assert_r(isValid(code), false);
	if (smNames[code] != "")
	{
		OmnAlarm << "Name already registered: " << smNames[code] 
			<< ":" << name << enderr;
		return false;
	}

	smNames[code] = name;
	return true;
}

