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
// 06/01/2011	Created by Ketty 
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_SeLogUtil_LogType_h 
#define AOS_SeLogUtil_LogType_h 

#include "SeLogUtil/LogNames.h"

#define AOSLOGOPR_ADD			"add"
#define AOSLOGOPR_ADDVERSION	"addver"
#define AOSLOGOPR_RETRIEVE		"retrieve"

#define AOSLOGTYPE_NORM			"norm"
#define AOSLOGTYPE_VERSION		"version"



class AosLogType
{
public:
	enum E
	{
		eInvalid,

		eNorm,
		eVersion,

		eMax
	};

	
	static E toEnum(const OmnString &name)
	{
		if (name.length() <= 0) return eInvalid;
		const char *data = name.data();
		switch (data[0])
		{
		case 'n':
			 if (name == AOSLOGTYPE_NORM) return eNorm;
			 break;

		case 'v':
			 if (name == AOSLOGTYPE_VERSION) return eVersion;
			 break;

		default:
			 break;
		}

		return eInvalid;
	}
	
	static bool isValid(const E code) {return code > eInvalid && code < eMax;}
	
	static inline E convertLognameToLogType(const OmnString &logname)
	{
		if (logname.length() <= 5) return eNorm;
		const char *data = logname.data();
		switch (data[4])
		{
		case 'v':
			 if (logname == AOSLOGNAME_VERSION) return eVersion;
			 break;

		default:
			 break;
		}
		return eNorm;
	}

};
#endif

