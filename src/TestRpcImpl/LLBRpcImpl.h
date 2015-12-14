////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: LLBRpcImpl.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_TestRpcImpl_LLBRpcImpl_h
#define Omn_TestRpcImpl_LLBRpcImpl_h

#include "Debug/Rslt.h"
#include "TestUtil/Ptrs.h"

class OmnSerialTo;
class OmnSerialFrom;

class OmnLLBRpcImpl
{
public:
	static OmnRslt	procRpc(const OmnSmRPCPtr &req, OmnSerialTo &s);
	static OmnRslt	sample(OmnSerialFrom &from, OmnSerialTo &to);
};


#endif

