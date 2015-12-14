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
// Modification History:
// 2015/02/06 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_QueryUtil_JoinType_h
#define Aos_QueryUtil_JoinType_h

#include "Util/String.h"



struct AosJoinType : public OmnRCObject
{
	enum E
	{
		eInvalid,

		eInnerJoin,
		eLeftOuterJoin,
		eRightOuterJoin,
		eLeftExcJoin,
		eRightExcJoin,
		eXOJoin
	};

	static E toCode(const OmnString &name);
	static OmnString toStr(const E code);
};
#endif

