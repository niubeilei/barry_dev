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
// 27/07/2012 	Created by Ketty 
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_TransUtil_AosServerFix_h
#define AOS_TransUtil_AosServerFix_h

#include "TransUtil/TransDistributor.h"

class AosServerFix : public AosTransDistributor	
{
	OmnDefineRCObject;
private:
	int		mNumServers;

public:
	AosServerFix(const int numphy);
	~AosServerFix();

	// TransDistributor Interface
	virtual int routeReq(const u64 &server_id);
	virtual void  setServerNum(u32 num) { mNumServers = num;}; 
	virtual u32  getServerNum(){ return mNumServers;};

};
#endif

