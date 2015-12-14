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
#ifndef AOS_ReplicMgr_ReplicOnePlusOne_h
#define AOS_ReplicMgr_ReplicOnePlusOne_h

#include "ReplicMgr/ReplicPolicy.h"
#include "Util/RCObjImp.h"
#include "Util/Ptrs.h"

class AosReplicOnePlusOne: public AosReplicPolicy 
{

public:
	AosReplicOnePlusOne(const bool regflag);
	~AosReplicOnePlusOne();

	virtual bool addBkpProc(
					map<u32, AosServerInfoPtr> &svr_infos,
					map<u32, AosServerInfoPtr>::iterator &crt_itr,
					AosCubeGroupPtr &cube_group);
	
	virtual u32 getCubeBkpNum(){ return 2; };
	
};
#endif
