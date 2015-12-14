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
// 4/16/2007: Created by cding
////////////////////////////////////////////////////////////////////////////
#include "AmClient/ApiUserBased.h"

#include "alarm/Alarm.h"
#include "Alarm/Alarm.h"
#include "AmClient/AmServer.h"
#include "AmClient/AmClient.h"
#include "AmUtil/AmMsg.h"
#include "AmUtil/AmRespCode.h"
#include "Thread/ThreadShell.h"
#include "Thread/ThrdShellProc.h"
#include "Thread/ThrdTrans.h"
#include "UtilComm/ConnBuff.h"



AosApiUserBased::AosApiUserBased(
				const OmnString &app, 
				const OmnString &user, 
				const OmnString &opr, 
				const OmnString &rsc, 
				const u16 transId, 
				const u16 sessionId, 
				const u32 timer,
				const AosAmServerPtr &server, 
				const AosAmClientPtr &client,
				AosAmApiCallback callback)
:
mApp(app),
mUser(user),
mOpr(opr),
mRsc(rsc),
mTransId(transId),
mSessionId(sessionId),
mTimer(timer),
mServer(server),
mClient(client),
mCallback(callback)
{
}


AosApiUserBased::~AosApiUserBased()
{
}


bool
AosApiUserBased::proc(const OmnThreadShellPtr &shell)
{
	if (!mServer)
	{
		OmnString errmsg = "No available server";
		mCallback(false, AosAmRespCode::eInternalError, errmsg);
		OmnAlarm << errmsg << enderr;
		return false;
	}

	// 
	// It sends an Access Request to one of the servers. 
	//
	AosAmMsg req(500);
	req.setTransId(mTransId);
	req.setMsgId(AosAmMsgId::eUserBasedAccess);
	req.setSessionId(mSessionId);
	aos_assert_r(req.addTag(AosAmTagId::eApp, mApp), false);
	aos_assert_r(req.addTag(AosAmTagId::eUser, mUser), false);
	aos_assert_r(req.addTag(AosAmTagId::eOpr, mOpr), false);
	aos_assert_r(req.addTag(AosAmTagId::eRsc, mRsc), false);

	if (!mClient->sendMsg(req, mServer))
	{
		OmnString errmsg = "Failed to send request";
		mCallback(false, AosAmRespCode::eInternalError, errmsg);
		OmnAlarm << errmsg << enderr;
		return false;
	}

	OmnThrdTrans trans(mTransId);
	OmnThrdTransPtr transPtr(&trans, false);
	if (!shell->waitForMsg(transPtr, mTimer))
	{
		OmnString errmsg = "Failed to retrieve response";
		mCallback(false, AosAmRespCode::eInternalError, errmsg);
		OmnAlarm << errmsg << enderr;
		return false;
	}

	AosAmMsg resp(trans.getBuff());
	if (resp.isRespPositive()) 
	{
		mCallback(true, AosAmRespCode::eAllowed, "");
		return true;
	}

	AosAmRespCode::E respCode = (AosAmRespCode::E)resp.getRespCode();
	OmnString errmsg = resp.getErrmsg();
	mCallback(false, respCode, errmsg);
	return true;
}

