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
// 3/30/2007: Created by cding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_AmClient_AmServer_h
#define Aos_AmClient_AmServer_h

#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/IpAddr.h"
#include "Util/String.h"
#include "UtilComm/Ptrs.h"



class AosAmServer : virtual public OmnRCObject
{
	OmnDefineRCObject;

private: 
	OmnIpAddr		mAddr;
	u16				mPort;
	u32				mNumPorts;
	OmnString		mName;
	OmnTcpClientPtr	mConn;
	u32				mUseCount;
	bool			mIsGood;
	
public:
	AosAmServer();
	AosAmServer(const OmnIpAddr &addr, 
				const u16 port, 
				const u32 numPorts, 
				const OmnString &name);
	virtual ~AosAmServer();

	bool		sendMsg(char *buff, u32 len);
	bool		isGood() const {return mIsGood;}
	u32			getUseCount() const {return mUseCount;}
	bool		receiveResp(OmnConnBuffPtr &buff, const u32 timer);
	bool		receiveResp(OmnConnBuffPtr &buff);
	bool		connect();
	bool		stop();
	OmnString	toString() const;

	bool operator == (const AosAmServer &rhs) const;
	int 		getSock() const ;

private:

};
#endif

