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
// In Release 3.1, DocFileMgrNew is implemented through DataletSE. 
// DocFileMgrNew (this class) becomes a wrapper to it. Each 
// DocFileMgrNew is identified by DFMID. DFMID identifies the 
// instance of DataletSE. DatalsetSEs are managed by Raft. 
//
// There can be multiple DocFileMgrNew instances. All share the 
// same Raft. 
//
// This instance runs on DocEngine. It packages requests and sends them
// to Raft on DocStore. When Raft receives the request, it retrieves the
// DFMID and forwards the request to its DataletSE. After that, if it 
// needs to replicate, it will replicate as needed. When the majority 
// has responded, Raft sends a response back to the caller.
//
// Modification History:
// 2015/05/28 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "DocFileMgrNew/DocFileMgrNew.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "API/AosApi.h"
#include "UtilData/JPID.h"    
#include "UtilData/JSID.h"    
#include "JimoRaft/RaftServer.h"
#include "SEInterfaces/ClusterMgrObj.h"
#include "SEInterfaces/ClusterObj.h"
#include "SEInterfaces/CubeMapObj.h"


AosDocFileMgrNew::AosDocFileMgrNew(
		AosRundata	*rdata,
		const u64	aseid,
		const u32	headerCustomSize)
:
mAseId(aseid),
mHeaderCustomSize(headerCustomSize)
{
	if (!init(rdata))
	{
		OmnThrowException(rdata->getErrmsg());
		return;
	}
}


AosDocFileMgrNew::AosDocFileMgrNew(
		AosRundata *rdata,
		const AosDfmConfig &config)	
:
mDfmLog(config)
{
	if (!init(rdata))
	{
		OmnThrowException(rdata->getErrmsg());
		return;
	}
}


AosDocFileMgrNew::~AosDocFileMgrNew()
{
}


bool
AosDocFileMgrNew::init(AosRundata *rdata)
{
	// An instance of AosDocFileMgrNew is created for a DataletSE
	// on a physical machine. The DataletSE running on this physical
	// machine is defined by a set of Raft instances, one for
	// each cube hosted by this physical machine.
	// This function creates these Raft instances. 
	AosClusterMgrObj *cluster_mgr = AosClusterMgrObj::getClusterMgr(rdata);
	aos_assert_rr(cluster_mgr, rdata, false);
	AosClusterObj *cluster = cluster_mgr->getDocStoreClusterAsync(rdata);
	aos_assert_rr(cluster, rdata, false);

	vector<AosCubeMapObj::CubeInfo> cubes;
	bool rslt = cluster->getHostedCubes(rdata, cubes);
	aos_assert_rr(rslt, rdata, false);
	if (cubes.size() > 0)
	{
		for (u32 i=0; i<cubes.size(); i++)
		{
			u64 cube_id = cubes[i].cube_id;
			vector<AosCubeMapObj::CubeInfo> cube_group;
			rslt = cluster->getCubeGroup(rdata, cube_id, cube_group);
			aos_assert_rr(rslt, rdata, false);
			aos_assert_rr(cube_group.size() > 0, rdata, false);

			//later on, we need to filter out the RAFT 
			//not belonging to current node
			if (mRafts.size() < cube_id+1) mRafts.resize(cube_id+1);
			AosRaftStateMachinePtr pStatMach = new AosBlobSEAPI(rdata, cube_id, mAseId, mHeaderCustomSize);
			mRafts[cube_id] = OmnNew AosRaftServer(rdata, cube_id, cube_group, pStatMach);
			mRafts[cube_id]->setJPId(0);
			mRafts[cube_id]->setAseId(mAseId); 
			mRafts[cube_id]->setCubeId(cube_id);
			if (!pStatMach->init(rdata, mRafts[cube_id]->getLastLogIdApplied()))
			{
				OmnAlarm << "BlobSEAPI init failed, lastLogIdApplied:" << mRafts[cube_id]->getLastLogIdApplied()
						<< " cubeid:" << cube_id << enderr;

				return false;
			}
		}
	}

	//currently set them the same value
	mCubeNum = mLocalCubeNum = cubes.size();
	return true;
}

