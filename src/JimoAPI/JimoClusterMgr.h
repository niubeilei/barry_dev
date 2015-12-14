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
#ifndef Aos_JimoAPI_JimoClusterMgr_h
#define Aos_JimoAPI_JimoClusterMgr_h

#include "SEInterfaces/Ptrs.h"
#include "Util/String.h"
#include "UtilData/RepPolicy.h"
#include "XmlUtil/Ptrs.h"
#include "JimoCall/JimoCall.h"

#include <vector>

class AosRundata;

namespace Jimo
{

	extern AosClusterObj *jimoGetCluster(AosRundata *rdata, const OmnString &cluster_name);

	extern AosClusterObj *jimoGetJimoCluster(AosRundata *rdata);

	extern AosClusterObj *jimoGetAdminEngineCluster(AosRundata *rdata);
	extern AosClusterObj *jimoGetAdminStoreCluster(AosRundata *rdata);

	extern AosClusterObj *jimoGetDocEngineCluster(AosRundata *rdata);
	extern AosClusterObj *jimoGetDocEngineClusterAsync(AosRundata *rdata);
	extern AosClusterObj *jimoGetDocStoreCluster(AosRundata *rdata);
	extern AosClusterObj *jimoGetDocStoreClusterAsync(AosRundata *rdata);	//by White, 2015-08-28 14:27:01

	extern AosClusterObj *jimoGetIndexEngineCluster(AosRundata *rdata);
	extern AosClusterObj *jimoGetIndexStoreCluster(AosRundata *rdata);

	extern AosClusterObj *jimoGetQueryEngineCluster(AosRundata *rdata);
	extern AosClusterObj *jimoGetQueryStoreCluster(AosRundata *rdata);

	extern AosClusterObj *jimoGetStreamEngineCluster(AosRundata *rdata);
	extern AosClusterObj *jimoGetStreamStoreCluster(AosRundata *rdata);

	extern AosClusterObj *jimoGetLogEngineCluster(AosRundata *rdata);
	extern AosClusterObj *jimoGetLogEngineClusterAsync(AosRundata *rdata);
	extern AosClusterObj *jimoGetLogStoreCluster(AosRundata *rdata);

	extern AosClusterObj *jimoGetIDOCluster(AosRundata *rdata);
	extern AosCubeMapObj *jimoRetrieveCubeMap(AosRundata *rdata, const u64 docid);

	extern AosCubeMapObj *jimoCreateCubeMap(AosRundata *rdata, const AosXmlTagPtr &def);

	extern bool jimoGetCubeEndpointIDs(AosRundata *rdata, 
						const u32 cube_id, 
						const u64 docid,
						vector<int> &endpoint_ids,
						const u32 replic_policy);

	extern bool jimoGetCubeEndpointIDs(AosRundata *rdata, 
						const u32 cube_id, 
						const u64 docid,
						vector<int> &endpoint_ids,
						const AosRepPolicy &replic_policy, 
						const AosJimoCallerPtr &caller);

	extern bool jimoGetBackupCubes(AosRundata *rdata, 
						const u32 cube_id,
						vector<int> &endpoint_ids);

	extern AosClusterObj *jimoGetHelloWorldCluster(AosRundata *rdata);

};
#endif

