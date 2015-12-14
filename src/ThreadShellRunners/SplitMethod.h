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
// 07/18/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Omn_ThreadShellRunners_SplitMethod_h 
#define Omn_ThreadShellRunners_SplitMethod_h

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "Rundata/Ptrs.h"
#include "Util/RCObject.h"

#define AOSSPLITMETHOD_BYPAGESIZE     		"pagesize"
#define AOSSPLITMETHOD_BYNUMBEROFCORES     	"core"
#define AOSSPLITMETHOD_BYNUMSEGMENTS 	    "seq"


class AosSplitMethod
{
public:
	enum SplitMethod
	{
		eInvalid,

		eByPageSize,
		eByNumberOfCores,
		eByNumSegments,

		eMax
	};
	inline static SplitMethod toEnum(const OmnString &type)
	{
		aos_assert_r(type != "", eInvalid);
		switch(type.data()[0])
		{
		case 'c':
			if (type == AOSSPLITMETHOD_BYNUMBEROFCORES) return eByNumberOfCores;
		case 'p':
			if (type == AOSSPLITMETHOD_BYPAGESIZE) return eByPageSize;
		case 's':
			if (type == AOSSPLITMETHOD_BYNUMSEGMENTS) return eByNumSegments;
		default:
			OmnAlarm << "Undefined type: " << type << enderr;
			return eInvalid;
		}

		OmnShouldNeverComeHere;
		return eInvalid;
	}
};
#endif