AosRaftServer*
AosDocFileMgrNew::getRaftServer(u64 cubeId)
{
	aos_assert_r(cubeId < mRafts.size(), NULL);
	return mRafts[cubeId];
}

bool 
AosDocFileMgrNew::start(AosRundata *rdata)
{
	for (u32 i=0; i<mRafts.size(); i++)
	{
		if (mRafts[i]) mRafts[i]->start();
	}

	return true;
}


bool 
AosDocFileMgrNew::stop()
{
	// This function stops a DocFileMgrNew
	OmnNotImplementedYet;
	return false;
}


bool 
AosDocFileMgrNew::createDoc(
		AosRundata* rdata, 
		const u64 aseid,
		const u64 docid, 
		const u64 snap_id,
		const AosBuffPtr &doc_body,
		AosJimoCall &jimo_call,
		AosBuffPtr pHeaderCustomData)
{
	int cube_id = getCubeId(docid);
	aos_assert_r(cube_id >= 0, false);
	if (!checkLeader(rdata, docid, jimo_call))
	{
		jimo_call.sendResp(rdata); 
		//for debugging purpose
		//aos_assert_r(false, false);
		return true;
	}

	if (!checkId(docid, rdata, jimo_call))
	{
		jimo_call.sendResp(rdata);
		//for debugging purpose
		//aos_assert_r(false, false);
		return true;
	}

	//generate buff data to be handled by BlobSE
	//hard code some values for now
	AosBuffPtr buff = OmnNew AosBuff(5000 AosMemoryCheckerArgs);
	if (pHeaderCustomData.isNull())
	{
		pHeaderCustomData = OmnNew AosBuff(mHeaderCustomSize AosMemoryCheckerArgs);
		pHeaderCustomData->setDataLen(mHeaderCustomSize);
	}

	if (mHeaderCustomSize < pHeaderCustomData->dataLen())
	{
		OmnAlarm << "mHeaderCustomSize:" << mHeaderCustomSize
				<< " < pHeaderCustomData->dataLen():"  << pHeaderCustomData->dataLen() << enderr;
		return false;
	}

	if (pHeaderCustomData->dataLen() < mHeaderCustomSize)
	{
		//fill the blank
		AosBuffPtr pNewCustomData = OmnNew AosBuff(mHeaderCustomSize AosMemoryCheckerArgs);
		bool rslt = pNewCustomData->setBuff(pHeaderCustomData->data(), pHeaderCustomData->dataLen());
		if (!rslt)
		{
			OmnAlarm << "pNewCustomData->setBuff failed! docid:" << docid << " snap_id:" << snap_id
					<< " doc_body:" << doc_body << enderr;
			return false;
		}
		//TODO:we do not fill the rest with anything for efficiency, but we may do it if there was any bugs
		//on the caller's side
		pHeaderCustomData = pNewCustomData;
		pHeaderCustomData->setDataLen(mHeaderCustomSize);
	}
	AosBlobSEAPI::genBlobSEData(buff, snap_id, docid, AosBlobSEReqEntry::eSave, doc_body, pHeaderCustomData);
	jimo_call.reset();

	//AosRaftStateMachine *RaftStateMachine = AosRaftStateMachine::createRaftStateMachineStatic(rdata, "AosBlobSEAPI", 1);
	//aos_assert_rr(RaftStateMachine, rdata, false);

	if (!mRafts[cube_id]->handleClientData(rdata, jimo_call, buff))
	{
		// It is out believing that Raft SHALL never fail. If raft fail, 
		// it is a serious system error.
		jimo_call.setHardwareFail();
		AosLogError(rdata, false, "failed_create_doc") << enderr;
		OmnAlarm << "failed_create_doc" << enderr;

		jimo_call.sendResp(rdata);
		return true;
	}

#if 0
	else
	{
		jimo_call.setSuccess();
		//OmnScreen << "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx" << endl;
	}

	jimo_call.sendResp(rdata);
#endif

	//Return without send response since this is 
	//an async call. 
	//
	//However, if something failed, the jimocall
	//needs to send back fail result right away
	//as above code
	return true;
}

