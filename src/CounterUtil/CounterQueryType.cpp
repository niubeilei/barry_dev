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
// 06/28/2010: Created by Lynch yang
////////////////////////////////////////////////////////////////////////////
#if 0
#include "CounterUtil/CounterQueryType.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "Debug/Debug.h"


AosCounterQueryType 
AosCounterQueryType_strToCode(const OmnString &name)
{
	// All GIC tyes are in the form:
	// 		gic_xxx
	const char *data = name.data();
	switch (data[0])
	{
	case 's':
		 if (name == AOSCOUNTER_QUERY_SINGLE) return eAosCounter_Query_Single;
		 if (name == AOSCOUNTER_QUERY_SUB) return eAosCounter_Query_Sub; 
		 break;

	case 'm':
		 if (name == AOSCOUNTER_QUERY_MULTI) return eAosCounter_Query_Multi;
		 break;
	default: 
		 OmnAlarm << "Unrecognized counter query type :" << name << enderr;
		 break;
	}
	return eAosCounter_Query_Invalid;
}

OmnString
AosCounterQueryType_enumToStr(const AosCounterQueryType typeName)
{
	return "";
}
#endif
