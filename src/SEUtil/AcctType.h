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
// 10/02/2010	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef OMN_SEUtil_AcctType_h
#define OMN_SEUtil_AcctType_h

#include "Util/String.h"


class AosAcctType
{
public:
	enum E
	{
		eInvalid,

		eNormal,
		eTimed,
		eUsage,

		eMax
	};

public:
	static E 			toEnum(const OmnString &opr);
	static OmnString	toString(const E opr);
};
#endif

