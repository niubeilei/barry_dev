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
#include "SEInterfaces/GenericVerbMgrObj.h"

#include "API/AosApi.h"


static AosGenericVerbMgrObjPtr 	sgGenericVerbMgrObj;
static OmnMutex					sgLock;


AosGenericVerbMgrObj::AosGenericVerbMgrObj(const int version)
:
AosJimo(AosJimoType::eGenericVerbMgr, version)
{
	OmnScreen << "JimoType: " << mJimoType << endl;
}


AosGenericVerbMgrObjPtr 
AosGenericVerbMgrObj::getSelf(AosRundata *rdata)
{
	if (sgGenericVerbMgrObj) return sgGenericVerbMgrObj;

	sgLock.lock();
	if (sgGenericVerbMgrObj)
	{
		sgLock.unlock();
		return sgGenericVerbMgrObj;
	}

	AosJimoPtr jimo = AosCreateJimoByClassname(rdata, "AosGenericVerbMgr", 1);
	if (!jimo)
	{
		AosSetError(rdata, "indexmgrobj_internal_error") << enderr;
		sgLock.unlock();
		return 0;
	}

	if (jimo->getJimoType() != AosJimoType::eGenericVerbMgr)
	{
		AosSetErrorUser(rdata, "indexmgrobj_invalid_jimo") << enderr;
		sgLock.unlock();
		return 0;
	}

	sgGenericVerbMgrObj = dynamic_cast<AosGenericVerbMgrObj*>(jimo.getPtr());
	sgLock.unlock();

	if (!sgGenericVerbMgrObj)
	{
		AosSetError(rdata, "internal_error") << enderr;
		return 0;
	}

	return sgGenericVerbMgrObj;
}


