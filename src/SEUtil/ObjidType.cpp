////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// Modification History:
// 01/19/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "SEUtil/ObjidType.h"

#include "Thread/Mutex.h"
#include "Util/HashUtil.h"


AosStr2U32_t AosObjidType::smNameMap;
bool		 AosObjidType::smStatus[AosObjidType::eMax];
bool AosObjidType::smInited = false;
static OmnMutex sgLock;

bool
AosObjidType::init()
{
	if (smInited) return true;
	sgLock.lock();

	memset(smStatus, 0, sizeof(smStatus));
	smNameMap[AOSRSVEDOBJID_NORMAL] 			= eNormal;
	smStatus[eNormal] = true;

	smNameMap[AOSRSVEDOBJID_DOMAIN_OPRARD] 		= eDomainOprArd;
	smStatus[eDomainOprArd] = true;

	smNameMap[AOSRSVEDOBJID_USER_OPRARD]		= eUserOprArd;
	smStatus[eUserOprArd] = true;

	smNameMap[AOSRSVEDOBJID_OPRDOC_CTNR]		= eOprDocCtnr;
	smStatus[eOprDocCtnr] = true;

	smNameMap[AOSRSVEDOBJID_DFT_USER_OPRDOC]	= eDftUserOprDoc;
	smStatus[eDftUserOprDoc] = true;

	smNameMap[AOSRSVEDOBJID_ALARM_LOG]			= eSystem;
	smNameMap[AOSRSVEDOBJID_DICT_PARENT]		= eSystem;
	smStatus[eSystem] = true;

	smNameMap[AOSRSVEDOBJID_STMC_INSTDOC]		= eStmcInstDoc;
	smStatus[eStmcInstDoc] = true;

	smNameMap[AOSRSVEDOBJID_DICTCTNR]			= eDictCtnr;
	smStatus[eDictCtnr] = true;

	for (int i=eInvalid+1; i<eMax; i++)
	{
		if (!smStatus[i])
		{
			OmnAlarm << "Objid Type not registered: " << i << enderr;
		}
	}
	smInited = true;
	sgLock.unlock();
	return true;
}


