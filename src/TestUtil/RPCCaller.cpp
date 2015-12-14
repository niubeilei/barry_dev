////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: RPCCaller.cpp
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#include "TestUtil/RpcCaller.h"

#include "TestServer/TestServer.h"
#include "TestUtil/SmRPC.h"
#include "TestUtil/SmRPCResp.h"
#include "Thread/Mutex.h"
#include "Thread/CondVar.h"
#include "Util/OmnNew.h"


OmnRpcCaller::OmnRpcCaller(const OmnString &funcName, 
						   const OmnString &agentName)
				   :
mAgentName(agentName),
mFuncName(funcName),
mLock(OmnNew OmnMutex()),
mCondVar(OmnNew OmnCondVar())
{
	mSerialTo.push(1, mFuncName);
}


void
OmnRpcCaller::call(const int sec)
{
	if (!mReq)
	{
		// 
		// Need to create the message. 
		//
		mReq = OmnNew OmnSmRPC(mSerialTo.getDataBuffer());
	}

	mLock->lock();
	OmnRpcCallerPtr thisPtr(this, false);
	OmnTestServerSelf->callFunc(thisPtr);

	// 
	// Wait for 
	//
	bool isTimeout;
	mCondVar->timedWait(mLock, isTimeout, sec);

	mLock->unlock();
	if (isTimeout)
	{
		// 
		// Server not responding. 
		//
		mErrmsg = "Server not responding";
		OmnExcept e(OmnFileLine, OmnErrId::eRPCFailed, mErrmsg);
		throw e;
	}

	if (!mResp)
	{
		// 
		// Something is wrong. 
		//
		mErrmsg = "Response is null";
		OmnExcept e(OmnFileLine, OmnErrId::eRPCFailed, mErrmsg);
		throw e;
	}

	mSerialFrom.setBuffer(mResp->getReturnValue());
}


OmnSmRPCPtr
OmnRpcCaller::getReq() const
{
	return mReq;
}
