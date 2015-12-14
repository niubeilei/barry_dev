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
#include "ClusterMgr/ClusterMgr.h"

#include "ClusterMgr/Ptrs.h"
#include "ClusterMgr/Cluster.h"
#include "Debug/Debug.h"
#include "JimoCall/JimoCallDialer.h"
#include "Thread/Mutex.h"
#include "Thread/CondVar.h"
#include "UtilData/FN.h"
#include "UtilData/ClusterNames.h"
#include "XmlUtil/XmlTag.h"
#include "SEInterfaces/CubeMapObj.h"


// static AosCluster *sgAdminEngineCluster = 0;
// static AosCluster *sgAdminStoreCluster = 0;
// static AosCluster *sgDocEngineCluster = 0;
// static AosCluster *sgDocStoreCluster = 0;
// static AosCluster *sgIndexEngineCluster = 0;

extern "C"
{

	AosJimoPtr AosCreateJimoFunc_AosClusterMgr_1(const AosRundataPtr &rdata, const int version)
	{
		try
		{
			OmnScreen << "To create Jimo: " << endl;
			AosJimoPtr jimo = OmnNew AosClusterMgr(version);
			aos_assert_r(jimo, 0);
			return jimo;
		}

		catch (...)
		{
			AosSetErrorU(rdata, "Failed creating jimo") << enderr;
			return 0;
		}

		OmnShouldNeverComeHere;
		return 0;
	}
}


AosClusterMgr::AosClusterMgr(const int version)
:
AosClusterMgrObj(version),
// AosGenericObj(version),
mLock(OmnNew OmnMutex()),
mCondVar(OmnNew OmnCondVar())
{
	/*
	if (!sgAdminEngineCluster)
	{
		mLock->lock();
		if (!sgAdminEngineCluster)
		{
			if (!createDefaultClustersLocked())
			{
				mLock->unlock();
				OmnThrowException("failed creating default clusters");
				return;
			}
		}
		mLock->unlock();
	}
	*/
}


AosClusterMgr::~AosClusterMgr()
{
	OmnDelete mLock;
}


AosJimoPtr 
AosClusterMgr::cloneJimo() const 
{ 
	AosClusterMgrPtr thisPtr = OmnNew AosClusterMgr(mJimoVersion);
	thisPtr.getPtr()->mClusters = mClusters;
	thisPtr.getPtr()->mClusterPtrs = mClusterPtrs;
	return thisPtr;
}


bool
AosClusterMgr::addClusterPriv(
		AosRundata *rdata,
		const AosClusterObjPtr &cluster)
{
	OmnString cluster_name = cluster->getClusterName();
	if (cluster_name == "")
	{
		AosLogError(rdata, false, "missing_cluster_name") << enderr;
		return false;
	}

	cluster->setValid();

	itr_t itr = mClusters.find(cluster_name);
	if (itr != mClusters.end())
	{
		AosLogError(rdata, false, "cluster_already_exist")
			<< AosFN("Cluster Name") << cluster_name << enderr;
		return false;
	}

	mClusters[cluster_name] = cluster.getPtr();
	mClusterPtrs.push_back(cluster);
	return true;
}


AosClusterObj *
AosClusterMgr::getCluster(
		AosRundata *rdata, 
		const OmnString &cluster_name)
{
	// This function retrieves the cluster 'cluster_name'. 
	mLock->lock();
	itr_t itr = mClusters.find(cluster_name);
	if (itr != mClusters.end())
	{
		// Check whether someone is retrieving this cluster.
		if (itr->second->isRetrieving())
		{
			// Someone is retrieving the cluster.
			mCondVar->wait(mLock);
			itr = mClusters.find(cluster_name);
			if (itr == mClusters.end())
			{
				mLock->unlock();
				AosLogError(rdata, false, "internal_error") << enderr;
				return 0;
			}
		}

		mLock->unlock();
		if (!itr->second->isValid())
		{
			AosLogError(rdata, false, "internal_error") << enderr;
			return 0;
		}

		return itr->second;
	}

	// The cluster has not been loaded into memory yet. 
	
	// The cluster is not in the map. Need to retrieve it. 
	// First, create an invalid cluster.

	// Check whether the cluster is configured locally.
	OmnScreen << "To create the cluster: " << cluster_name << endl;
	AosXmlTagPtr app_confs = OmnApp::getAppConfig();
	aos_assert_rg(app_confs, rdata, 0, AosErrMsg("missing_configuration"));
	AosXmlTagPtr confs = app_confs->getFirstChild("clusters");
	if (!confs)
	{
		mLock->unlock();
		AosLogError(rdata, true, "missing_cluster_confs") << enderr;
		return false;
	}

	AosClusterObj *cluster = createClusterLocked(rdata, cluster_name, confs);
	if (cluster)
	{
		mLock->unlock();
		aos_assert_rg(cluster->isValid(), rdata, 0, AosErrMsg("failed_retrieving_cluster"));
		return cluster;
	}

	// The cluster is not defined. Try to retrieve it from 
	// the admin database. 
	AosClusterObjPtr new_cluster = OmnNew AosCluster(cluster_name);
	new_cluster->setRetrieving();
	mClusters[cluster_name] = new_cluster.getPtr();
	mClusterPtrs.push_back(new_cluster);
	mLock->unlock();

	bool rslt = new_cluster->retrieveFromAdmin(rdata);
	aos_assert_rg(rslt, rdata, 0, AosErrMsg("failed_retrieving_cluster"));
	aos_assert_rg(new_cluster->isValid(), rdata, 0, AosErrMsg("failed_retrieving_cluster"));
	return new_cluster.getPtrNoLock();
}

	
/*
bool 
AosClusterMgr::createDefaultClustersLocked()
{
	// This function assumes the configuration defines the Admin
	// cluster. First, it creates AOS_CLUSTERNAME_ADMIN_ENGINE cluster.
	AosXmlTagPtr config = OmnApp::getAppConfig();
	if (!config)
	{
		OmnAlarm << "Missing configuration" << enderr;
		return false;
	}

	AosRundataPtr app_rdata = OmnApp::getRundata();
	aos_assert_r(app_rdata, false);
	AosRundata *rdata = app_rdata.getPtr();

	sgAdminEngineCluster = createClusterLocked(rdata, AOS_CLUSTERNAME_ADMINENGINE, config);
	sgAdminStoreCluster  = createClusterLocked(rdata, AOS_CLUSTERNAME_ADMINSTORE, config);
	sgDocEngineCluster   = createClusterLocked(rdata, AOS_CLUSTERNAME_DOCENGINE, config);
	sgDocStoreCluster    = createClusterLocked(rdata, AOS_CLUSTERNAME_DOCSTORE, config);
	sgIndexEngineCluster = createClusterLocked(rdata, AOS_CLUSTERNAME_INDEXENGINE, config);

	createClusterLocked(rdata, AOS_CLUSTERNAME_IDOSTORE, config);

	return true;
}
*/


