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
// This is a utility to select docs.
//
// Modification History:
// 04/26/2011	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_ValueSel_ValueType_h
#define AOS_ValueSel_ValueType_h

#include "Util/String.h"
#include "Alarm/Alarm.h"

class AosValueType
{

public:
	enum E
	{
		eInvalid,

		eConstant,
		eObjAttr,

		eMax
	};

	static bool isValid(const E type)
	{
		return (type > eInvalid && type < eMax);
	}

	static E toEnum(const OmnString &str)
	{
		if (str.length() <= 0) return eInvalid;

		const char *data = str.data();
		switch (data[0])
		{
		case '1':
			 return eConstant;

		default:
			 break;
		}
		OmnAlarm << "Invalid type: " << str << enderr;
		return eInvalid;
	}
};
#endif

