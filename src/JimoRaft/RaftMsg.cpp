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
#include "JimoRaft/RaftMsg.h"

#include "JimoRaft/RaftServer.h"
#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "Porting/Sleep.h"
#include "Porting/TimeOfDay.h"

static u32 sgTotal = 0;

AosRaftMsg::AosRaftMsg(
		RaftMsgType msgType,
		AosRaftServer *server)
{
	aos_assert(server);

	mMsgType = msgType;
	mRole = (int)server->getRole();
	mSenderId = server->getServerId();
	mCurTermId = server->getCurTermId();
	mLastLogId = server->getLastLogId();
	mCommitIndex = server->getCommitIndex();
	mLastLogIdApplied = server->getLastLogIdApplied();
	mTimeTag = OmnGetTimestamp();
	mReqTimeTag = 0; //means no request time tag

	mServer = server;

	sgTotal++;
	//if (sgTotal % 100 == 10)
	//	OmnScreen << "Total messages: " << sgTotal << endl;
}

//all the member values will be set later on
AosRaftMsg::AosRaftMsg()
{
	mServer = NULL;

	sgTotal++;
	//if (sgTotal % 100 == 10)
	//	OmnScreen << "Total messages: " << sgTotal << endl;
}

AosRaftMsg::~AosRaftMsg()
{
	sgTotal--;
	//if (sgTotal % 100 == 10)
	//	OmnScreen << "Total messages: " << sgTotal << endl;
}

bool
AosRaftMsg::serializeTo(
		AosRundata*  rdata,
		AosBuff *buff)
{
	aos_assert_r(buff, false);

	buff->setU32(mMsgType);
	buff->setU32(mSenderId);
	buff->setU32(mCurTermId);
	buff->setU64(mLastLogId);
	buff->setU64(mCommitIndex);
	buff->setU64(mLastLogIdApplied);
	buff->setInt(mRole);
	buff->setU64(mTimeTag);
	buff->setU64(mReqTimeTag);

	return true;
}

bool
AosRaftMsg::serializeFrom(
		AosRundata*  rdata,
		AosBuff *buff)
{
	aos_assert_r(buff, false);

	mMsgType = (RaftMsgType)buff->getU32(0);
	mSenderId = buff->getU32(0);
	mCurTermId = buff->getU32(0);
	mLastLogId = buff->getU64(0);
	mCommitIndex = buff->getU64(0);
	mLastLogIdApplied = buff->getU64(0);
	mRole = buff->getInt(0);
	mTimeTag = buff->getU64(0);
	mReqTimeTag = buff->getU64(0);

	return true;
}

////////////////////////////////////////////////
//       helper methods
////////////////////////////////////////////////
OmnString
AosRaftMsg::getMsgStr()
{
	return getMsgStr(mMsgType);
}

OmnString 
AosRaftMsg::getMsgStr(RaftMsgType msgType)
{
	OmnString str = "";

	switch (msgType)
	{
		case eMsgAppendEntryReq:
			str << "MsgAppendEntryReq";
			break;

		case eMsgAppendEntryRsp:
			str << "MsgAppendEntryRsp";
			break;

		case eMsgVoteReq:
			str << "MsgVoteReq";
			break;

		case eMsgVoteRsp:
			str << "MsgVoteRsp";
			break;

		case eMsgHeartbeat:
			str << "MsgHeartbeat";
			break;

		default:
			str << "Invalid";
			break;
	}

	return str;
}

OmnString
AosRaftMsg::toString()
{
	OmnString str = "";

	str <<"(TimeTag=" << mTimeTag 
		<<" ReqTimeTag=" << mReqTimeTag 
		<<" MsgType=" << getMsgStr(mMsgType) 
	    <<" mSenderId=" << mSenderId
		<<" mCurTermId=" << mCurTermId
		<<" mLastLogId=" << mLastLogId
		<<" mLastLogIdApplied=" << mLastLogIdApplied
		<<" mCommitIndex=" << mCommitIndex
		<<" Role=" << AosRaftServer::getRoleStr((AosRaftServer::RaftRole)mRole)
		<<") ";

	return str;
}
