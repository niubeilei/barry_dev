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
// 2015/03/13 Created by Ketty Guo 
////////////////////////////////////////////////////////////////////////////
#include "DataSync/DataSync.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "JimoAPI/ClusterMgr.h"
#include "Porting/Sleep.h"
#include "Util/File.h"
#include "Util/OmnNew.h"
#include "Util/Ptrs.h"

//using namespace Jimo;

AosDataSync::AosDataSync()
:
mInited(false),
mSelfSvrId(-1),
mSelfCluster(0),
mNeedSyncData(false)
{
}


AosDataSync::~AosDataSync()
{
}


bool
AosDataSync::init(const AosRundataPtr &rdata)
{
	// check whether last time System is normal exit.
	
	mCubeId = gSystem->getSelfCubeId(rdata); 
	mSelfSvrId = gSystem->getSelfSvrId(rdata);
	
	mSelfCluster = gClusterMgr->getSelfCluster(rdata);
	aos_assert_r(mSelfCluster, false);

	OmnString dir_name = gSystem->getRootDir();
	aos_assert_r(dir_name != "", false);

	mSysInitFname = dir_name;
	mSysInitFname << "/" << ".JimoInit";

	bool rslt = checkNeedSyncData(rdata);
	aos_assert_r(rslt, false);
	
	mInited = true;
	return true;
}


bool
AosDataSync::checkNeedSyncData(const AosRundataPtr &rdata)
{
	aos_assert_r(mSysInitFname != "", false);
	
	bool is_new_init = false;
	OmnFilePtr init_file = OmnNew OmnFile(mSysInitFname, OmnFile::eReadWrite AosMemoryCheckerArgs);
	if(!init_file || !init_file->isGood())
	{
		// Don't has .JimoInit file,
		// means The Whole System just startup with no Data.
		// no need SyncData.
		
		init_file = OmnNew OmnFile(mSysInitFname, OmnFile::eCreate AosMemoryCheckerArgs);
		if(!init_file->isGood())
		{
			OmnAlarm << "Failed to open the file: "
				<< mSysInitFname << enderr;
			return false;
		}
		
		is_new_init = true;
	}
	aos_assert_r(init_file && init_file->isGood(), false);	
	
	if(!hasBkpCube(rdata))
	{
		mNeedSyncData = false;	
		OmnScreen << "DataSync; "
			<< "self cluster has no bkp. no need sync Data."
			<< endl;
	}
	else if(is_new_init)
	{
		mNeedSyncData = false;	
		OmnScreen << "DataSync; " 
			<< "System has no data. no need sync Data."
			<< endl;
	}
	else
	{
		bool normal_exit_flag = ((init_file->readBinaryU32(eNormalExitOff, 0)) == 1); 
		mNeedSyncData = !normal_exit_flag;
	
		OmnString p_str = "DataSync; ";
		if(mNeedSyncData)
		{
			p_str << "System abnormal exit. need sync Data.";
		}
		else
		{
			p_str << "System normal exit. no need sync Data.";
		}
		OmnScreen << p_str << endl;
	}

	// reset normal_exist_flag
	init_file->setU32(eNormalExitOff, 0, true);
	init_file->closeFile();
	
	return true;
}


bool
AosDataSync::stop(const AosRundataPtr &rdata)
{
	// when System normal exit. need call this func.
	// will write a normal_exit flag into .JimoInit file
	
	aos_assert_r(mSysInitFname != "", false);

	OmnFilePtr init_file = OmnNew OmnFile(mSysInitFname, OmnFile::eReadWrite AosMemoryCheckerArgs);
	if(!init_file || !init_file->isGood())
	{
		OmnAlarm << "Failed to open the file: "
			<< mSysInitFname << enderr;
		return false;
	}
	
	// set normal_exist_flag
	init_file->setU32(eNormalExitOff, 1, true);
	init_file->closeFile();

	return true;
}


