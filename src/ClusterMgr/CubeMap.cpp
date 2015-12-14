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
// CubeMap is implemented through an In-memory Data Object (IDO). An 
// IDO is made of two objects: Client Object and Server Object. The 
// client object implements the API calls and the server object implements
// the logic. 
//
// Modification History:
// 2015/03/12 Created by Ketty Guo
////////////////////////////////////////////////////////////////////////////
#include "ClusterMgr/CubeMap.h"

#include "API/AosApi.h"
#include "JimoAPI/JimoIDO.h"
#include "JimoAPI/JimoClusterMgr.h"
#include "JimoAPI/JimoRepPolicyMgr.h"
#include "JimoCall/JimoCallSyncRead.h"
#include "SEInterfaces/ClusterObj.h"
#include "XmlUtil/XmlTag.h"

using namespace Jimo;

AosCubeMap::AosCubeMap(
		AosRundata *rdata, 
		const AosXmlTagPtr &conf,
		const int num_endpoints)
:
mMayUseLocalCopy(true),
mIdoDocid(0),
mCubeIdx(0),
mLock(OmnNew OmnMutex()),
mCondVar(OmnNew OmnCondVar())
{
	if (!config(rdata, conf, num_endpoints))
	{
		OmnThrowException("failed_creating_cube_map");
		return;
	}
}


AosCubeMap::~AosCubeMap()
{
	OmnDelete mCondVar;
	OmnDelete mLock;
}


bool
AosCubeMap::config(AosRundata *rdata, const AosXmlTagPtr &conf, const int num_endpoints)
{
	//    <cube_map docid="5000">
	//        <cube cube_id="0">
	//            <physical id="1" status="1"/>
	//            <physical id="2" status="1"/>
	//        </cube>
	//        <cube cube_id="1">
	//            <physical id="1" status="1"/>
	//            <physical id="2" status="1"/>
	//        </cube>
	//        <cube cube_id="2">
	//            <physical id="2" status="1"/>
	//            <physical id="1" status="1"/>
	//        </cube>
	//        <cube cube_id="3">
	//            <physical id="2" status="1"/>
	//            <physical id="1" status="1"/>
	//        </cube>
	//    </cube_map>

	aos_assert_rr(conf, rdata, false);

	mDocid = conf->getAttrU64("docid", 0);
	//mNumCubes = conf->getAttrU64("num_cubes", 20);
	//resize to a default value at the beginning
	mCubes.resize(20);

	// Create the cube map
	AosXmlTagPtr tag = conf->getFirstChild("cube");
	if (!tag)
	{
		AosLogError(rdata, true, "missing_cubes_tag") 
			<< AosFN("Config") << conf << enderr;
		return false;
	}

	int cube_id = 0;
	int count = 0;
	int maxCubeId = 0;
	while (tag)
	{
		//comment out checking. 
		//Permit incontinuous cube_id for now. Phil 07/19
#if 0
		int cid = tag->getAttrInt("cube_id", -1);
		if (cid != cube_id)
		{
			AosLogError(rdata, true, "cube_id_incorrect")
				<< AosFN("Expecting Cube ID") << cube_id
				<< AosFN("Actual Cube ID") << cid 
				<< AosFN("Current Tag") << tag
				<< AosFN("Config") << conf << enderr;
			return false;
		}
#endif

		cube_id = tag->getAttrInt("cube_id", -1);
		u8 dupid = 0;
		vector<CubeInfo> cube_infos;
		AosXmlTagPtr cube_info_tag = tag->getFirstChild("physical");
		while(cube_info_tag)
		{
			int physical_id = cube_info_tag->getAttrInt("id", -1);
			if( physical_id < 0 || physical_id >= num_endpoints)
			{
				AosLogError(rdata, true, "physical_id_incorrect")
					<< AosFN("Expecting physical ID") << physical_id
					<< AosFN("Current Tag") << cube_info_tag
					<< AosFN("Config") << conf << enderr;
				return false;
			}
			Status status = (Status)(cube_info_tag->getAttrInt("status", 0));
			if( status == eInvalid )
			{
				AosLogError(rdata, true, "status_incorrect")
					<< AosFN("Expecting physical ID") << physical_id
					<< AosFN("Actual status") << status
					<< AosFN("Current Tag") << cube_info_tag
					<< AosFN("Config") << conf << enderr;
				return false;
			}

			CubeInfo cube_info(cube_id, physical_id, dupid++, status);
			cube_infos.push_back(cube_info);

			cube_info_tag = tag->getNextChild();
		}

		/*
		OmnString ss = tag->getNodeText();
		vector<OmnString> terms;
		int nn = AosSplitStr(ss, ",", terms, eMaxCubeBackups);
		aos_assert_rr(nn < eMaxCubeBackups, rdata, false);

		vector<int> cube_ids;
		for (u32 k=0; k<terms.size(); k++)
		{
			int mm = atoi(terms[cube_id].data());
			if (mm < 0 || mm >= eMaxCubeBackups)
			{
				AosLogError(rdata, true, "invalid_cube_id")
					<< AosFN("Cube ID") << terms[cube_id]
					<< AosFN("Current Tag") << tag
					<< AosFN("Config") << conf << enderr;
				return true;
			}
			cube_ids.push_back(mm);
		}
		*/

		//change the following code   07/19 Phil
		//Need to resize the cube_infos so that cube_ids
		//in the cube_infos vector can be incontinous
		//mCubes.push_back(cube_infos);
		if (mCubes.size() <= cube_id)
			mCubes.resize(cube_id + 1);

		mCubes[cube_id] = cube_infos;

		if (maxCubeId < cube_id)
			maxCubeId = cube_id;

		count++;

		tag = conf->getNextChild();
		//comment out for now. Phil 07/19
		//cube_id++;
	}

	//check if mCubes is continuous
	if (count != maxCubeId + 1)
	{
		//report error
		AosLogError(rdata, true, "cube id is not continuous")
			<< AosFN("Config") << conf << enderr;
		return false;
	}

	//remove extra cube space
	if (count < mCubes.size())
		mCubes.resize(count);

	return true;
}


