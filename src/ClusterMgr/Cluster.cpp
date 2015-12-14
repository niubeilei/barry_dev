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
#include "ClusterMgr/Cluster.h"

#include "ClusterMgr/Ptrs.h"
#include "ClusterMgr/CubeMap.h"
#include "JimoAPI/JimoDocEngine.h"
#include "JimoCall/JimoCall.h"
#include "JimoCall/JimoCallDialer.h"
#include "UtilData/SysTableNames.h"
#include "Util/UtUtil.h"
#include "XmlUtil/XmlTag.h"


using namespace Jimo;

AosCluster::AosCluster(const OmnString &cluster_name)
:
mClusterName(cluster_name),
mIsRetrieving(false),
mIsValid(false),
mCondVar(0),
mLock(0),
mCubeMap(0),
mNumCubes(-1)
{
}


AosCluster::AosCluster(AosRundata *rdata, const AosXmlTagPtr &conf)
:
mIsRetrieving(false),
mIsValid(false),
mCondVar(0),
mLock(0),
mCubeMap(0),
mNumCubes(-1)
{
	if (!config(rdata, conf))
	{
		OmnThrowException(rdata->getErrmsg());
		return;
	}
}


AosCluster::~AosCluster()
{
	OmnDelete mCondVar;
	OmnDelete mLock;
	OmnDelete mCubeMap;
}


bool
AosCluster::config(AosRundata *rdata, const AosXmlTagPtr &conf)
{
//	mNumCubes = conf->getAttrInt("num_cubes", -1);
//	if (mNumCubes <= 0)
//	{
//		AosLogError(rdata, true, "invalid_num_cubes")
//			<< AosFN("Config") << conf << enderr;
//		return false;
//	}
//	OmnScreen << "mNumCubes:" << mNumCubes << endl;
	mClusterName = conf->getAttrStr("name");
	if (mClusterName == "")
	{
		AosLogError(rdata, true, "missing_cluster_name") << enderr;
		return false;
	}

	// Configure Endpoints
	AosXmlTagPtr endpoints = conf->getFirstChild("physicals");
	if (!endpoints)
	{
		AosLogError(rdata, true, "missing_endpoints") 
			<< AosFN("Config") << conf->toString() << enderr;
		return false;
	}

	AosXmlTagPtr tag = endpoints->getFirstChild();
	if (!tag)
	{
		AosLogError(rdata, true, "missing_endpoints") 
			<< AosFN("Config") << conf->toString() << enderr;
		return false;
	}

	while (tag)
	{
		AosEndPointInfo info;
		if (!info.init(rdata, tag))
		{
			AosLogError(rdata, true, "invalid_physical_config") 
				<< AosFN("Config") << conf->toString() << enderr;
			return false;
		}

		mEndpoints.push_back(info);
		tag = endpoints->getNextChild();
	}

	tag = conf->getFirstChild("cube_map");
	if (!tag)
	{
		AosLogError(rdata, true, "missing_cube_map") 
			<< AosFN("Config") << conf->toString() << enderr;
		return false;
	}

	try
	{
		mCubeMap = OmnNew AosCubeMap(rdata, tag, mEndpoints.size());
	}

	catch (...)
	{
		AosLogError(rdata, false, "failed_config_cube_map") << enderr;
		return false;
	}

	// Create the dialer
	AosXmlTagPtr dialer_tag = conf->getFirstChild("dialer");
	if (!dialer_tag)
	{
		AosLogError(rdata, true, "missing_dialer_config")
			<< AosFN("Config") << conf << enderr;
		return false;
	}
	mDialer = AosJimoCallDialer::createDialer(rdata, dialer_tag);
	aos_assert_rr(mDialer, rdata, false);

	return true;
}


AosClusterPtr
AosCluster::retrieveCluster(
		AosRundata *rdata, 
		const OmnString &cluster_name)
{
	OmnAlarm << enderr;
	return 0;
/*
	AosXmlTagPtr doc;
	bool rslt = gJimoDocEngine.getDocByName(rdata, doc, 
					AOS_SYSTABLENAME_CLUSTERS, AOSTAG_NAME, cluster_name);
	aos_assert_rr(rslt, rdata, 0);
	if (!doc)
	{
		AosLogError(rdata, true, "cluster_not_found")
			<< AosFN("Cluster Name") << cluster_name << enderr;
		return 0;
	}

	try
	{
		AosClusterPtr cluster = OmnNew AosCluster(rdata, doc);
		aos_assert_rr(cluster, rdata, 0);
		return cluster;
	}

	catch (...)
	{
		AosLogError(rdata, false, "internal_error") << enderr;
		return 0;
	}
*/
}


