////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: NetRoute.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_Network_NetRoute_h
#define Omn_Network_NetRoute_h

#include "Message/MsgId.h"
#include "Network/NetEtyType.h"
#include "UtilComm/Ptrs.h"
#include "Util/IpAddr.h"

struct OmnNetRoute
{
	int					mTargetId;
	OmnNetEtyType::E	mType;
    OmnCommGroupPtr 	mCommGroup;
    OmnIpAddr       	mRemoteIpAddr;
    int             	mRemotePort;
    bool            	mIsDirect;
	OmnMsgId::E			mProtocol;
};
#endif