bool
AosCubeMap::getTargetCubes(
		AosRundata *rdata, 
		const u32 cube_id, 
		vector<CubeInfo> &cube_infos)
{
	// This function retrieves enough backups for the given 
	// cube 'cube_id'.
	cube_infos.clear();
	if (!mMayUseLocalCopy)
	{
		bool rslt = updateFromIDO(rdata);
		aos_assert_rr(rslt, rdata, false);
		mMayUseLocalCopy = true;
	}

	aos_assert_rr(cube_id < mCubes.size(), rdata, false);
	cube_infos = mCubes[cube_id];

	return true;
}


bool 
AosCubeMap::getTargetCubes(
		AosRundata *rdata, 
		vector<CubeInfo> &cube_infos) 
{
	cube_infos.clear();
	if (!mMayUseLocalCopy)
	{
		bool rslt = updateFromIDO(rdata);
		aos_assert_rr(rslt, rdata, false);
		mMayUseLocalCopy = true;
	}

	mLock->lock();
	if(mCubeIdx>= mCubes.size()) mCubeIdx = 0;
	const vector<CubeInfo> &eids = mCubes[mCubeIdx++];
	mLock->unlock();

	u32 nn = eids.size();

	u32 idx = 0;
	while (idx < nn)
	{
		switch (eids[idx].status)
		{
		case eActive:
			 cube_infos.push_back(eids[idx]);
			 break;

		case eWriteOnly:
			 cube_infos.push_back(eids[idx]);
			 break;

		case eSuspended:
			 break;

		default:
			 AosLogError(rdata, false, "internal_error") << enderr;
			 return false;
		}

		idx++;
	}

	return true;
}


