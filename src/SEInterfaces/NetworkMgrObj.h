////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
//
// Modification History:
// 03/18/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_SEInterfaces_NetworkMgrObj_h
#define Aos_SEInterfaces_NetworkMgrObj_h

#include "alarm_c/alarm.h"
#include "NetworkMgrUtil/NetCommand.h"
#include "Rundata/Ptrs.h"
#include "ReplicMgr/Ptrs.h"
#include "SEInterfaces/Ptrs.h"
#include "SEInterfaces/ProcessType.h"
#include "SEInterfaces/ServerInfo.h"
#include "SEUtil/SeConfig.h"
#include "TransUtil/Ptrs.h"
#include "Util/String.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/Ptrs.h"
#include "XmlUtil/Ptrs.h"

#include <vector>
#include <map>
using namespace std;

class AosNetworkMgrObj : public OmnRCObject
{
private:
	static AosNetworkMgrObjPtr	smNetworkMgr;

protected:
	enum
	{
		eMaxCubeId = 100000	
	};

	static u32					smNumVirtuals;
	static vector<int>			smCubeId2PhyId;

public:
	static void setNetworkMgr(const AosNetworkMgrObjPtr &mgr) {smNetworkMgr = mgr;}
	static AosNetworkMgrObjPtr getNetworkMgr() {return smNetworkMgr;}
	
	virtual bool configNetwork(const AosXmlTagPtr &app_config) = 0;
	virtual bool configSelfSvr(const AosXmlTagPtr &self_conf) = 0;
	virtual bool configServers(const AosXmlTagPtr &svrs_conf) = 0;
	virtual bool configCluster(const AosXmlTagPtr &cluster_conf) = 0;

	virtual int getSelfServerId()  = 0;
	virtual int getSelfClientId() = 0;
	virtual u32 getSelfProcId()  = 0;
	virtual AosProcessType::E getSelfProcType()  = 0;
	virtual u32 getSelfCubeGrpId()  = 0;
	virtual vector<u32> &getSelfCubeIds()  = 0;
	virtual int getSelfConfigMasterId() = 0;
	virtual int getSelfCrtMasterId()  = 0;
	
	// Chen Ding, 2014/08/12
	// virtual u32 getNumVirtuals() = 0;
	static inline u32 getNumVirtuals() {return smNumVirtuals;}
	virtual u32 getNumSvrs() = 0;
	virtual map<u32, AosServerInfoPtr> &getSvrInfos()  = 0;
	virtual map<u32, AosCubeGroupPtr> &getCubeGrps()  = 0;
	virtual bool isVirtualIdLocal(const u32 cube_id) = 0;
	virtual vector<u32> & getSvrIds() = 0;
	virtual vector<u32> & getTotalCubeIds() = 0;
	virtual vector<u32> & getCubeIdsByGrpId(const u32 grp_id) = 0;
	virtual vector<u32> & getCubeSendSvrIds(const u32 cube_grp_id) = 0;
	virtual bool getTotalCubeGrps(vector<u32> &cube_grps) = 0;
	virtual bool isValidSvrId(const int svr_id) = 0;
	// Chen Ding, 2014/08/12
	virtual int getNextSvrId(const u32 cube_grp_id, const int crt_sid) = 0;
	static inline int getPhysicalId(const u32 cube_id) 
	{
		aos_assert_r(cube_id < smCubeId2PhyId.size(), -1);
		return smCubeId2PhyId[cube_id];
	}

	virtual AosServerInfoPtr getSvrInfo(const u32 svr_id) = 0;
	virtual int getCubeGrpIdByCubeId(const u32 cube_id) = 0;
	virtual OmnString getProcExeName(const AosProcessType::E tp) = 0;
	virtual int	 getBroadcastPort() = 0;
	
};
#endif