int
AosCluster::calculateCubeID(const u64 docid) const
{
//	aos_assert_r(mNumCubes > 0, -1);
//	return docid % mNumCubes;
}


int 
AosCluster::calculateCubeID(const OmnString &objid) const
{
//	u64 nn = AosStrHashFunc(objid.data(), objid.length());
//	aos_assert_r(mNumCubes > 0, -1);
//	return nn % mNumCubes;
}


bool 
AosCluster::getTargetCubes(
		AosRundata *rdata, 
		const i64 cube_id, 
		vector<AosCubeMapObj::CubeInfo> &cube_infos) const
{
	if (-1 == cube_id)
	{
		return mCubeMap->getTargetCubes(rdata, cube_infos);
	}
	else
	{
		return mCubeMap->getTargetCubes(rdata, cube_id, cube_infos);
	}
}


bool 
AosCluster::getTargetCubes(
		AosRundata *rdata, 
		vector<AosCubeMapObj::CubeInfo> &cube_infos) const
{
	// This function uses either objid or docid to determine where
	// the requested resides. 
	return mCubeMap->getTargetCubes(rdata, cube_infos);
}


u64 
AosCluster::nextJimoCallID(AosRundata *rdata)
{
	aos_assert_rr(mDialer, rdata, 0);
	return mDialer->nextJimoCallID(rdata);
}


bool 
AosCluster::eraseJimoCall(AosRundata *rdata, const u64 call_id)
{
	aos_assert_rr(mDialer, rdata, false);
	mDialer->eraseJimoCall(call_id);
	return true;
}


bool 
AosCluster::makeJimoCall(
		AosRundata *rdata, 
		const int endpoint_id,
		const AosJimoCallPtr &jimo_call, 
		AosBuff *buff)
{
	aos_assert_rr(endpoint_id >= 0 && (u32)endpoint_id < mEndpoints.size(), rdata, false);
	aos_assert_rr(mDialer, rdata, false);
	return mDialer->makeJimoCall(rdata, mEndpoints[endpoint_id], jimo_call, buff);
}


bool 
AosCluster::modifyCubeAllocation(
		AosRundata *rdata, 
		const int cube_id, 
		const vector<AosCubeMapObj::CubeInfo> &endpoint_ids)
{
	OmnNotImplementedYet;
	return false;
}


int 
AosCluster::getNumCubes() const
{
	return mCubeMap->getNumCubes();
}


bool
AosCluster::retrieveFromAdmin(AosRundata *rdata)
{
	OmnNotImplementedYet;
	return false;
}


bool 
AosCluster::getHostedCubes(
		AosRundata *rdata, 
		vector<AosCubeMapObj::CubeInfo> &cubes)
{
	return mCubeMap->getHostedCubes(rdata, cubes);
}


bool 
AosCluster::getCubeGroup(AosRundata *rdata, 
		const u32 cube_id,
		vector<AosCubeMapObj::CubeInfo> &cubes)
{
	// This function returns the cube group for the given cube ID: cube_id
	return mCubeMap->getCubeGroup(rdata, cube_id, cubes);
}


bool
AosCluster::updateLeadership(
		AosRundata						*rdata,
		const u32						cube_id,
		const i32						leader_id,	//wish i32 shall be not enough for our system, ^_^
		vector<AosCubeMapObj::CubeInfo> &cube_infos)
{
	return mCubeMap->updateLeadership(rdata, cube_id, leader_id, cube_infos);
}


bool
AosCluster::putTimeoutEndpointToEnd(
		AosRundata						*rdata,
		const u32						cube_id,
		const i32						endpoint_id,	//I wish i32 shall be not enough for our system, ^_^
		vector<AosCubeMapObj::CubeInfo>	&cube_infos)
{
	return mCubeMap->putTimeoutEndpointToEnd(rdata, cube_id, endpoint_id, cube_infos);
}
