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
#include "JimoAPI/JimoSample.h"

#include "JimoAPI/JimoPackageNames.h"
#include "JimoAPI/JimoClusterMgr.h"
#include "JimoCall/JimoCall.h"
#include "SEInterfaces/JimoCaller.h"
#include "SEInterfaces/ClusterObj.h"
#include "UtilData/FN.h"

using namespace std;

namespace Jimo
{

bool 
JimoSample::helloJimo(AosRundata *rdata)
{
	AosJimoCallPtr jimo_call = OmnNew AosJimoCall(rdata,
		"JimoSample", (int)eHelloJimo, 
		jimoGetJimoCluster(rdata));
	jimo_call->makeCall(rdata);
	return jimo_call->isCallSuccess();
}


bool 
JimoSample::helloJimo(
		AosRundata *rdata, 
		const OmnString &message)
{
	AosJimoCallPtr jimo_call = OmnNew AosJimoCall(rdata,
		"JimoSample", (int)eHelloJimoWithMessage, 
		jimoGetJimoCluster(rdata));
	jimo_call->arg(AosFN::eMessage, message);
	jimo_call->makeCall(rdata);
	return jimo_call->isCallSuccess();
}


/*
bool 
JimoSample::helloJimo(AosJimoRundata *jimo_rdata)
{
	AosRundataPtr rdataptr = jimo_rdata->convert();
	AosRundata * rdata = rdataptr.getPtr();
	AosJimoCallPtr jimocall = OmnNew AosJimoCall(rdata,
		JimoPackageID::eHelloWorld, JimoFunc::eHelloWorld, 
		AosJimoCallDialer::getIILManualDialer(rdata));
	jimocall->syncCall(rdata);
	return jimocall->isCallSuccess();
}


bool 
JimoSample::helloJimo(
		AosJimoRundata *jimo_rdata, 
		const OmnString &message,
		AosJimoCallPtr &jimo_call)
{
	AosRundataPtr rdataptr = jimo_rdata->convert();
	AosRundata * rdata = rdataptr.getPtr();
	jimo_call->set(
		JimoPackageID::eHelloWorld, JimoFunc::eHelloWorldWithResults, 
		AosJimoCallDialer::getIILManualDialer(rdata));
	jimo_call->arg(AosFieldName::eMessage, message);
	jimo_call->syncCall(rdata);
	return jimo_call->isCallSuccess();
}
*/

};

