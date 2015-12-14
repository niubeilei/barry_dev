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
// 2014/03/07	Created by Ketty 
////////////////////////////////////////////////////////////////////////////
#include "SysMsg/StartJobMgrMsg.h"

#include "API/AosApi.h"
#include "SEInterfaces/JobMgrObj.h"
#include "SEInterfaces/SvrProxyObj.h"
#include "SEInterfaces/TransSvrObj.h"

AosStartJobMgrMsg::AosStartJobMgrMsg(const bool regflag)
:
AosAppMsg(MsgType::eStartJobMgrMsg, regflag)
{
}


AosStartJobMgrMsg::AosStartJobMgrMsg()
:
AosAppMsg(MsgType::eStartJobMgrMsg, AosGetSelfServerId(), AosProcessType::eFrontEnd, -1)
{
}


AosStartJobMgrMsg::~AosStartJobMgrMsg()
{
}


bool
AosStartJobMgrMsg::serializeFrom(const AosBuffPtr &buff)
{
	bool rslt = AosAppMsg::serializeFrom(buff);
	aos_assert_r(rslt, false);
	return true;
}


bool
AosStartJobMgrMsg::serializeTo(const AosBuffPtr &buff)
{
	bool rslt = AosAppMsg::serializeTo(buff);
	aos_assert_r(rslt, false);
	return true;
}


AosConnMsgPtr
AosStartJobMgrMsg::clone2()
{
	return OmnNew AosStartJobMgrMsg(false);
}


bool
AosStartJobMgrMsg::proc()
{
	AosJobMgrObj::getJobMgr()->tryStartJob();
	return true;
}