AosJimoPtr
AosCubeMap::cloneJimo() const
{
	return OmnNew AosCubeMap(*this);
}


AosCubeMapObjPtr 
AosCubeMap::createCubeMap(AosRundata *rdata, const AosXmlTagPtr &def)
{
	try
	{
		return OmnNew AosCubeMap(rdata, def, 1);
	}

	catch (...)
	{
		AosLogError(rdata, true, "failed_creating_cubemap")
			<< AosFN("Config") << def << enderr;
		return 0;
	}
}


AosCubeMapObjPtr 
AosCubeMap::createCubeMap(AosRundata *rdata, AosBuff *buff)
{
	try
	{
		AosIDOPtr ido = AosIDO::createIDO(rdata, buff);
		aos_assert_rr(ido, rdata, 0);
		AosCubeMapObjPtr cubemap = dynamic_cast<AosCubeMapObj *>(ido.getPtr());
		aos_assert_rr(cubemap, rdata, 0);
		return cubemap;
	}

	catch (...)
	{
		AosLogError(rdata, false, "internal_error") << enderr;
		return 0;
	}
}


bool 
AosCubeMap::updateFromIDO(AosRundata *rdata)
{
	AosClusterObj *cluster = jimoGetIDOCluster(rdata);
	aos_assert_rr(cluster, rdata, 0);

	u32 distr_id = cluster->getDistrID(rdata, mIdoDocid);
	AosJimoCallPtr jimo_call = OmnNew AosJimoCallSyncRead(rdata,
		"AosCubeMapSvr", AosCubeMapObj::eMethodGetByDocid,
		distr_id, gRepPolicyMgr.getCubeMapRepPolicy(), cluster);

	jimo_call->arg(AosFN::eDocid, mIdoDocid);

	jimo_call->makeCall(rdata);

	if (!jimo_call->isCallSuccess())
	{
		// Did not create. 
		OmnString errmsg = jimo_call->getOmnStr(rdata, AosFN::eErrmsg, "");
		if (errmsg != "")
		{
			AosLogError(rdata, false,  "failed_retrieving_cube_map")
				<< AosFN("mIdoDocid") << mIdoDocid 
				<< AosFN("Error Message") << errmsg << enderr;
		}
		else
		{
			AosLogError(rdata, false,  "failed_retrieving_cube_map")
				<< AosFN("mIdoDocid") << mIdoDocid << enderr;
		}

		return false;
	}

	AosBuff *buff = jimo_call->getBuffRaw(rdata, AosFN::eBuff, 0);
	aos_assert_rr(buff, rdata, false);

	u64 docid = mIdoDocid;
	serializeFrom(rdata, buff);
	aos_assert_rr(mIdoDocid == docid, rdata, false);
	return true;
}


// Chen Ding, 2015/07/16
bool
AosCubeMap::getCubeGroup(
		AosRundata *rdata, 
		const u32 cube_id, 
		vector<CubeInfo> &cubes) 
{
	cubes.clear();
	if (!mMayUseLocalCopy)
	{
		bool rslt = updateFromIDO(rdata);
		aos_assert_rr(rslt, rdata, false);
		mMayUseLocalCopy = true;
	}

	aos_assert_rr(cube_id < mCubes.size(), rdata, false);
	cubes = mCubes[cube_id];

	return true;
}


bool 
AosCubeMap::getHostedCubes(
		AosRundata *rdata, 
		vector<AosCubeMapObj::CubeInfo> &cubes)
{
	// This function retrieves all the cubes that are hosted by this server.
	int self_phyid = AosGetSelfServerId();
	cubes.clear();
	for (u32 i=0; i<mCubes.size(); i++)
	{
		vector<AosCubeMapObj::CubeInfo> &cc = mCubes[i];
		for (u32 j=0; j<cc.size(); j++)
		{
			if (cc[j].physical_id == self_phyid)
			{
				cubes.push_back(cc[j]);
				break;
			}
		}
	}

	return true;
}


