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
#ifndef AOS_TransUtil_AosIdRobin_h
#define AOS_TransUtil_AosIdRobin_h

#include "TransUtil/TransDistributor.h"
//#include "SEInterfaces/ServerInfo.h"

class AosIdRobin : public AosTransDistributor	
{
	OmnDefineRCObject;
private:
	int 	mNumVirtuals;
	int		mCrtVirServerNum;
	//AosServerInfo::Type mSvrType;

public:
	AosIdRobin();
	//AosIdRobin(const AosServerInfo::Type type);
	~AosIdRobin();

	// TransDistributor Interface
	virtual int routeReq(const u64 &dist_id);

};
#endif