bool 
AosDocFileMgrNew::modifyDoc(AosRundata* rdata, 
		const u64 docid, 
		const u64 snap_id, 
		const AosBuffPtr &doc_body,
		AosJimoCall &jimo_call,
		AosBuffPtr pHeaderCustomData)
{
	int cube_id = getCubeId(docid);
	aos_assert_r(cube_id >= 0, false);
	if (!checkLeader(rdata, docid, jimo_call)) 
	{
		jimo_call.sendResp(rdata);
		return true;                    
	}

	if (!checkId(docid, rdata, jimo_call))
	{
		jimo_call.sendResp(rdata);
		return true; 
	}

	AosBuffPtr buff = OmnNew AosBuff(5000 AosMemoryCheckerArgs);
	if (pHeaderCustomData.isNull())
	{
		pHeaderCustomData = OmnNew AosBuff(mHeaderCustomSize AosMemoryCheckerArgs);
		pHeaderCustomData->setDataLen(mHeaderCustomSize);
	}
	if (mHeaderCustomSize < pHeaderCustomData->dataLen())
	{
		OmnAlarm << "mHeaderCustomSize:" << mHeaderCustomSize
				<< " < pHeaderCustomData->dataLen():"  << pHeaderCustomData->dataLen() << enderr;
		return false;
	}
	if (pHeaderCustomData->dataLen() < mHeaderCustomSize)
	{
		//fill the blank
		AosBuffPtr pNewCustomData = OmnNew AosBuff(mHeaderCustomSize AosMemoryCheckerArgs);
		bool rslt = pNewCustomData->setBuff(pHeaderCustomData->data(), pHeaderCustomData->dataLen());
		if (!rslt)
		{
			OmnAlarm << "pNewCustomData->setBuff failed! docid:" << docid << " snap_id:" << snap_id
					<< " doc_body:" << doc_body << enderr;
			return false;
		}
		//TODO:we do not fill the rest with anything for efficiency, but we may do it if there was any bugs
		//on the caller's side
		pHeaderCustomData = pNewCustomData;
		pHeaderCustomData->setDataLen(mHeaderCustomSize);
	}
	AosBlobSEAPI::genBlobSEData(buff, snap_id, docid, AosBlobSEReqEntry::eSave, doc_body, pHeaderCustomData);
	jimo_call.reset();
	//AosRaftStateMachine *RaftStateMachine = AosRaftStateMachine::createRaftStateMachineStatic(rdata, "AosBlobSEAPI", 1);
	//aos_assert_rr(RaftStateMachine, rdata, false);

	if (!mRafts[cube_id]->handleClientData(rdata, jimo_call, buff)) 
	{
		jimo_call.setHardwareFail(); 
		AosLogError(rdata, false, "failed_create_doc") << enderr;

		jimo_call.sendResp(rdata);
		return true;              
	}
#if 0	
	else
	{
		jimo_call.setSuccess();
	}

	jimo_call.sendResp(rdata); 
#endif

	return true;
}