bool
AosDataSync::start(const AosRundataPtr &rdata)
{
	bool rslt;
	if(!mInited)
	{
		rslt = init(rdata);
		aos_assert_r(rslt, false);
	}

	if(!mNeedSyncData)	return normalStart(rdata);

	vector<int> bkp_cube_sids;
	rslt = getBkpCubeSids(rdata, bkp_cube_sids);
	aos_assert_r(rslt, false);

	// first sync data.
	OmnScreen << "DataSync; first sync data." << endl; 
	rslt = syncDataBase(rdata, bkp_cube_sids);
	aos_assert_r(rslt, false);
	OmnScreen << "DataSync; first sync data finish." << endl; 
	
	rslt = gDocFileMgr.setAllDfmsWaitSecondSync();
	aos_assert_r(rslt, false);

	// this must be sync call. when all other recved this msg, this func can callback.
	rslt = mSelfCluster->updateCubeStatus(rdata, mCubeId, 
			mSelfSvrId, AosCubeStatus::eWriteOnly);
	aos_assert_r(rslt, false);

	// second sync data.
	OmnScreen << "DataSync; second sync data." << endl; 
	rslt = syncDataBase(rdata, bkp_cube_sids);
	aos_assert_r(rslt, false);
	OmnScreen << "DataSync; second sync data finish." << endl; 
	
	rslt = normalStart(rdata);
	aos_assert_r(rslt, false);

	return true;
}


bool
AosDataSync::normalStart(const AosRundataPtr &rdata)
{
	bool rslt = gDocFileMgr.setAllDfmsAvailable();
	aos_assert_r(rslt, false);
		
	rslt = mSelfCluster->updateCubeStatus(rdata, mCubeId, 
				mSelfSvrId, AosCubeStatus::eAvailable);
	aos_assert_r(rslt, false);
	return true;	
}


bool
AosDataSync::syncDataBase(
		const AosRundataPtr &rdata,
		vector<int> &bkp_cube_sids)
{
	// This func need guarantee sync data succ.

	bool rslt;
	bool sync_succ;

	// try sync with one of available bkp.
	rslt = syncWithOneBkp(rdata, bkp_cube_sids, sync_succ);
	aos_assert_r(rslt, false);
	OmnScreen << "DataSync; syncWithOneAvailableBkp succ? " 
		<< (sync_succ ? "true" : "false") << endl; 

	if(sync_succ) return true;

	// has no available bkp.
	sync_succ = false;
	while(!sync_succ)
	{
		rslt = syncWithEachBkp(rdata, bkp_cube_sids, sync_succ);
		aos_assert_r(rslt, false);
		
		if(!sync_succ)
		{
			// maybe all other bkp svr had down. wait them startup.
			OmnScreen << "DataSync; syncWithEachBkp failed."
				<< "try again. "<< endl; 
			OmnSleep(1);
		}
	}
	
	OmnScreen << "DataSync; syncWithEachBkp succ."  << endl; 
	return true;
}


bool
AosDataSync::syncWithOneBkp(
		const AosRundataPtr &rdata,
		vector<int> &bkp_cube_sids,
		bool &sync_succ)
{
	// sync Data with one available bkp.
	bool rslt;

	sync_succ = false;

	vector<AosDocFileMgrObjPtr> vt_dfms;
	rslt = gDocFileMgr.getAllDfms(vt_dfms);
	aos_assert_r(rslt, false);

	for(u32 i = 0; i<vt_dfms.size(); i++)
	{
		bool crt_dfm_sync_succ = false;
		rslt = syncOneDFMWithOneBkp(rdata, vt_dfms[i],
				bkp_cube_sids, crt_dfm_sync_succ);
		if (!rslt)
		{
			AosLogError(rdata, false, "internal_error") << enderr;
			return false;
		}

		if (!crt_dfm_sync_succ)
		{
			sync_succ = false;
			return true;
		}
	}

	sync_succ = true;
	return true;
}