bool
AosCubeMap::updateLeadership(
		AosRundata			*rdata,
		const u32			cube_id,
		const i32			leader_id,	//wish i32 shall be not enough for our system, ^_^
		vector<CubeInfo>	&cube_infos)
{
	if (-1 == leader_id)	//means the cluster does not have a leader, just retry
	{
		return true;
	}
	mLock->lock();
	OmnScreen << "updating leadership, new leader id:" << leader_id << endl;
	OmnString s = "";
	for (vector<CubeInfo>::iterator it = cube_infos.begin(); it != cube_infos.end(); it++)
	{
		s << "cubeid:" << it->cube_id << " physicalid:" << it->physical_id << " dupid:" << it->dupid
				<< " status:" << it->status << "\n";
	}
	OmnScreen << "cube_info before updating\n" << s << endl;
	for (vector<CubeInfo>::iterator it = cube_infos.begin(); it != cube_infos.end(); it++)
	{
		if (leader_id == it->physical_id)
		{
			CubeInfo sCubeInfo(it->cube_id, it->physical_id, it->dupid, it->status);
			cube_infos.erase(it);
			cube_infos.insert(cube_infos.begin(), sCubeInfo);
			break;
		}
	}
	if (cube_id >= mCubes.size())
	{
		OmnAlarm << "cube_id:" << cube_id << " >= mCubes.size():" << mCubes.size() << enderr;
		mLock->unlock();
		return false;
	}
	mCubes[cube_id] = cube_infos;
	s = "";
	for (vector<CubeInfo>::iterator it = cube_infos.begin(); it != cube_infos.end(); it++)
	{
		s << "cubeid:" << it->cube_id << " physicalid:" << it->physical_id << " dupid:" << it->dupid
				<< " status:" << it->status << "\n";
	}
	OmnScreen << "cube_info after updating\n" << s << endl;
	mLock->unlock();
	return true;
}


bool
AosCubeMap::putTimeoutEndpointToEnd(
		AosRundata			*rdata,
		const u32			cube_id,
		const i32			endpoint_id,	//I wish i32 shall be not enough for our system, ^_^
		vector<CubeInfo>	&cube_infos)
{
	mLock->lock();
	OmnScreen << "putting timeout endpoint to end, id:" << endpoint_id << endl;
	if (1 == cube_infos.size())
	{
		OmnScreen << "there is only one endpoint and it times out, be warned." << endl;
		mLock->unlock();
		return true;
	}
	OmnString s = "";
	for (vector<CubeInfo>::iterator it = cube_infos.begin(); it != cube_infos.end(); it++)
	{
		s << "cubeid:" << it->cube_id << " physicalid:" << it->physical_id << " dupid:" << it->dupid
				<< " status:" << it->status << "\n";
	}
	OmnScreen << "cube_info before updating\n" << s << endl;
	for (vector<CubeInfo>::iterator it = cube_infos.begin(); it != cube_infos.end(); it++)
	{
		if (endpoint_id == it->physical_id)
		{
			CubeInfo sCubeInfo(it->cube_id, it->physical_id, it->dupid, it->status);
			cube_infos.erase(it);
			cube_infos.push_back(sCubeInfo);
			break;
		}
	}
	if (cube_id >= mCubes.size())
	{
		OmnAlarm << "cube_id:" << cube_id << " >= mCubes.size():" << mCubes.size() << enderr;
		mLock->unlock();
		return false;
	}
	mCubes[cube_id] = cube_infos;
	s = "";
	for (vector<CubeInfo>::iterator it = cube_infos.begin(); it != cube_infos.end(); it++)
	{
		s << "cubeid:" << it->cube_id << " physicalid:" << it->physical_id << " dupid:" << it->dupid
				<< " status:" << it->status << "\n";
	}
	OmnScreen << "cube_info after updating\n" << s << endl;
	mLock->unlock();
	return true;
}
