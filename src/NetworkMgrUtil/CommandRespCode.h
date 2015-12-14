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
// 07/24/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_NetworkMgrUtil_CommandRespCode_h
#define Aos_NetworkMgrUtil_CommandRespCode_h

#include "Util/String.h"


class AosCommandRespCode
{
public:
	enum E
	{
		eInvalid,

		eOk,

		eJobNotFound,
		eCollectorNotFound,

		eMax
	};

	static bool isValid(const E code)
	{
		return code > eInvalid && code < eMax;
	}
};
#endif

