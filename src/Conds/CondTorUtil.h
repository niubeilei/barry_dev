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
// 01/14/2012: Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_Conds_CondTorUtil_h
#define Aos_Conds_CondTorUtil_h

#include "Conds/Ptrs.h"
#include "Rundata/Ptrs.h"


class AosCondTorUtil
{
public:
	static AosConditionObjPtr getRandCond(const AosRundataPtr &rdata);

};
#endif

