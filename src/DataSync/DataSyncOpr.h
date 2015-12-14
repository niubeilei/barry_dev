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
// 09/28/2010	Created by Chen Ding
// 2014/01/28 Turned off by Chen Ding
////////////////////////////////////////////////////////////////////////////
#if 0
#ifndef Aos_DataSync_DataSyncOpr_h
#define Aos_DataSync_DataSyncOpr_h

#include "Util/String.h" 

#define AOSADD			"Add"

class AosDataSyncOpr
{
public:
	enum E
	{
		eInvalid,
		
		eAdd,

		eMax
	};

	OmnString toStr(const E code)
	{
		switch (code)
		{
			case eAdd:
				return AOSADD;

			default:
				return "invalid";
		}
		return "invalid";
	}

static	E toEnum(const OmnString &name)
	{
		if (name == AOSADD) return eAdd;
		return eInvalid;
	}
};

#endif

#endif