bool
AosDataSync::syncOneDFMWithOneBkp(
		const AosRundataPtr &rdata,
		const AosDocFileMgrObjPtr &dfm,
		vector<int> &bkp_cube_sids,
		bool &sync_succ)
{
	// this bkp_cube_sids are available bkp cube.
	bool rslt;
	AosCubeStatus::E bkp_cube_sts;
	for (u32 i=0; i<bkp_cube_sids.size(); i++)
	{
		bkp_cube_sts = mSelfCluster->getCubeStatus(rdata,
				mCubeId, bkp_cube_sids[i]);
		if(bkp_cube_sts != AosCubeStatus::eAvailable)
		{
			// bkp_cube_sts is setted locally.
			continue;
		}

		rslt = syncDataPriv(rdata, dfm,
				bkp_cube_sids[i], sync_succ, bkp_cube_sts);
		aos_assert_r(rslt, false);

		if (!sync_succ)
		{
			// the bkp must be deathed.
			continue;
		}

		if(bkp_cube_sts != AosCubeStatus::eAvailable)
		{
			// bkp_cube_sts is setted by the bkp cube, will 
			// more accurate.
			// this is for check.
			continue;
		}
		
		// has sync successful!.
		return true;
	}

	// Did not sync successfully. This is a serious problem.
	// This means that we tried all the backups but still
	// failed syncing. 
	AosLogError(rdata, false, "failed_syncing_data") << enderr;
	OmnScreen << "DataSync; sync DFM with onebkp failed."
		<< "dfm_id:" << dfm->getDfmId() 
		<< endl; 

	sync_succ = false;
	return true;
}


bool
AosDataSync::syncWithEachBkp(
		const AosRundataPtr &rdata,
		vector<int> &bkp_cube_sids,
		bool &sync_succ)
{
	// sync Data with each bkp unless. if one of this bkps has
	// deathed. sync failed.
	bool rslt;

	sync_succ = false;

	vector<AosDocFileMgrObjPtr> vt_dfms;
	rslt = gDocFileMgr.getAllDfms(vt_dfms);
	aos_assert_r(rslt, false);

	for(u32 i = 0; i<vt_dfms.size(); i++)
	{
		bool crt_dfm_sync_succ = false;
		rslt = syncOneDFMWithEachBkp(rdata, vt_dfms[i],
				bkp_cube_sids, crt_dfm_sync_succ);
		if (!rslt)
		{
			AosLogError(rdata, false, "internal_error") << enderr;
			return false;
		}

		if (!crt_dfm_sync_succ)
		{
			sync_succ = false;
			return true;
		}
	}

	sync_succ = true;
	return true;
}


bool
AosDataSync::syncOneDFMWithEachBkp(
		const AosRundataPtr &rdata,
		const AosDocFileMgrObjPtr &dfm,
		vector<int> &bkp_cube_sids,
		bool &sync_succ)
{
	bool rslt;
	for (u32 i=0; i<bkp_cube_sids.size(); i++)
	{
		rslt = syncDataPriv(rdata, dfm,
				bkp_cube_sids[i], sync_succ, bkp_cube_sts);
		aos_assert_r(rslt, false);

		if (!sync_succ)
		{
			// means this bkp svr has deathed.
			OmnScreen << "DataSync; sync DFM with eachbkp failed."
				<< "dfm_id:" << dfm->getDfmId()
				<< "bkp_cube_sid:" << bkp_cube_sids[i]
				<< endl; 
			return true;
		}
	}

	sync_succ = true;
	return true;
}


