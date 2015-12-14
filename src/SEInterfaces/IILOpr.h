////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// Modification History:
// 2015/07/22 Created by Jozhi
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_SEInterfaces_IILOpr_h
#define AOS_SEInterfaces_IILOpr_h

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "Util/String.h"


#define AOSIILOPR_INVALID 		"invalid"
#define AOSIILOPR_INC	 		"inc"
#define AOSIILOPR_ADD	 		"add"
#define AOSIILOPR_DEL	 		"del"

class AosIILOpr
{
public:
	enum E
	{
		eInvalid,

		eInc,
		eAdd,
		eDel,

		eMax
	};

	static OmnString toStr(const E code)
	{
		switch (code)
		{
		case eInc:		return AOSIILOPR_INC;
		case eAdd:		return AOSIILOPR_ADD;
		case eDel:		return AOSIILOPR_DEL;
		default:		return AOSIILOPR_INVALID;
		}
		OmnAlarm << "Unrecognized status code: " << code << enderr;
		return AOSIILOPR_INVALID;
	}

	static E toEnum(const OmnString &str)
	{
		if (str == AOSIILOPR_INC) return eInc;
		if (str == AOSIILOPR_ADD) return eAdd;
		if (str == AOSIILOPR_DEL) return eDel;
		return eInvalid;
	}

	static bool isValid(const AosIILOpr::E code)
	{
		return code > eInvalid && code < eMax;
	}

};

#endif
