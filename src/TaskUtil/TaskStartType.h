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
// 06/02/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_TaskUtil_TaskStartType_h
#define AOS_TaskUtil_TaskStartType_h

#include "Util/String.h"

#define AOSTASKSTARTTYPE_INVALID			"invalid"

#define AOSTASKSTARTTYPE_AUTO				"auto"
#define AOSTASKSTARTTYPE_BYTOKEN			"bytoken"

class AosTaskStartType
{
public:
	enum E
	{
		eInvalid,

		eAuto,
		eByToken, 

		eMax
	};

	static bool isValid(const E id) {return id > eInvalid && id < eMax;}

	static E toEnum(const OmnString &reqid)
	{
		if (reqid == AOSTASKSTARTTYPE_AUTO) return eAuto;
		if (reqid == AOSTASKSTARTTYPE_BYTOKEN) return eByToken;
		return eInvalid;
	}

	static OmnString toStr(const E code)
	{
		switch (code)
		{
		case eAuto:			return AOSTASKSTARTTYPE_AUTO;
		case eByToken:		return AOSTASKSTARTTYPE_BYTOKEN;
		default:			break;
		}
		return AOSTASKSTARTTYPE_INVALID;
	}
};

#endif
