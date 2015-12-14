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
// 04/28/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_IILRouter_IILRouter_h
#define Aos_IILRouter_IILRouter_h

#include "Rundata/Ptrs.h"
#include "Util/Opr.h"
#include "Util/Orders.h"

class AosIILRouter
{
public:
	static bool getPhysicalServerId(const u64 &iilid, const AosRundataPtr &rdata);
};
#endif
