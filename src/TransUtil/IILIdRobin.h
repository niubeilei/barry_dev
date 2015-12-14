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
#ifndef AOS_TransUtil_AosIILIdRobin_h
#define AOS_TransUtil_AosIILIdRobin_h

#include "TransUtil/TransDistributor.h"

class AosIILIdRobin : public AosTransDistributor	
{
	OmnDefineRCObject

private:
	int		mNumServers;
	int 	mVriNumServers;
	int		mCrtServerNum;

public:
	AosIILIdRobin();
	~AosIILIdRobin();

	// TransDistributor Interface
	virtual int		routeReq(const u64 &distid);
};
#endif

