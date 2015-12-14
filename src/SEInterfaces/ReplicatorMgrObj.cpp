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
// 2014/11/19 Created by White Wu
////////////////////////////////////////////////////////////////////////////
#include "SEInterfaces/ReplicatorMgrObj.h"

#include "Rundata/Rundata.h"

static AosReplicatorMgrObj * 	sgMgrObj = 0;
static OmnMutex					sgLock;


AosReplicatorMgrObj::AosReplicatorMgrObj(const int version)
:
AosJimo(AosJimoType::eReplicatorMgr, version)
{
}


AosReplicatorMgrObjPtr 
AosReplicatorMgrObj::getSelf(AosRundata *rdata)
{
	if (sgMgrObj) return sgMgrObj;

	sgLock.lock();
	if (sgMgrObj)
	{
		sgLock.unlock();
		return sgMgrObj;
	}

	AosJimoPtr jimo = AosCreateJimoByClassname(rdata, "AosReplicatorMgr", 1);
	if (!jimo)
	{
		AosSetError(rdata, "indexmgrobj_internal_error") << enderr;
		sgLock.unlock();
		return 0;
	}

	if (jimo->getJimoType() != AosJimoType::eReplicatorMgr)
	{
		AosSetErrorUser(rdata, "indexmgrobj_invalid_jimo") << enderr;
		sgLock.unlock();
		return 0;
	}

	sgMgrObj = dynamic_cast<AosReplicatorMgrObj*>(jimo.getPtr());
	sgLock.unlock();

	if (!sgMgrObj)
	{
		AosSetError(rdata, "internal_error") << enderr;
		return 0;
	}

	return sgMgrObj;
}