bool
AosDocFileMgrNew::deleteDoc(
		AosRundata *rdata, 
		const u64 aseid,
		const u64 docid,
		const u64 snap_id,
		AosJimoCall &jimo_call,
		AosBuffPtr pHeaderCustomData)
{
	int cube_id = getCubeId(docid);
	aos_assert_r(cube_id >= 0, false);
	if (!checkLeader(rdata, docid, jimo_call))  
	{
		jimo_call.sendResp(rdata);
		return true;          
	}

	if (!checkId(docid, rdata, jimo_call))
	{
		jimo_call.sendResp(rdata);
		return true; 
	}

	AosBuffPtr buff = OmnNew AosBuff(5000 AosMemoryCheckerArgs);
	if (pHeaderCustomData.isNull())
	{
		pHeaderCustomData = OmnNew AosBuff(mHeaderCustomSize AosMemoryCheckerArgs);
		pHeaderCustomData->setDataLen(mHeaderCustomSize);
	}
	if (mHeaderCustomSize < pHeaderCustomData->dataLen())
	{
		OmnAlarm << "mHeaderCustomSize:" << mHeaderCustomSize
				<< " < pHeaderCustomData->dataLen():"  << pHeaderCustomData->dataLen() << enderr;
		return false;
	}
	if (pHeaderCustomData->dataLen() < mHeaderCustomSize)
	{
		//fill the blank
		AosBuffPtr pNewCustomData = OmnNew AosBuff(mHeaderCustomSize AosMemoryCheckerArgs);
		bool rslt = pNewCustomData->setBuff(pHeaderCustomData->data(), pHeaderCustomData->dataLen());
		if (!rslt)
		{
			OmnAlarm << "pNewCustomData->setBuff failed! docid:" << docid << " snap_id:" << snap_id << enderr;
			return false;
		}
		//TODO:we do not fill the rest with anything for efficiency, but we may do it if there was any bugs
		//on the caller's side
		pHeaderCustomData = pNewCustomData;
		pHeaderCustomData->setDataLen(mHeaderCustomSize);
	}
	AosBlobSEAPI::genBlobSEData(buff, snap_id, docid, AosBlobSEReqEntry::eDelete, OmnNew AosBuff(AosMemoryCheckerArgsBegin), pHeaderCustomData);
	//AosRaftStateMachine *RaftStateMachine = AosRaftStateMachine::createRaftStateMachineStatic(rdata, "AosBlobSEAPI", 1); 
	//aos_assert_rr(RaftStateMachine, rdata, false);

	if (!mRafts[cube_id]->handleClientData(rdata, jimo_call, buff))
	{
		jimo_call.setHardwareFail();
		AosLogError(rdata, false, "failed_create_doc") << enderr;

		jimo_call.sendResp(rdata);
		return true;              
	}
#if 0	
	else
	{
		jimo_call.setSuccess();
	}

	jimo_call.sendResp(rdata); 
#endif	
	return true;
}


bool 
AosDocFileMgrNew::getDoc(
		AosRundata *rdata, 
		const u64 aseid,
		const u64 snap_id,
		const u64 docid, 
		AosJimoCall &jimo_call)
{
	int cube_id = getCubeId(docid);
	aos_assert_r(cube_id >= 0, false);

	if (!checkLeader(rdata, docid, jimo_call))
	{
		OmnScreen << "checkLeader failed, docid:" << docid 
				  << " jimocallid:" << jimo_call.getJimoCallID() << endl;
		jimo_call.sendResp(rdata);
		return true;
	}

	if (!checkId(docid, rdata, jimo_call))
	{
		OmnAlarm << "checkId failed, docid:" << docid 
				 << " jimocallid:" << jimo_call.getJimoCallID() << enderr;
		jimo_call.sendResp(rdata);
		return true;
	}

	AosBlobSEAPI *blob;
	u64 timestamp = OmnTime::getTimestamp();
	bool rslt;
	AosBuffPtr body_data;
	AosBuffPtr pHeaderCustomData;
	aos_assert_r(mRafts[cube_id], false);
	blob = dynamic_cast<AosBlobSEAPI*>(mRafts[cube_id]->getStatMach());
	aos_assert_r(blob, false);
	rslt = blob->readDoc(rdata, aseid, snap_id, docid, timestamp, body_data, pHeaderCustomData); 
	if(!rslt)
	{
		OmnString errmsg;
		errmsg << "getDoc failed, param:[" << " aseid:" << aseid 
			   << " snap_id:" << snap_id << " docid:" << docid
			   << " ]";
		jimo_call.arg(AosFN::eErrmsg, errmsg);
		jimo_call.setHardwareFail();
		AosLogError(rdata, false, "failed_create_doc") << enderr;
		OmnAlarm << errmsg << enderr;
	}
	else
	{
		//Successful result
		if (body_data)
		{
			jimo_call.arg(AosFN::eBuff, body_data);
		}
		if (pHeaderCustomData.notNull())
		{
			jimo_call.arg(AosFN::eHeaderCustomData, pHeaderCustomData);
		}
		jimo_call.setSuccess();
	}

	//send the response at the end only
	jimo_call.sendResp(rdata);
	return true;
}


	bool 
