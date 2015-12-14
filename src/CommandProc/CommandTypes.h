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
// 03/09/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#if 0
#ifndef Aos_CommandProc_CommandTypes_h
#define Aos_CommandProc_CommandTypes_h

#include "Util/String.h"


class AosCommandType
{
public:
	enum E
	{
		eInvalid,

		eRestartServer,
		eRestartServerFinished,

		eMax
	};

	static bool isValid(const E code) { return code > eInvalid && code < eMax; }
	static E toEnum(const OmnString &name);
	static bool addName(const OmnString &name, const E eid, OmnString &errmsg);
};
#endif
#endif