AosClusterObj *
AosClusterMgr::createClusterLocked(
		AosRundata *rdata,
		const OmnString &cluster_name, 
		const AosXmlTagPtr &config)
{
	// This function creates a cluster from the configuration.
	// When the product is stable, it should create the cluster
	// by retrieving the configuration from the admin database. 
	AosXmlTagPtr tag = config->getFirstChild(cluster_name);
	if (!tag)
	{
		AosLogError(rdata, false, "missing_cluster_tag") 
			<< AosFN("Cluster Name") << cluster_name << enderr;
		return 0;
	}

	try
	{
		AosCluster *cluster = OmnNew AosCluster(rdata, tag);
		if (!cluster)
		{
			AosLogError(rdata, false, "failed_creating_cluster") << enderr;
			return 0;
		}

		if (cluster->getClusterName() != cluster_name)
		{
			AosLogError(rdata, false, "invalid_cluster_name") << enderr;
			return 0;
		}

		addClusterPriv(rdata, cluster);
		return cluster;
	}

	catch (...)
	{
		AosLogError(rdata, false, "internal_error") << enderr;
		return 0;
	}
}


AosClusterObj *
AosClusterMgr::getJimoCluster(AosRundata *rdata)
{
	return getCluster(rdata, AOS_CLUSTERNAME_JIMO);
}


AosClusterObj *
AosClusterMgr::getAdminEngineCluster(AosRundata *rdata)
{
	return getCluster(rdata, AOS_CLUSTERNAME_ADMINENGINE);
}


AosClusterObj *
AosClusterMgr::getAdminStoreCluster(AosRundata *rdata)
{
	return getCluster(rdata, AOS_CLUSTERNAME_ADMINSTORE);
}


AosClusterObj *
AosClusterMgr::getDocEngineCluster(AosRundata *rdata)
{
	return getCluster(rdata, AOS_CLUSTERNAME_DOCENGINE);
}


AosClusterObj *
AosClusterMgr::getDocEngineClusterAsync(AosRundata *rdata)
{
	return getCluster(rdata, AOS_CLUSTERNAME_DOCENGINE_ASYNC);
}


AosClusterObj *
AosClusterMgr::getDocStoreCluster(AosRundata *rdata)
{
	return getCluster(rdata, AOS_CLUSTERNAME_DOCSTORE);
}


AosClusterObj *
AosClusterMgr::getDocStoreClusterAsync(AosRundata *rdata)	//by White, 2015-08-28 14:27:01
{
	return getCluster(rdata, AOS_CLUSTERNAME_DOCSTORE_ASYNC);
}


AosClusterObj *
AosClusterMgr::getIndexEngineCluster(AosRundata *rdata)
{
	return getCluster(rdata, AOS_CLUSTERNAME_INDEXENGINE);
}


AosClusterObj *
AosClusterMgr::getIndexStoreCluster(AosRundata *rdata)
{
	return getCluster(rdata, AOS_CLUSTERNAME_INDEXSTORE);
}


AosClusterObj *
AosClusterMgr::getQueryEngineCluster(AosRundata *rdata)
{
	return getCluster(rdata, AOS_CLUSTERNAME_QUERYENGINE);
}


AosClusterObj *
AosClusterMgr::getQueryStoreCluster(AosRundata *rdata)
{
	return getCluster(rdata, AOS_CLUSTERNAME_QUERYSTORE);
}


AosClusterObj *
AosClusterMgr::getStreamEngineCluster(AosRundata *rdata)
{
	return getCluster(rdata, AOS_CLUSTERNAME_STREAMENGINE);
}


AosClusterObj *
AosClusterMgr::getStreamStoreCluster(AosRundata *rdata)
{
	return getCluster(rdata, AOS_CLUSTERNAME_STREAMSTORE);
}


AosClusterObj *
AosClusterMgr::getIDOCluster(AosRundata *rdata)
{
	return getCluster(rdata, AOS_CLUSTERNAME_IDOSTORE);
}


AosCubeMapObj *
AosClusterMgr::createCubeMap(AosRundata*, const AosXmlTagPtr&)
{
	OmnAlarm << enderr;
	return 0;
}


AosClusterObj *
AosClusterMgr::getHelloWorldCluster(AosRundata *rdata)
{
	return getCluster(rdata, AOS_CLUSTERNAME_HELLOWORLD);
}


bool 
AosClusterMgr::run(const AosRundataPtr&)
{
	OmnShouldNeverComeHere;
	return false;
}

