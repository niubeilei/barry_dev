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
// 02/05/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "GicCodeGen/GicGenType.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "Util/HashUtil.h"


static AosStr2U32_t sgGicGenMap;
static OmnString 	sgNames[AosGicGenType::eMax];

bool 
AosGicGenType::addName(const OmnString &name, const E code)
{
	aos_assert_r(isValid(code), false);
	sgNames[code] = name;
	return true;
}


bool
AosGicGenType::init()
{
	static AosGicGenDataCol(eDataCol, true);

	smNames[eInvalid] 		= AOSGICGEN_INVALID;
	smNames[eMax] 			= AOSGICGEN_INVALID;

	// Make sure the map is created properly
	for (int i=eInvalid+1; i<eMax; i++)
	{
		if (smNames[i] == "")
		{
			OmnAlarm << "GicCodeGenn not registered: " << i << enderr;
			exit(0);
		}

		if (toEnum(smNames[i]) != i)
		{
			OmnAlarm << "GicCodeGen registered incorrectly: " << smNames[i] << enderr;
			exit(0);
		}
	}
	return true;
}


AosGicGenType::E
AosGicGenType::toEnum(const OmnString &opr)
{
	AosStr2U32Itr_t itr = sgGicGenMap.find(opr);
	if (itr == sgGicGenMap.end()) return eInvalid;
	return (E)itr->second;
}


OmnString	
AosGicGenType::toString(const E opr)
{
	if (opr <= eInvalid || opr > eMax) return AOSGICGEN_INVALID;
	return smNames[opr];
}

