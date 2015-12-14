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
// 09/17/2013	Created by Young
////////////////////////////////////////////////////////////////////////////
#include "AdminTrans/AdminRunCmdTrans.h"

#include "XmlUtil/XmlTag.h"
#include "XmlUtil/SeXmlParser.h"
#include "SysMsg/StartProcessMsg.h"
#include "Util/File.h"
#include "CommandProc/CommandProc.h"
#include "SystemMonitor/RunCmd.h"

AosAdminRunCmdTrans::AosAdminRunCmdTrans(const bool regflag)
:
AosAdminTrans(AosTransType::eRemoteCmd, regflag)
{
}


AosAdminRunCmdTrans::AosAdminRunCmdTrans(
		const OmnString &cmd,
		const int svr_id,
		const bool need_save,
		const bool need_resp)
:
AosAdminTrans(AosTransType::eRemoteCmd, svr_id, need_save, need_resp),
mCmd(cmd)
{
}


AosAdminRunCmdTrans::~AosAdminRunCmdTrans()
{
}


bool
AosAdminRunCmdTrans::serializeFrom(const AosBuffPtr &buff)
{
	bool rslt = AosAdminTrans::serializeFrom(buff);
	aos_assert_r(rslt, false);

	mCmd = buff->getOmnStr("");
	aos_assert_r(mCmd != "", false);

	return true;
}


bool
AosAdminRunCmdTrans::serializeTo(const AosBuffPtr &buff)
{
	bool rslt = AosAdminTrans::serializeTo(buff);
	aos_assert_r(rslt, false);

	buff->setOmnStr(mCmd);
	return true;
}


AosTransPtr
AosAdminRunCmdTrans::clone()
{
	return OmnNew AosAdminRunCmdTrans(false);
}


bool
AosAdminRunCmdTrans::proc()
{
OmnScreen << "****************************AosAdminRunCmdTrans**********************" << endl;
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

	if (resp == "") resp = " "; 
	AosBuffPtr buff = OmnNew AosBuff(resp.length() AosMemoryCheckerArgs);
	buff->setOmnStr(resp);
	sendResp(buff);
	mRdata->setOk();

	return true;
}


