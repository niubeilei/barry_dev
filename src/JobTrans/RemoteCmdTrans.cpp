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
// 05/10/2013	Created by Young Pan 
////////////////////////////////////////////////////////////////////////////
#include "JobTrans/RemoteCmdTrans.h"

#include "SystemMonitor/RunCmd.h"
#include "XmlUtil/XmlTag.h"
#include "SEInterfaces/NetFileMgrObj.h"
#include "CommandProc/CommandProc.h"

AosRemoteCmdTrans::AosRemoteCmdTrans(const bool regflag)
:
AosTaskTrans(AosTransType::eRemoteCmd, regflag)
{
}


AosRemoteCmdTrans::AosRemoteCmdTrans(
		const OmnString &cmd, 		// the command string in buff
		const int svr_id)           // remote server ID
:
AosTaskTrans(AosTransType::eRemoteCmd, svr_id, false, true),
mCmd(cmd)
{
}


AosRemoteCmdTrans::~AosRemoteCmdTrans()
{
}


bool
AosRemoteCmdTrans::serializeFrom(const AosBuffPtr &buff)
{
	bool rslt = AosTaskTrans::serializeFrom(buff);
	aos_assert_r(rslt, false);

	mCmd = buff->getOmnStr("");
	aos_assert_r(mCmd != "", false);
	return true;
}


bool
AosRemoteCmdTrans::serializeTo(const AosBuffPtr &buff)
{
	bool rslt = AosTaskTrans::serializeTo(buff);
	aos_assert_r(rslt, false);

	buff->setOmnStr(mCmd);
	return true;
}


AosTransPtr
AosRemoteCmdTrans::clone()
{
	return OmnNew AosRemoteCmdTrans(false);
}


bool
AosRemoteCmdTrans::proc()
{
	// Run command and return the result.
	/*
	OmnString resp;

	FILE* ff = popen(mCmd.data(), "r");
	aos_assert_r(ff != NULL, false);

	while (!feof(ff))
	{   
		char buff[1024]; 
		int len = fread(buff, 1, 1024, ff);
		buff[len-1]=0;
		resp << buff;
	} 
	pclose(ff); 
	*/

	AosRunCmd *runcmd = AosRunCmd::getSelf();
	OmnString resp = "";
	if (mCmd == "cancel")
	{
		bool rslt = runcmd->cancel();
		if (rslt)
		{
			resp = "Tcancel the command successful!";
		}
		else
		{
			resp = "Fcancel the command not successful!";
		}
	}
	else
	{
		resp = runcmd->run(mCmd);
	}

	// create by Young 2013/08/21
	if (resp == "") resp = " "; 
	AosBuffPtr buff = OmnNew AosBuff(resp.length() AosMemoryCheckerArgs);
	buff->setOmnStr(resp);
	sendResp(buff);

	return true;
}
	

