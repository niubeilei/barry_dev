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
#ifndef Aos_DataComm_ClusterMgr_h
#define Aos_DataComm_ClusterMgr_h

#include "ClusterMgr/Ptrs.h"
#include "SEInterfaces/Ptrs.h"
#include "SEInterfaces/ClusterMgrObj.h"
#include "SEInterfaces/GenericObj.h"
#include "Util/HashUtil.h"

class OmnMutex;
class OmnCondVar;

class AosClusterMgr : virtual public AosClusterMgrObj
{
	OmnDefineRCObject;

private:
	typedef hash_map<const OmnString, AosClusterObj*, Omn_Str_hash, compare_str> map_t;
	typedef hash_map<const OmnString, AosClusterObj*, Omn_Str_hash, compare_str>::iterator itr_t;

	OmnMutex *		mLock;
	OmnCondVar *	mCondVar;
	map_t			mClusters;
	vector<AosClusterObjPtr> mClusterPtrs;

public:
	AosClusterMgr(const int version);
	~AosClusterMgr();

	// Jimo Interface
	virtual bool run(const AosRundataPtr&);
	// virtual bool run(const AosRundataPtr &rdata, 
	 // 						const AosXmlTagPtr &worker_doc);
	virtual AosJimoPtr cloneJimo() const;
	virtual bool config(	const AosRundataPtr &rdata, 
							const AosXmlTagPtr &worker_doc,
							const AosXmlTagPtr &jimo_doc) { return true; }
	virtual void* getMethod(const AosRundataPtr &rdata, 
							const char *name, 
							AosMethodId::E &method_id) { return false; }
	virtual bool serializeTo(
							const AosRundataPtr &rdata, 
							AosBuff *buff){ return false; }
	virtual bool serializeFrom(
							const AosRundataPtr &rdata, 
							AosBuff *buff){ return false; }
	virtual bool queryInterface(
							const AosRundataPtr &rdata, 
							const char *interface_objid) const { return false; }

	virtual AosClusterObj *getCluster(AosRundata *rdata, const OmnString &cluster_name);

	virtual AosClusterObj *getJimoCluster(AosRundata *rdata);

	virtual AosClusterObj *getAdminEngineCluster(AosRundata *rdata);
	virtual AosClusterObj *getAdminStoreCluster(AosRundata *rdata);

	virtual AosClusterObj *getDocEngineCluster(AosRundata *rdata);
	virtual AosClusterObj *getDocEngineClusterAsync(AosRundata *rdata);
	virtual AosClusterObj *getDocStoreCluster(AosRundata *rdata);
	virtual AosClusterObj *getDocStoreClusterAsync(AosRundata *rdata);	//by White, 2015-08-28 14:27:01

	virtual AosClusterObj *getIndexEngineCluster(AosRundata *rdata);
	virtual AosClusterObj *getIndexStoreCluster(AosRundata *rdata);

	virtual AosClusterObj *getQueryEngineCluster(AosRundata *rdata);
	virtual AosClusterObj *getQueryStoreCluster(AosRundata *rdata);

	virtual AosClusterObj *getStreamEngineCluster(AosRundata *rdata);
	virtual AosClusterObj *getStreamStoreCluster(AosRundata *rdata);

	virtual AosClusterObj *getIDOCluster(AosRundata *rdata);

	virtual AosCubeMapObj *createCubeMap(AosRundata*, const AosXmlTagPtr&);

	virtual AosClusterObj *getHelloWorldCluster(AosRundata *rdata);

private:
	bool addClusterPriv(AosRundata *rdata, const AosClusterObjPtr &cluster);
	AosClusterObj * createClusterLocked(
							AosRundata *rdata, 
							const OmnString &cluster_name, 
							const AosXmlTagPtr &config);
};
#endif