bool
AosDataSync::syncDataPriv(
		const AosRundataPtr &rdata, 
		const AosDocFileMgrObjPtr dfm,
		const int bkp_cube_svrid,
		bool &sync_succ,
		AosCubeStatus::E &bkp_cube_sts)
{
	// This function syncs the data for 'dfm' with the backup
	// 'bkp_cube_svrid'. If successful, 'sync_succ' is set to true.
	// Otherwise, it is set to false. If it fails syncing with
	
	// the bkp_cube_sts is setted by the bkp cube, will 
	// more accurate.
	
	bool rslt;
	sync_succ = false;

	AosBuffPtr synced_data;
	bool sync_finished = false;
	bool svr_death;
	bool first_sync = true;
	int crt_sync_pos = -1;	// set by remote bkp cube.

	u32 dfm_id = dfm->getDfmId();
	while(!sync_finished)
	{
		if(first_sync)
		{
			// read dfm last recved trans_ids.
			u64 last_transid;
			rslt = dfm->getLastRecvedTransId(rdata, last_transid);
			aos_assert_r(rslt, false);
			
			OmnScreen << "DataSync; syncData."
				<< "dfm_id:" << dfm->getDfmId()
				<< "bkp_cube_sid:" << bkp_cube_sid
				<< "last_transid:" << last_transid
				<< endl; 

			rslt = gDocFileMgr.getHistoryData(rdata, dfm_id, bkp_cube_svrid, 
					last_transid, svr_death, crt_sync_pos, synced_data, sync_finished);
			aos_assert_r(rslt, false);
			first_sync = false;
		}
		else
		{
			OmnScreen << "DataSync; syncData."
				<< "dfm_id:" << dfm->getDfmId()
				<< "bkp_cube_sid:" << bkp_cube_sid
				<< "crt_sync_pos:" << crt_sync_pos 
				<< endl; 
			
			rslt = gDocFileMgr.getHistoryData(rdata, dfm_id, bkp_cube_svrid, 
					svr_death, crt_sync_pos, synced_data, sync_finished);
			aos_assert_r(rslt, false);
		}

		if(svr_death)
		{
			// sync failed. try next bkp svr.
			bkp_cube_sts = AosCubeStatus::eDeath;
			sync_succ = false;
			return true;
		}

		rslt = dfm->procRecoveredData(rdata, synced_data);
		aos_assert_r(rslt, false);
	}

	// bkp_cube_sts ought to set by bkp cube. 
	// temp we set bkp_cube_sts available.
	bkp_cube_sts = AosCubeStatus::eAvailable;
	sync_succ = true;
	return true;
}


bool
AosDataSync::getBkpCubeSids(
		const AosRundataPtr &rdata,
		vector<int> &bkp_cube_sids)
{
	vector<int> endpoint_ids;
	bool rslt = mSelfCluster->getCubeEndpointIds(rdata, mCubeId, endpoint_ids);
	aos_assert_rr(rslt, rdata, false);
	aos_assert_rr(endpoint_ids.size() > 0, rdata, false);
	if(endpoint_ids.size() == 1)
	{
		aos_assert_r(endpoint_ids[0] == mSelfSvrId, false);
	}

	bkp_cube_sids.clear();
	for(u32 i=0; i<endpoint_ids.size(); i++)
	{
		if(endpoint_ids[i] != mSelfSvrId)
		{
			bkp_cube_sids.push_back(endpoint_ids[i]);
		}
	}

	return true;
}


bool
AosDataSync::hasBkpCube(const AosRundataPtr &rdata)
{
	vector<int> bkp_cube_sids;
	bool rslt = getBkpCubeSids(rdata, bkp_cube_sids);
	aos_assert_r(rslt, false);
	
	return (bkp_cube_sids.size() > 0);
}





// need funcs.
/*
CubeStatus
{
	eIdle,
	eWriteOnly,
	eAvailable,
	eDeath,
}.

DocFileMgrStatus 
{
	eWaitFirstSync,
	eWaitSecondSync,
	eAvailable,
}.
	
u32 cube_id = gSystem->getSelfCubeId(rdata); 
int svr_id = gSystem->getSelfSvrId(rdata);
OmnString dir_name = gSystem->getRootDir();


AosClusterObj *self_cluster = gClusterMgr->getSelfCluster(rdata);
bool rslt = self_cluster->getCubeEndpointIDs(rdata, mCubeId, bkp_cube_sids);
bool rslt = self_cluster->updateCubeStatus(rdata, mCubeId, svr_id, AosCubeStatus::eAvailable);
AosCubeStatus::E sts = self_cluster->getCubeStatus(rdata, mCubeId, svr_id);



bool rslt = gDocFileMgr.setAllDfmsAvailable();
bool rslt = gDocFileMgr.setAllDfmsWaitSecondSync();
bool rslt = gDocFileMgr.getAllDfms(vt_dfms);
bool rslt = dfm->getLastRecvedTransId(rdata, last_transid);
bool rslt = dfm->procRecoveredData(rdata, synced_data);

*/

