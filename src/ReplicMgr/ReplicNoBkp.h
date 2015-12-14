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
// 08/14/2012	Created by Ketty
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_ReplicMgr_ReplicNoBkp_h
#define AOS_ReplicMgr_ReplicNoBkp_h

#include "ReplicMgr/ReplicPolicy.h"
#include "Util/RCObjImp.h"
#include "Util/Ptrs.h"

class AosReplicNoBkp: public AosReplicPolicy 
{

public:
	AosReplicNoBkp(const bool regflag);
	~AosReplicNoBkp();
	
	virtual bool addBkpProc(
					map<u32, AosServerInfoPtr> &svr_infos,
					map<u32, AosServerInfoPtr>::iterator &crt_itr,
					AosCubeGroupPtr &cube_group){ return true; };
	
	virtual u32 getCubeBkpNum(){ return 1;};

};
#endif