AosDocFileMgrNew::removeAllFiles(AosRundata *rdata)
{
	OmnNotImplementedYet;
	return false;
}


u64 
AosDocFileMgrNew::createSnapshot(AosRundata *rdata, const u64 snap_id)
{
	OmnNotImplementedYet;
	return 0;
}


	bool 
AosDocFileMgrNew::commitSnapshot(AosRundata *rdata, const u64 snap_id)
{
	OmnNotImplementedYet;
	return false;
}


	bool 
AosDocFileMgrNew::rollbackSnapshot(AosRundata *rdata, const u64 snap_id)
{
	OmnNotImplementedYet;
	return false;
}


	bool 
AosDocFileMgrNew::mergeSnapshot(
		AosRundata *rdata, 
		const u64 target_snap_id, 
		const u64 merger_snap_id)
{
	OmnNotImplementedYet;
	return false;
}

bool
AosDocFileMgrNew::checkLeader(AosRundata *rdata, 
		u64 docid,
		AosJimoCall &jimo_call)
{
	//u32 cube_id = docid % AosGetNumCubes();
	int cube_id = getCubeId(docid);
	aos_assert_r(cube_id >= 0, false);
	if (cube_id >= mRafts.size())
	{
		// IMPORTANT!!!!!!!!!!!!!!!!
		// For testing, we treat this error as system error. 
		// When in production, it should not be a system error.
		jimo_call.setHardwareFail();
		AosLogError(rdata, true, "internal_error") << enderr;
		return false;
	}

	AosRaftServer *raft = mRafts[cube_id];
	if (!raft)
	{
		// IMPORTANT!!!!!!!!!!!!!!!!
		// For testing, we treat this error as system error. 
		// When in production, it should not be a system error.
		jimo_call.setHardwareFail();
		AosLogError(rdata, true, "internal_error") << enderr;
		return false;
	}

	if (!raft->isLeader())
	{
		// This is not the leader. It needs to inform the caller
		// the leader.
		int leader_id = raft->getLeaderId();
		OmnScreen << "My id:" << raft->getServerId() << ", my role is:"
			<< raft->getRoleStr() << " leader_id =  " << leader_id 
			<< " jimocall id:" << jimo_call.getJimoCallID() << endl;
		jimo_call.arg(AosFN::eLeader, leader_id);
		jimo_call.arg(AosFN::eErrmsg, "not_leader");
		jimo_call.setLogicalFail();
		//jimo_call.sendResp(rdata);
		return false;
	}

	return true;
}

bool
AosDocFileMgrNew::checkId(u64 docid, AosRundata *rdata, AosJimoCall &jimo_call)
{
	// Check JPID
	u64 jpid = rdata->getJPID();
	if (!AosJPID::isValid(jpid)) 
	{
		jimo_call.arg(AosFN::eErrmsg, "invalid_jpid");
		jimo_call.setLogicalFail();
		//jimo_call.sendResp(rdata);
		return false;
	}

	u64 jsid = rdata->getJSID();
	if (!AosJSID::isValid(jsid)) 
	{
		jimo_call.arg(AosFN::eErrmsg, "invalid_jsid");
		jimo_call.setLogicalFail();
		//jimo_call.sendResp(rdata);
		return false;
	}

	if (docid == 0)
	{
		jimo_call.arg(AosFN::eErrmsg, "invalid_docid");
		jimo_call.setLogicalFail();
		//jimo_call.sendResp(rdata);
		return false;
	}

	return true;
}

int
AosDocFileMgrNew::getCubeId(u64 docid)
{
	aos_assert_r(mCubeNum > 0, -1);
	return docid % mCubeNum;
}
