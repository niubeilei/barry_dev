////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: LLBModule.cpp
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#include "TestUtil/LLBModule.h"

#include "Util/SerialFrom.h"
#include "Util/SerialTo.h"
#include "TestUtil/SmRPC.h"
#include "TestUtil/SmRPCResp.h"
#include "TestUtil/RpcCaller.h"
#include "Thread/Mutex.h"
#include "Thread/CondVar.h"


int
OmnLLBModule::sample(const OmnString &agentName, const int value1, int *value2)
{
	OmnRpcCaller rpc("sample", agentName);
	(rpc << value1) << *value2;
	rpc.call(30);

	int retValue;
	rpc >> *value2;
	rpc >> retValue;
	return retValue;
}


