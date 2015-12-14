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
// 2013/09/24 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_SEInterfaces_ReplicMgrObj_h
#define Aos_SEInterfaces_ReplicMgrObj_h

#include "SEInterfaces/Ptrs.h"
#include "SEInterfaces/ReplicPolicyType.h"
#include "Util/RCObject.h"
#include "Util/String.h"
#include <map>
using namespace std;


class AosReplicMgrObj : public OmnRCObject
{
private:
	static AosReplicMgrObjPtr	smObject;

public:
	virtual bool initPolicy(const OmnString str_policy) = 0;
	virtual bool initBkpProc(
					map<u32, AosServerInfoPtr> &svr_infos,
					map<u32, AosCubeGroupPtr> &cube_grps) = 0;
	virtual u32 getCubeBkpNum() = 0;
	virtual AosReplicPolicyType::E getPolicyType() = 0;

	static void setReplicMgr(const AosReplicMgrObjPtr &r) {smObject = r;}
	static AosReplicMgrObjPtr getReplicMgr(){ return smObject; };
};

#endif
