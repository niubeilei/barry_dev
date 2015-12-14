////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// Description:
//   
//
// Modification History:
// 2014/11/22 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "JimoAPI/JimoHelloWorld.h"

#include "JimoAPI/JimoPackage.h"
#include "JimoAPI/JimoFunc.h"
#include "JimoAPI/JimoClusterMgr.h"
#include "JimoAPI/JimoRepPolicyMgr.h"
#include "JimoCall/JimoCallSyncRead.h"
#include "JimoCall/JimoCallSyncNorm.h"
#include "SEInterfaces/ClusterObj.h"


static OmnString sgHelloWorldClassname = "AosHelloWorldJimoCalls";

namespace Jimo
{

bool 
jimoCallSyncNorm(
		AosRundata *rdata,
		const OmnString &message,
		OmnString &resp_str)
{
	// This example shows how to implement a very simple JimoCall
	// API. It sends 'message' to the remote side, which will 
	// respond a message back. JimoCall supports sync and async 
	// calls. This example implements a sync call.
	AosJimoCallPtr jimo_call = OmnNew AosJimoCallSyncNorm(rdata,
		sgHelloWorldClassname, JimoFunc::eJimoCallSyncNorm,
		jimoGetHelloWorldCluster(rdata));
	jimo_call->arg(AosFN::eMessage, message);

	bool rslt = jimo_call->makeCall(rdata);
	if( !rslt )
	{
		OmnAlarm << " call failed ! " << enderr;
		return false;
	}

	AosValueRslt value;
	rslt = jimo_call->getFieldValue(rdata, AosFN::eReturnValue, value);
	if( !rslt )
	{
		OmnAlarm << enderr;
		return false;
	}
	resp_str = value.getStr();

	return true;
}


bool 
jimoCallSyncNorm(
		AosRundata *rdata,
		const OmnString &message,
		OmnString &resp_str, 
		const int endpoint_id)
{
	// This call is the same as the above except that it will pick 
	// and endpoint manually instead of asking the cluster to 
	// pick the endpoints to send the call.
	AosJimoCallPtr jimo_call = OmnNew AosJimoCallSyncNorm(rdata,
		sgHelloWorldClassname, JimoFunc::eJimoCallSyncNorm,
		jimoGetHelloWorldCluster(rdata), endpoint_id);
	jimo_call->arg(AosFN::eMessage, message);

	bool rslt = jimo_call->makeCall(rdata);
	if( !rslt )
	{
		OmnAlarm << " call failed ! " << enderr;
		return false;
	}

	AosValueRslt value;
	rslt = jimo_call->getFieldValue(rdata, AosFN::eReturnValue, value);
	if( !rslt )
	{
		OmnAlarm << enderr;
		return false;
	}
	resp_str = value.getStr();

	return true;
}


bool 
jimoCallSyncRead(
		AosRundata *rdata,
		const OmnString &message,
		OmnString &resp_str)
{
	AosJimoCallPtr jimo_call = OmnNew AosJimoCallSyncRead(rdata,
		sgHelloWorldClassname, JimoFunc::eJimoCallSyncRead,
		5005, gRepPolicyMgr.getCubeMapRepPolicy(), jimoGetHelloWorldCluster(rdata));
	jimo_call->arg(AosFN::eMessage, message);

	bool rslt = jimo_call->makeCall(rdata);
	if( !rslt )
	{
		OmnAlarm << " call failed ! " << enderr;
		return false;
	}

	AosValueRslt value;
	rslt = jimo_call->getFieldValue(rdata, AosFN::eReturnValue, value);
	if( !rslt )
	{
		OmnAlarm << enderr;
		return false;
	}
	resp_str = value.getStr();

	return true;
}

bool 
jimoCallSyncWrite(
		AosRundata *rdata,
		const OmnString &message,
		OmnString &resp_str)
{
#if 0
	AosJimoCallPtr jimo_call = OmnNew AosJimoCallSyncRead(rdata,
		sgJimoHelloPackageName, JimoFunc::eJimoCallSyncWrite,
		5005, gRepPolicyMgr.getCubeMapRepPolicy(), jimoGetHelloWorldCluster(rdata));
	jimo_call->arg(AosFN::eMessage, message);

	bool rslt = jimo_call->makeCall(rdata);
	if( !rslt )
	{
		OmnAlarm << " call failed ! " << enderr;
		return false;
	}

	AosValueRslt value;
	rslt = jimo_call->getFieldValue(rdata, AosFN::eReturnValue, value);
	if( !rslt )
	{
		OmnAlarm << enderr;
		return false;
	}
	resp_str = value.getValueStr1();
#endif
	return true;
}

bool 
jimoCallAsyncNorm(
		AosRundata *rdata,
		const OmnString &message,
		OmnString &resp_str)
{
#if 0
	AosJimoCallPtr jimo_call = OmnNew AosJimoCallSyncRead(rdata,
		sgJimoHelloPackageName, JimoFunc::eJimoCallAsyncNorm,
		5005, gRepPolicyMgr.getCubeMapRepPolicy(), jimoGetHelloWorldCluster(rdata));
	jimo_call->arg(AosFN::eMessage, message);

	bool rslt = jimo_call->makeCall(rdata);
	if( !rslt )
	{
		OmnAlarm << " call failed ! " << enderr;
		return false;
	}

	AosValueRslt value;
	rslt = jimo_call->getFieldValue(rdata, AosFN::eReturnValue, value);
	if( !rslt )
	{
		OmnAlarm << enderr;
		return false;
	}
	resp_str = value.getValueStr1();
#endif
	return true;
}


bool 
jimoCallAsyncRead(
		AosRundata *rdata,
		const OmnString &message,
		OmnString &resp_str)
{
#if 0
	AosJimoCallPtr jimo_call = OmnNew AosJimoCallSyncRead(rdata,
		sgJimoHelloPackageName, JimoFunc::eJimoCallAsyncRead,
		5005, gRepPolicyMgr.getCubeMapRepPolicy(), jimoGetHelloWorldCluster(rdata));
	jimo_call->arg(AosFN::eMessage, message);

	bool rslt = jimo_call->makeCall(rdata);
	if( !rslt )
	{
		OmnAlarm << " call failed ! " << enderr;
		return false;
	}

	AosValueRslt value;
	rslt = jimo_call->getFieldValue(rdata, AosFN::eReturnValue, value);
	if( !rslt )
	{
		OmnAlarm << enderr;
		return false;
	}
	resp_str = value.getValueStr1();
#endif

	return true;
}

bool 
jimoCallAsyncWrite(
		AosRundata *rdata,
		const OmnString &message,
		OmnString &resp_str)
{
#if 0
	AosJimoCallPtr jimo_call = OmnNew AosJimoCallSyncRead(rdata,
		sgJimoHelloPackageName, JimoFunc::eJimoCallAsyncWrite,
		5005, gRepPolicyMgr.getCubeMapRepPolicy(), jimoGetHelloWorldCluster(rdata));
	jimo_call->arg(AosFN::eMessage, message);

	bool rslt = jimo_call->makeCall(rdata);
	if( !rslt )
	{
		OmnAlarm << " call failed ! " << enderr;
		return false;
	}

	AosValueRslt value;
	rslt = jimo_call->getFieldValue(rdata, AosFN::eReturnValue, value);
	if( !rslt )
	{
		OmnAlarm << enderr;
		return false;
	}
	resp_str = value.getValueStr1();
#endif

	return true;
}

};

