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
#ifndef AOS_TransUtil_SeLogReqRobin_h
#define AOS_TransUtil_SeLogReqRobin_h

#include "SEInterfaces/ServerInfo.h"
#include "TransUtil/TransDistributor.h"

#include <vector>
using namespace std;

class AosSeLogReqRobin : public AosTransDistributor	
{
	OmnDefineRCObject;

private:
	int 				  mNumVirs;
	vector<u32>			  mSvrIds;
	vector<u32>::iterator mCrtItr;

public:
	//AosSeLogReqRobin(const AosServerInfo::Type type);
	AosSeLogReqRobin();
	~AosSeLogReqRobin();

 	// TransDistributor Interface
	virtual int routeReq(const u64 &dist_id);

};
#endif

