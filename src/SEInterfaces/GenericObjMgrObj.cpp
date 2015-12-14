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
// 2014/10/26	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "SEInterfaces/GenericObjMgrObj.h"

#include "Rundata/Rundata.h"

static AosGenericObjMgrObjPtr 	sgMgrObj;
static OmnMutex					sgLock;


AosGenericObjMgrObj::AosGenericObjMgrObj(const int version)
:
AosJimo(AosJimoType::eGenericObjMgr, version)
{
}

AosGenericObjMgrObj::~AosGenericObjMgrObj()
{
}


AosGenericObjMgrObjPtr 
AosGenericObjMgrObj::getSelf(AosRundata *rdata)
{
	if (sgMgrObj) return sgMgrObj;

	sgLock.lock();
	if (sgMgrObj)
	{
		sgLock.unlock();
		return sgMgrObj;
	}

	AosJimoPtr jimo = AosCreateJimoByClassname(rdata, "AosGenericObjMgr", 1);
	if (!jimo)
	{
		AosSetError(rdata, "indexmgrobj_internal_error") << enderr;
		sgLock.unlock();
		return 0;
	}

	if (jimo->getJimoType() != AosJimoType::eGenericObjMgr)
	{
		AosSetErrorUser(rdata, "indexmgrobj_invalid_jimo") << enderr;
		sgLock.unlock();
		return 0;
	}

	sgMgrObj = dynamic_cast<AosGenericObjMgrObj*>(jimo.getPtr());
	sgLock.unlock();

	if (!sgMgrObj)
	{
		AosSetError(rdata, "internal_error") << enderr;
		return 0;
	}

	return sgMgrObj;
}

