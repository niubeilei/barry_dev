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
// 2015/03/14 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "JimoAPI/JimoClusterMgr.h"

#include "JimoAPI/JimoRepPolicyMgr.h"
#include "JimoAPI/JimoDocEngine.h"
#include "JimoAPI/JimoFunc.h"
#include "JimoCall/JimoCall.h"
#include "JimoCall/JimoCallSyncRead.h"
#include "SEInterfaces/ClusterObj.h"
#include "SEInterfaces/ClusterMgrObj.h"
#include "SEInterfaces/CubeMapObj.h"
#include "Thread/Mutex.h"
#include "Util/String.h"

static AosClusterMgrObj *sgClusterMgr = 0;
static OmnMutex sgLock;

void sgInit(AosRundata *rdata)
{
	sgLock.lock();
	if (sgClusterMgr)
	{
		sgLock.unlock();
		return;
	}

	sgClusterMgr = AosClusterMgrObj::getClusterMgr(rdata);
	sgLock.unlock();
}


namespace Jimo
{

AosClusterObj * jimoGetCluster(AosRundata *rdata, const OmnString &cluster_name)
{
	if (!sgClusterMgr) sgInit(rdata);
	aos_assert_rr(sgClusterMgr, rdata, 0);
	return sgClusterMgr->getCluster(rdata, cluster_name);
}


AosClusterObj * jimoGetJimoCluster(AosRundata *rdata)
{
	if (!sgClusterMgr) sgInit(rdata);
	aos_assert_rr(sgClusterMgr, rdata, 0);
	return sgClusterMgr->getJimoCluster(rdata);
}

AosClusterObj * jimoGetAdminEngineCluster(AosRundata *rdata)
{
	if (!sgClusterMgr) sgInit(rdata);
	aos_assert_rr(sgClusterMgr, rdata, 0);
	return sgClusterMgr->getAdminEngineCluster(rdata);
}


AosClusterObj * jimoGetAdminStoreCluster(AosRundata *rdata)
{
	if (!sgClusterMgr) sgInit(rdata);
	aos_assert_rr(sgClusterMgr, rdata, 0);
	return sgClusterMgr->getAdminStoreCluster(rdata);
}


AosClusterObj * jimoGetDocEngineClusterAsync(AosRundata *rdata)
{
	if (!sgClusterMgr) sgInit(rdata);
	aos_assert_rr(sgClusterMgr, rdata, 0);
	return sgClusterMgr->getDocEngineClusterAsync(rdata);
}


AosClusterObj * jimoGetDocEngineCluster(AosRundata *rdata)
{
	if (!sgClusterMgr) sgInit(rdata);
	aos_assert_rr(sgClusterMgr, rdata, 0);
	return sgClusterMgr->getDocEngineCluster(rdata);
}


AosClusterObj * jimoGetDocStoreClusterAsync(AosRundata *rdata)	//by White, 2015-08-28 14:27:01
{
	if (!sgClusterMgr) sgInit(rdata);
	aos_assert_rr(sgClusterMgr, rdata, 0);
	return sgClusterMgr->getDocStoreClusterAsync(rdata);
}


AosClusterObj * jimoGetDocStoreCluster(AosRundata *rdata)
{
	if (!sgClusterMgr) sgInit(rdata);
	aos_assert_rr(sgClusterMgr, rdata, 0);
	return sgClusterMgr->getDocStoreCluster(rdata);
}


AosClusterObj * jimoGetIndexEngineCluster(AosRundata *rdata)
{
	if (!sgClusterMgr) sgInit(rdata);
	aos_assert_rr(sgClusterMgr, rdata, 0);
	return sgClusterMgr->getIndexEngineCluster(rdata);
}


AosClusterObj * jimoGetIndexStoreCluster(AosRundata *rdata)
{
	if (!sgClusterMgr) sgInit(rdata);
	aos_assert_rr(sgClusterMgr, rdata, 0);
	return sgClusterMgr->getIndexStoreCluster(rdata);
}


AosClusterObj * jimoGetQueryEngineCluster(AosRundata *rdata)
{
	if (!sgClusterMgr) sgInit(rdata);
	aos_assert_rr(sgClusterMgr, rdata, 0);
	return sgClusterMgr->getQueryEngineCluster(rdata);
}


AosClusterObj * jimoGetQueryStoreCluster(AosRundata *rdata)
{
	if (!sgClusterMgr) sgInit(rdata);
	aos_assert_rr(sgClusterMgr, rdata, 0);
	return sgClusterMgr->getQueryStoreCluster(rdata);
}


AosClusterObj * jimoGetStreamEngineCluster(AosRundata *rdata)
{
	if (!sgClusterMgr) sgInit(rdata);
	aos_assert_rr(sgClusterMgr, rdata, 0);
	return sgClusterMgr->getStreamEngineCluster(rdata);
}


AosClusterObj * jimoGetStreamStoreCluster(AosRundata *rdata)
{
	if (!sgClusterMgr) sgInit(rdata);
	aos_assert_rr(sgClusterMgr, rdata, 0);
	return sgClusterMgr->getStreamStoreCluster(rdata);
}


AosClusterObj * jimoGetIDOCluster(AosRundata *rdata)
{
	if (!sgClusterMgr) sgInit(rdata);
	aos_assert_rr(sgClusterMgr, rdata, 0);
	return sgClusterMgr->getIDOCluster(rdata);
}

/*
AosCubeMapObj*
JimoClusterMgr::retrieveCubeMap(AosRundata *rdata, const u64 docid) 
{
	aos_assert_rr(docid > 0, rdata, 0);

	AosXmlTagPtr def = gJimoDocEngine.getDocByDocid(rdata, docid, 
		gRepPolicyMgr.getCubeMapRepPolicy(),
		//gClusterMgr.getAdminCluster(rdata));
		gClusterMgr.getAdminStoreCluster(rdata));

	if (!def)
	{
		AosLogError(rdata, true, "cube_map_not_found")
			<< AosFN::eDocid << docid << enderr;
		return 0;
	}

	return createCubeMap(rdata, def);
}
*/

bool jimoGetCubeEndpointIDs(
		AosRundata *rdata, 
		const u32 cube_id, 
		const u64 docid,
		vector<int> &endpoint_ids,
		const u32 replic_policy) 
{
	AosJimoCallPtr jimo_call = OmnNew AosJimoCallSyncRead(rdata,
		"AosCubeMapIDOSvr", JimoFunc::eMethodGetCubeEndpointIDs,
		docid, gRepPolicyMgr.getCubeMapRepPolicy(), jimoGetIDOCluster(rdata));

	jimo_call->arg(AosFN::eCubeID, cube_id);
	jimo_call->arg(AosFN::eDocid, docid);
	jimo_call->arg(AosFN::eReplicPolicy, replic_policy);
	jimo_call->makeCall(rdata);

	bool rslt = jimo_call->isCallSuccess();
	if (!rslt)
	{
		AosLogError(rdata, true, "failed_retrieving_cube_map") << enderr;
		return false;
	}

	AosValueRslt vv;
	rslt = jimo_call->getFieldValue(rdata, AosFN::eReturnValue, vv);
	if (!rslt)
	{
		AosLogError(rdata, true, "failed_retrieving_endpoint_ids")
			<< AosFN("Cube ID") << cube_id 
			<< AosFN("Replication Policy") 
			<< AosRepPolicy::getNumSync(replic_policy) << "."
			<< AosRepPolicy::getNumAsync(replic_policy) << "."
			<< AosRepPolicy::getNumRemotes(replic_policy) << enderr;
		return false;
	}
	endpoint_ids.push_back(vv.getU64());

	return true;
}


bool jimoGetCubeEndpointIDs(
		AosRundata *rdata, 
		const u32 cube_id, 
		const u64 docid,
		vector<int> &endpoint_ids,
		const AosRepPolicy &replic_policy, 
		const AosJimoCallerPtr &caller) 
{
	// This is an async call.
	//ClusterMgrCallerPtr cluster_caller = OmnNew AosClusterMgrCaller(
	//		rdata, &endpoint_ids, caller);
/*
	AosJimoCallPtr jimo_call = OmnNew AosJimoCallAsyncRead(rdata,
		"AosCubeMapIDOSvr", JimoFunc::eMethodGetCubeEndpointIDs, docid, 
		gRepPolicyMgr.getCubeMapRepPolicy(), caller, gClusterMgr.getIDOStoreCluster(rdata));

	jimo_call->arg(AosFN::eReplicPolicy, replic_policy);
	jimo_call->makeCall(rdata);
*/
	OmnAlarm << enderr;
	return true;
}

/*
bool
ClusterMgrCaller::callFinished(AosRundata *rdata, AosJimoCall &jimo_call)
{
	bool rslt = jimo_call.isCallSuccess();
	if (!rslt)
	{
		mCaller->callFinished(rdata, jimo_call);
		return true;
	}

	aos_assert_rr(mEndpointIDs, rdata, false);
	rslt = jimo_call.getFieldValue(rdata, AosFN::eReturnValue, *mEndpointIDs);
	if (!rslt)
	{
		AosLogError(rdata, true, "failed_retrieving_endpoint_ids")
			<< AosFN("Endpoint IDs") << endpointIDs
			<< AosFN("Replication Policy") 
			<< AosRepPolicy::getNumSync(replic_policy) << "."
			<< AosRepPolicy::getNumAsync(replic_policy) << "."
			<< AosRepPolicy::getNumRemotes(replic_policy) << enderr;
		return false;
	}

	mCaller->callFinished(rdata, jimo_call);
}
*/
AosClusterObj * jimoGetHelloWorldCluster(AosRundata *rdata)
{
	if (!sgClusterMgr) sgInit(rdata);
	aos_assert_rr(sgClusterMgr, rdata, 0);
	return sgClusterMgr->getHelloWorldCluster(rdata);
}


};


