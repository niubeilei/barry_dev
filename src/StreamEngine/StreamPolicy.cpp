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
// 05/14/2012 Created by Ketty 
////////////////////////////////////////////////////////////////////////////
#include "StreamEngine/StreamPolicy.h"

#include "SEInterfaces/SysInfo.h"
#include "Debug/Debug.h"
#include <stdlib.h>
#include "API/AosApiG.h"
#include "StreamEngine/ServiceMgr.h"

AosStreamPolicy::AosStreamPolicy()
{
}

AosStreamPolicy::AosStreamPolicy(const OmnString &str)
{
	mPolicy = strToId(str);
}

AosStreamPolicy::AosStreamPolicy(const Policy &id)
{
	mPolicy = id;
}

AosStreamPolicy::~AosStreamPolicy()
{
}

///////////////////////////////////////////
//  policy method
///////////////////////////////////////////
int 
AosStreamPolicy::getPhyId(int rddId)
{
	int id;

	switch (mPolicy)
	{
		case eRR:
			//id = rand() % AosGetNumPhysicals();
			id = rand() % AosGetNumPhysicals();
			break;

		case eLocal:
			id = AosGetSelfServerId();
			break;

		case eShuffle:
		default:
			id = rddId % AosGetNumPhysicals();
			break;
	}

	return id;
}

u32
AosStreamPolicy::getLogicPid(const int svr_id)
{
	//1. specify svr_id get the logic pid(task.exe logic id)
	//2. the process has the service
	AosServiceMgr::ProcInfo info;
	AosServiceMgr::getSelf()->getProcInfo((u32)svr_id, info);
	return info.mLogicPid;
}

bool
AosStreamPolicy::getTargetProcess(
						int &svr_id, 
						u32 &logic_pid, 
						const int rddId,
						const u64 &service_docid)
{
	bool found = false;
	int tmp_svr_id = -1;
	while(!found)
	{
		tmp_svr_id = getPhyId(rddId);
		found  = AosServiceMgr::getSelf()->getLogicPid(logic_pid, tmp_svr_id, service_docid);
	}
	svr_id = tmp_svr_id;
	return true;
}

///////////////////////////////////////////
//  helper method
///////////////////////////////////////////
OmnString
AosStreamPolicy::idToStr(const Policy &id)
{
	OmnString str;

	switch (id)
	{
		case eRR:
			str = "rr";
			break;
			
		case eLocal:
			str = "rr";
			break;
			
		case eShuffle:
			str = "shuffle";
			break;
			
		default:
			str = "invalid";
			break;
	}

	return str;
}

AosStreamPolicy::Policy
AosStreamPolicy::strToId(const OmnString &str)
{
	Policy id;

	if (str == STREAMPOLICY_RR)
		id = eRR;
	else if (str == STREAMPOLICY_LOCAL)
		id = eLocal;
	else if (str == STREAMPOLICY_SHUFFLE)
		id = eShuffle;
	else
		id = eInvalid;

	return id;
}
