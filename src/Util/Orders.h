////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// This type of IIL maintains a list of (string, docid) and is sorted
// based on the string value. 
//
// Modification History:
// 04/04/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_Util_Orders_h
#define AOS_Util_Orders_h

#define AOS_ORDER_ALPHABETIC		"alph"
#define AOS_ORDER_NUMERIC_ALPHA		"nmal"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "Util/String.h"


class AosOrder
{
public:
	enum E
	{
		eInvalid,

		eAlphabetic,
		eNumAlphabetic,

		eMax
	};

public:
	inline static bool isValid(const E code)
	{
		return code > eInvalid && code < eMax;
	}

	inline static E toEnum(const OmnString &name)
	{
		int len = name.length();
		if (len < 4) return eInvalid;
		const char *data = name.data();
		switch (data[0])
		{
		case 'a':
			 if (name == AOS_ORDER_ALPHABETIC) return eAlphabetic;
			 break;

		case 'n':
			 if (name == AOS_ORDER_NUMERIC_ALPHA) return eNumAlphabetic;
			 break;

		default:
			 break;
		}
		return eInvalid;
	}

	inline static OmnString toString(const E code)
	{
		if(code == eAlphabetic) return "alph";
		if(code == eNumAlphabetic) return "nmal";
		OmnAlarm << "error code:" << code << enderr;
		return "";
	}
};
#endif

