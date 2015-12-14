////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: LLBRpcImpl.cpp
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#include "TestRpcImpl/LLBRpcImpl.h"

#include "Util/SerialFrom.h"
#include "Util/SerialTo.h"
#include "TestUtil/SmRPC.h"
#include "TestUtil/SmRPCResp.h"


OmnRslt		
OmnLLBRpcImpl::procRpc(const OmnSmRPCPtr &req, OmnSerialTo &s)
{
	OmnString funcName;
	OmnSerialFrom from(req->getParms());
	OmnSerialTo to;
	from.pop(1, funcName);
	if (funcName == "sample")
	{
		return sample(from, to);
	}

	// 
	// If it comes to this point, it is an unrecognized function. 
	//
	OmnRslt rslt(OmnErrId::eUnrecognizedFuncName, "Unrecognized function name");
	return rslt;
}


OmnRslt
OmnLLBRpcImpl::sample(OmnSerialFrom &from, OmnSerialTo &to)
{
	// 
	// The function 'int sample (const int value1, int *value2) is a kernel
	// function. We need to call the kernel function through the kernel 
	// API. If successful, it retrieves the return value and put
	// it into 's'. 
	//

	// 
	// Retrieve the parms. 
	//
	int value1;
	int value2;
	from.pop(1, value1);
	from.pop(1, value2);
	// 
	// Call the kernel API. Not implemented yet. 
	//
	
	// 
	// If it comes to this point, we assume the call is successful, 
	// and the return value is in 'retValue', and the value for 'value2'
	// is stored in 'v2'. 
	//
	int retValue = 0;		// Note: this is for testing only.
	int v2 = 1;				// Note: this is for testing only.
	
	to.push(1, v2);
	to.push(1, retValue);

	OmnTrace << "v2 = " << v2 << ", retValue = " << retValue << endl;
	return true;
}


