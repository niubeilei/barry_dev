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
#ifndef AOS_ReplicMgr_ReplicPolicy_h
#define AOS_ReplicMgr_ReplicPolicy_h

#include "Rundata/Ptrs.h"
#include "ReplicMgr/Ptrs.h"
#include "SEInterfaces/CubeGroup.h"
#include "SEInterfaces/ReplicPolicyType.h"
#include "Util/String.h"
#include "Util/Ptrs.h"
#include "Util/RCObjImp.h"

#include <map>
using namespace std;

class AosReplicPolicy: public OmnRCObject 
{
	OmnDefineRCObject;
	
private:
	OmnString				mName;
	AosReplicPolicyType::E	mType;
	
public:
	AosReplicPolicy(
		const OmnString &name,
		const AosReplicPolicyType::E tp,
		const bool reg);
	~AosReplicPolicy();
	
	static AosReplicPolicyPtr getReplicPolicy(const OmnString &str_policy);

	virtual u32 getCubeBkpNum() = 0;
	virtual bool addBkpProc(
					map<u32, AosServerInfoPtr> &svr_infos,
					map<u32, AosServerInfoPtr>::iterator &crt_itr,
					AosCubeGroupPtr &cube_group) = 0;
	
	
	AosReplicPolicyType::E getType(){ return mType; };

private:
	bool 	registerReplicPolicy(AosReplicPolicyPtr &rp);
	
};
#endif
