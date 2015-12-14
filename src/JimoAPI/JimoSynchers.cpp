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
// 2015/09/16 Created by Young
////////////////////////////////////////////////////////////////////////////
#include "JimoAPI/JimoSynchers.h"

#include "alarm_c/alarm.h"
#include "JimoAPI/JimoRepPolicyMgr.h"
#include "JimoAPI/JimoFunc.h"
#include "JimoAPI/JimoClusterMgr.h"
#include "JimoAPI/JimoRepPolicyMgr.h"
#include "JimoCall/JimoCall.h"
#include "JimoCall/JimoCallSyncRead.h"
#include "JimoCall/JimoCallAsync.h"
#include "SEInterfaces/ClusterObj.h"
#include "SEInterfaces/SyncherObj.h"
#include "SEInterfaces/IDOServerObj.h"
#include "Thread/Mutex.h"
#include "Util/String.h"
#include "UtilData/JPID.h"
#include "UtilData/JSID.h"


namespace Jimo
{

AosJimoCallPtr jimoSendSyncher(
		AosRundata *rdata, 
		AosSyncherObj *syncher,
		AosJimoCallerPtr caller)
{
	// This function sends a syncher. One syncher should contain
	// data for one destination. Otherwise, it is an error.
	// This is an async call. 
	//
	// The function will return a jimo call. 
	// It is up to the caller to determine whether it wants to 
	// wait for this call to finish or 'forget for now and let
	// me know if failed'. 
	rdata->setJPID(AOSJPID_SYSTEM);
	rdata->setJSID(AOSJSID_SYSTEM);
	
	AosClusterObj *cluster = jimoGetCluster(rdata, "sync_engine");
	aos_assert_rr(cluster, rdata, 0);
	aos_assert_rr(syncher, rdata, 0);
	int distr_id = syncher->getDestCubeID();

	AosSyncherType::E synctype = syncher->getType();

	AosJimoCallPtr jimo_call = OmnNew AosJimoCallAsync(rdata,
		"AosSyncEngineJimoCallHandler", distr_id, cluster, caller, 0, 0);

	AosBuffPtr buff = syncher->serializeToBuff();
	jimo_call->arg(AosFN::eMethod, AosFN::eRunSyncher);
	jimo_call->arg(AosFN::eType, synctype);
	jimo_call->arg(AosFN::eBuff, buff);

	jimo_call->makeCall(rdata);
	return jimo_call;
}

};


