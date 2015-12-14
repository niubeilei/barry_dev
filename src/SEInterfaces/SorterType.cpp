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
// 06/04/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "SEInterfaces/SorterType.h"

#include "alarm_c/alarm.h"
#include "Debug/ExitHandler.h"
#include "Thread/Mutex.h"
#include "Util/HashUtil.h"



AosSorterType::E 
AosSorterType::toEnum(const OmnString &id)
{
	if (id.length() < 1) return eInvalid;
	
	if (id == AOSSORTER_MULTIFILE) return eMultiFiles;

	return eInvalid;
}


OmnString  
AosSorterType::toString(const E type)
{
	if (!isValid(type)) return "";

	if (type == eMultiFiles) return AOSSORTER_MULTIFILE;

	return AOSSORTER_INVALID;
}


