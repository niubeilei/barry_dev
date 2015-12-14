////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Zykie Networks, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// Description:
//
//
// Modification History:
// 2014/11/19 Created by White Wu
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_SEInterfaces_ReplicatorMgrObj_h
#define Aos_SEInterfaces_ReplicatorMgrObj_h

#include "Jimo/Jimo.h"
#include "SEInterfaces/Ptrs.h"
#include "SEInterfaces/ReplicPolicyType.h"
#include "Util/RCObject.h"
#include "Util/String.h"
#include <map>
using namespace std;


class AosReplicatorMgrObj : public AosJimo
{
private:

public:
	AosReplicatorMgrObj(const int version);

	static AosReplicatorMgrObjPtr getSelf(AosRundata *rdata);
};

#endif
