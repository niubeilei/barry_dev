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
// 03/10/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_NetworkMgr_NetworkTransType_h
#define Aos_NetworkMgr_NetworkTransType_h

#include "Util/String.h"

#define AOSNETWORKTRANSNAME_RESTARTBACKEND				"restartbackend"

class AosNetworkTransType
{
public:
	enum E
	{
		eInvalid,

		eRestartBackend,
		eCommand,

		eMax
	};

	static bool isValid(E code) {return code > eInvalid && code < eMax;}
	static E toEnum(const OmnString &name);
	static bool addName(const OmnString &name, const E eid);
};
#endif

