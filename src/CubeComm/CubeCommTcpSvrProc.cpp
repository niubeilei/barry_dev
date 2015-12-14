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
// 2014/11/08 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "CubeComm/CubeCommTcpSvrProc.h"

#include "Alarm/Alarm.h"
#include "Debug/Debug.h"
#include "Rundata/Rundata.h"
#include "Util/OmnNew.h"


AosCubeCommTcpSvrProc::AosCubeCommTcpSvrProc(const OmnCommListenerPtr &caller)
:
mCaller(caller)
{
}


AosCubeCommTcpSvrProc::AosCubeCommTcpSvrProc(AosCubeCommTcpSvrProc &rhs)
:
mCaller(rhs.mCaller)
{
}


AosCubeCommTcpSvrProc::~AosCubeCommTcpSvrProc()
{
}


AosNetReqProcPtr
AosCubeCommTcpSvrProc::clone()
{
	AosNetReqProcPtr proc = OmnNew AosCubeCommTcpSvrProc(*this);
	return proc;
}


bool
AosCubeCommTcpSvrProc::procRequest(const OmnConnBuffPtr &connbuff)
{
	aos_assert_r(mCaller, false);
	return mCaller->msgRead(connbuff);
}

