////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: Tcp.h
// Description:
//   
//
// Modification History:
// 08/05/2007	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Omn_TcpLoop_TcpLoopGroup_h
#define Omn_TcpLoop_TcpLoopGroup_h

#include "TcpLoop/Ptrs.h"
#include "TcpLoop/TcpLoop.h"
#include "Util/DynArray.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/IpAddr.h"


class OmnTcpLoopGroup : virtual public OmnRCObject 
{
public:
	OmnDefineRCObject;

	enum
	{
		eMaxLoops = 500
	};

private:
	OmnDynArray<OmnTcpLoopPtr>	mLoops;

public:
	OmnTcpLoopGroup(const OmnIpAddr &remoteAddr, 
			   const OmnIpAddr &remoteMask,
			   const OmnIpAddr &clientAddr,
			   const OmnIpAddr &clientMask,
			   const OmnIpAddr &serverAddr,
			   const OmnIpAddr &serverMask,
			   const u32 numLoops,
			   const OmnTcpLoop::BounceMethod method, 
			   const u32 sendBlockSize, 
			   const int sendSize,
			   const u32 sendWaitSec, 
			   const u32 sendWaitUsec);
	virtual ~OmnTcpLoopGroup();

	bool		connect(OmnString &errmsg);
	bool		start();
	u64			getTotalReceived() const;
	u64			getLastTotal() const;
	void		resetLastTotal();

private:
};
#endif

