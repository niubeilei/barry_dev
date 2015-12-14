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
// Modification History:
// 2015/03/13 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_SEInterfaces_ClusterMgrObj_h
#define Aos_SEInterfaces_ClusterMgrObj_h

#include "Jimo/Jimo.h"
#include "SEInterfaces/Ptrs.h"
#include "SEInterfaces/JimoCaller.h"
#include "SEInterfaces/ClusterObj.h"
#include "SEInterfaces/CubeMapObj.h"
#include "Util/RCObject.h"

class AosRundata;

class AosClusterMgrObj : virtual public AosJimo
{
private:
	
public:
	AosClusterMgrObj(const int version);

	static void setClusterMgr(const AosClusterMgrObjPtr &cluster);
	static AosClusterMgrObj * getClusterMgr(AosRundata *rdata);

	virtual AosClusterObj *getCluster(AosRundata *rdata, const OmnString &cluster_name) = 0;
	virtual AosCubeMapObj *createCubeMap(AosRundata *rdata, const AosXmlTagPtr &conf) = 0;

	virtual AosClusterObj *getJimoCluster(AosRundata *rdata) = 0;

	virtual AosClusterObj *getAdminEngineCluster(AosRundata *rdata) = 0;
	virtual AosClusterObj *getAdminStoreCluster(AosRundata *rdata) = 0;

	virtual AosClusterObj *getDocEngineCluster(AosRundata *rdata) = 0;
	virtual AosClusterObj *getDocEngineClusterAsync(AosRundata *rdata) = 0;
	virtual AosClusterObj *getDocStoreCluster(AosRundata *rdata) = 0;
	virtual AosClusterObj *getDocStoreClusterAsync(AosRundata *rdata) = 0;	//by White, 2015-08-28 14:27:01

	virtual AosClusterObj *getIndexEngineCluster(AosRundata *rdata) = 0;
	virtual AosClusterObj *getIndexStoreCluster(AosRundata *rdata) = 0;

	virtual AosClusterObj *getQueryEngineCluster(AosRundata *rdata) = 0;
	virtual AosClusterObj *getQueryStoreCluster(AosRundata *rdata) = 0;

	virtual AosClusterObj *getStreamEngineCluster(AosRundata *rdata) = 0;
	virtual AosClusterObj *getStreamStoreCluster(AosRundata *rdata) = 0;

	virtual AosClusterObj *getIDOCluster(AosRundata *rdata) = 0;

	virtual AosClusterObj *getHelloWorldCluster(AosRundata *rdata) = 0;

};
#endif

