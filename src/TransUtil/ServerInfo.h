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
//
// Modification History:
// 2012/05/23	Created by Ketty 
////////////////////////////////////////////////////////////////////////////
#ifndef Omn_TransUtil_ServerInfo_h
#define Omn_TransUtil_ServerInfo_h

#include "TransServer/Ptrs.h"
#include "Util/RCObjImp.h"
#include "Util/String.h"



class AosServerInfo: virtual public OmnRCObject
{
	OmnDefineRCObject;

private:
	int			mServerId;
	OmnString	mAddr;
	int			mPort;
	OmnString 	mMountDir;

public:
	AosServerInfo(
			const int serverId, 
			const OmnString &addr, 
			const int port,
			const OmnString &mountDir)
	:
	mServerId(serverId),
	mAddr(addr),
	mPort(port),
	mMountDir(mountDir)
	{}

	OmnString	getAddr() const { return mAddr; };
	OmnString 	getMountDir() const { return mMountDir; };
	int			getPort() const { return mPort; };

	struct AosServerInfoCmp{ 
		bool operator() (const AosServerInfoPtr &i,const AosServerInfoPtr &j) 
		{ 
			return i->mServerId <  j->mServerId; 
		} 
	};
};
#endif

