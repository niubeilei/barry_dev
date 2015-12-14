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
// 12/08/2011	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_SEModules_DftVpdPolicy_h
#define AOS_SEModules_DftVpdPolicy_h

#include "Util/String.h"

#define AOSHOMEVPD_POLILCY_BY_USER_ACCT			"by_useracct"
#define AOSHOMEVPD_POLILCY_BY_SEX				"by_sex"

class AosDftVpdPolicy
{
public:
	enum E
	{
		eInvalid,

		eBySex,

		eMax
	};

	static E toEnum(const OmnString &name)
	{
		if (name.length() < 4) return eInvalid;
		const char *data = name.data();
		if (data[0] != 'b' || data[1] != 'y' || data[2] != '_') return eInvalid;

		switch (data[0])
		{
		case 's':
			 if (name == AOSHOMEVPD_POLILCY_BY_SEX) return eBySex;
			 break;
			 
		default:
			 break;
		}
		return eInvalid;
	}

	static bool isValid(const E code)
	{
		return code > eInvalid && code < eMax;
	}

	static E getDftPolicy() 
	{
		return eInvalid;
	}
};
#endif

