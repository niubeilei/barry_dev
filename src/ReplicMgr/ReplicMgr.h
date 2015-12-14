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
// 09/21/2012	Created by Ketty
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_ReplicMgr_ReplicMgr_h
#define AOS_ReplicMgr_ReplicMgr_h

#include "Rundata/Ptrs.h"
#include "ReplicMgr/Ptrs.h"
#include "SEInterfaces/CubeGroup.h"
#include "SEInterfaces/ReplicPolicyType.h"
#include "SEInterfaces/ReplicMgrObj.h"
#include "Util/String.h"
#include "Util/Ptrs.h"
#include "Util/RCObjImp.h"

//#include <vector>
#include <map>
using namespace std;

class AosReplicMgr: public AosReplicMgrObj
{
	OmnDefineRCObject;
	
private:
	AosReplicPolicyPtr	mPolicy;
	
public:
	AosReplicMgr();
	~AosReplicMgr();
	
	static bool  registerPolicy(AosReplicPolicyPtr &rp);
	
	virtual bool initPolicy(const OmnString str_policy);
	virtual bool initBkpProc(
					map<u32, AosServerInfoPtr> &svr_infos,
					map<u32, AosCubeGroupPtr> &cube_grps);
	virtual u32 getCubeBkpNum();
	virtual AosReplicPolicyType::E getPolicyType();
	
};
#endif
