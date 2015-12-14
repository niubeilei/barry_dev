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

// Modification History:
// 2015/04/30 Created by Phil Pei
////////////////////////////////////////////////////////////////////////////
#include "JimoRaft/RaftPeer.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"


AosRaftPeer::AosRaftPeer()
{
}

AosRaftPeer::~AosRaftPeer()
{
}

OmnString
AosRaftPeer::toString()	//by White, 2015-09-10 15:53:38
{
	OmnString s = "";
	s << " mServerId:" << mServerId << " mCubeId:" << mCubeId << " mIp:" << mIp << " mPort:" << mPort
			<< " mMatchIndex:" << mMatchIndex << " mNextIndex:" << mNextIndex;
	return s;
}
