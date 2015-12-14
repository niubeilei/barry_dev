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
// 2013/03/02 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "CloudCube/CubeMgr.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "CloudCube/CubeMap.h"
#include "CloudCube/CubeMessager.h"
#include "SingletonClass/SingletonImpl.cpp"


OmnSingletonImpl(AosCubeMgrSingleton,
                 AosCubeMgr,
                 AosCubeMgrSelf,
                "AosCubeMgr");


AosCubeMgr::AosCubeMgr()
{
}


AosCubeMgr::~AosCubeMgr()
{
}


bool
AosCubeMgr::start()
{
	return true;
}


bool
AosCubeMgr::config(const AosXmlTagPtr &config)
{
	mCubeMsgRouter = OmnNew AosCubeMsgRouter();
	mCubeMsgRouter->config(config);
	return true;
}


bool
AosCubeMgr::stop()
{
    return true;
}


AosTrans1Ptr 
AosCubeMgr::sendMsg(
		const AosRundataPtr &rdata,
		const u32 cubeid,
		const OmnString &msg,
		const AosCubeCallerPtr &caller)
{
	return mCubeMsgRouter->sendMsg(rdata, cubeid, msg, caller);
}


AosTrans1Ptr 
AosCubeMgr::sendMsg(
		const AosRundataPtr &rdata,
		const u32 cubeid,
		const AosBuffPtr &msg,
		const AosCubeCallerPtr &caller)
{
	return mCubeMsgRouter->sendMsg(rdata, cubeid, msg, caller);
}


bool 
AosCubeMgr::registerMsgProc(const AosCubeMsgProcPtr &proc)
{
	return mCubeMsgRouter->registerMsgProc(proc);
}

