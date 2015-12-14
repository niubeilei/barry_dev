////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// Modification History:
// Created: 05/08/2013 by Ketty 
////////////////////////////////////////////////////////////////////////////
#include "SysMsg/RebootProcMsg.h"

#include "API/AosApi.h"
#include "SEInterfaces/TransSvrObj.h"
#include "SEInterfaces/VfsMgrObj.h"
//#include "SEInterfaces/FmtMgrObj.h"
#include "SEInterfaces/IpcCltObj.h"


AosRebootProcMsg::AosRebootProcMsg(const bool reg_flag)
:
AosAppMsg(MsgType::eRebootProcMsg, reg_flag)
{
}


AosRebootProcMsg::AosRebootProcMsg(
		const int to_svrid,
		const u32 to_proc_id)
:
AosAppMsg(MsgType::eRebootProcMsg, to_svrid, to_proc_id)
{
}


AosRebootProcMsg::~AosRebootProcMsg()
{
}


bool
AosRebootProcMsg::serializeFrom(const AosBuffPtr &buff)
{
	bool rslt = AosAppMsg::serializeFrom(buff);
	aos_assert_r(rslt, false);

	return true;
}


bool
AosRebootProcMsg::serializeTo(const AosBuffPtr &buff)
{
	bool rslt = AosAppMsg::serializeTo(buff);
	aos_assert_r(rslt, false);

	return true;
}


AosConnMsgPtr
AosRebootProcMsg::clone2()
{
	return OmnNew AosRebootProcMsg(false);
}


bool
AosRebootProcMsg::proc()
{
	OmnScreen << "To reboot process!!" << endl;
	OmnScreen << "To stop TransServer!" << endl;
	AosTransSvrObj::getTransSvr()->stop();	// Ketty 2013/03/12

	OmnScreen << "To stop VirtualFileSysMgr!" << endl;
	AosVfsMgrObj::getVfsMgr()->stop();

	//OmnScreen << "To stop FmtMgr!" << endl;
	//AosFmtMgrObj::getFmtMgr()->stop();

	OmnScreen << "To IpcClt!" << endl;
	AosIpcCltObj::getIpcClt()->stop();
	
	OmnScreen << "All Stopped!" << endl;
	OmnScreen << "Exiting!" << endl;
	
	exit(0);
}

