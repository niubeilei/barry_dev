////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: RPCCaller.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_TestUtil_RpcCaller_h
#define Omn_TestUtil_RpcCaller_h

#include "Util/SerialTo.h"
#include "Util/SerialFrom.h"
#include "TestUtil/Ptrs.h"
#include "Thread/Ptrs.h"
#include "Util/String.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"



class OmnRpcCaller : virtual public OmnRCObject
{
	OmnDefineRCObject;

private:
	OmnString		mAgentName;
	OmnString		mFuncName;
	OmnMutexPtr		mLock;
	OmnCondVarPtr	mCondVar;
	OmnSmRPCPtr		mReq;
	OmnSmRPCRespPtr	mResp;
	OmnString		mErrmsg;
	OmnSerialTo		mSerialTo;
	OmnSerialFrom	mSerialFrom;

public:
	OmnRpcCaller(const OmnString &funcName, 
				 const OmnString &agentName);
	virtual ~OmnRpcCaller() {}

	void		call(const int sec);
	OmnString	getErrmsg() const {return mErrmsg;}
	OmnSmRPCPtr	getReq() const;
	OmnString	getAgentName() const {return mAgentName;}

	OmnRpcCaller &	operator << (const int value) {mSerialTo.push(1, value); return *this;}

	OmnRpcCaller &	operator >> (int &value) {mSerialFrom.pop(1, value); return *this;}

};


#endif

