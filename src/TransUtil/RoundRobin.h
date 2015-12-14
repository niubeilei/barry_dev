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
// 05/31/2011 	Created by Ketty 
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_TransUtil_AosRoundRobin_h
#define AOS_TransUtil_AosRoundRobin_h

#include "SEInterfaces/ServerInfo.h"
#include "TransUtil/TransDistributor.h"

#include <vector>
using namespace std;

class AosRoundRobin : public AosTransDistributor	
{
	OmnDefineRCObject

private:
	vector<u32>			  mSvrIds;
	vector<u32>::iterator mCrtItr;

public:
	//AosRoundRobin(const AosServerInfo::Type type);
	AosRoundRobin();
	~AosRoundRobin();

	// TransDistributor Interface
	virtual int routeReq(const u64 &dist_id);
};
#endif

