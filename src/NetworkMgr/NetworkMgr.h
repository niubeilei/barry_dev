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
// 03/09/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_NetworkMgr_NetworkMgr_h
#define Aos_NetworkMgr_NetworkMgr_h

#include "CommandProc/AllCommandProcs.h"
#include "NetworkMgr/Ptrs.h"
#include "NetworkMgrUtil/Ptrs.h"
#include "NetworkMgrUtil/NetCommand.h"
#include "Rundata/Rundata.h"
#include "SEInterfaces/NetworkMgrObj.h"
#include "UtilData/ModuleId.h"
#include "Util/Ptrs.h"
#include "Util/HashUtil.h"
#include "UtilComm1/Ptrs.h"
#include <hash_map>
#include <map>


OmnDefineSingletonClass(AosNetworkMgrSingleton,
						AosNetworkMgr,
						AosNetworkMgrSelf,
						OmnSingletonObjId::eNetworkMgr,
						"NetworkMgr");

class AosNetworkMgr : public AosNetworkMgrObj
{
	OmnDefineRCObject;

	struct SelfConf
	{
		int                     mServerId;
		int                     mClientId;
		u32                     mProcId;
		AosProcessType::E          mProcType;
		int                     mCubeGrpId;
		vector<u32>             mCubeIds;
		int						mConfigMasterId;
		int                     mCrtMasterId;
	};

private:
	OmnMutexPtr					mLock;
	OmnCondVarPtr				mCondVar;
	
	SelfConf					mSelfConf;
	map<u32, AosServerInfoPtr>  mSvrInfos;
	vector<u32>                 mSvrIds;
	map<u32, AosCubeGroupPtr>   mCubeGroups;
	vector<u32>                 mCubeIds;
	map<u32, u32>               mCubeIdToGrpIdMap;
	map<AosProcessType::E, OmnString>	mTotalProcess;	
	
	int							mBroadcastPort;
	
public:
	AosNetworkMgr();
	~AosNetworkMgr();

	// Singleton class interface
	static AosNetworkMgr*   getSelf();
	virtual bool        start();
	virtual bool        stop();
	virtual bool        config(const AosXmlTagPtr &def);

	virtual bool configNetwork(const AosXmlTagPtr &app_config);
	virtual bool configSelfSvr(const AosXmlTagPtr &self_conf);
	virtual bool configServers(const AosXmlTagPtr &svrs_conf);
	virtual bool configCluster(const AosXmlTagPtr &cluster_conf);

	virtual int getSelfServerId() {return mSelfConf.mServerId;}
	virtual int getSelfClientId() {return mSelfConf.mClientId;}
	virtual u32 getSelfProcId() { return mSelfConf.mProcId; }
	virtual AosProcessType::E getSelfProcType() { return mSelfConf.mProcType; }
	virtual u32 getSelfCubeGrpId(){ return mSelfConf.mCubeGrpId; }
	virtual vector<u32> &getSelfCubeIds() { return mSelfConf.mCubeIds; }
	virtual int getSelfConfigMasterId() {return mSelfConf.mConfigMasterId; };
	virtual int getSelfCrtMasterId() { return mSelfConf.mCrtMasterId; }
	
	// Chen Ding, 2014/08/12
	// virtual u32 getNumVirtuals() { return mCubeIds.size();}
	virtual u32 getNumSvrs() { return mSvrIds.size(); }
	virtual map<u32, AosServerInfoPtr> &getSvrInfos(){ return mSvrInfos; }
	virtual map<u32, AosCubeGroupPtr> &getCubeGrps() { return mCubeGroups; }
	virtual vector<u32> & getSvrIds(){ return mSvrIds; };
	virtual vector<u32> & getTotalCubeIds(){ return mCubeIds; };
	virtual vector<u32> & getCubeIdsByGrpId(const u32 grp_id);
	virtual vector<u32> & getCubeSendSvrIds(const u32 cube_grp_id);
	virtual bool getTotalCubeGrps(vector<u32> &cube_grps);
	virtual bool isValidSvrId(const int svr_id);
	virtual int getNextSvrId(const u32 cube_grp_id, const int crt_sid);
	virtual AosServerInfoPtr getSvrInfo(const u32 svr_id);
	virtual int getCubeGrpIdByCubeId(const u32 cube_id);
	virtual OmnString getProcExeName(const AosProcessType::E tp);
	virtual int	 getBroadcastPort(){ return mBroadcastPort; };
	virtual bool isVirtualIdLocal(const u32 cube_id);

	//virtual u32	getSendProcId(const u32 cube_grp_id, const int svr_id);
	//virtual bool addCubeInfo(const AosCubeGroupPtr &cube_grp);
	//virtual bool setStatus(const u32 svr_id, const u32 proc_id, const AosProcInfo::Status status);
	//virtual AosServerInfoPtr getStatus(const u32 svr_id);

private:
	//bool configSelfSvr(const AosXmlTagPtr &self_conf);
	//bool configServers(const AosXmlTagPtr &svrs_conf);
	
	bool configProcesses(const AosXmlTagPtr &procs_conf);
	bool configCubeInfos(const AosXmlTagPtr &cubes_conf);
	bool addProcsToSvrInfo(const AosXmlTagPtr &svrs_conf);
	
	bool tryAddBkpCubeToSvrInfo();
	bool initCubeIdToGrpIdMap();
	//bool initCubeGrpToSvrMap();

	bool addDefaultProcs(
			const AosServerInfoPtr &svr_info,
			const AosXmlTagPtr procs_conf);

};
#endif

