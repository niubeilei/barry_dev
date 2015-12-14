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
// 2015/05/29 Modified by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "Debug/ErrorMsg.h"

#include "Message/Msg.h"
#include "Porting/GetTime.h"
#include "Thread/Mutex.h"
#include "UtilComm/ConnBuff.h"
#include "UtilComm/CommGroup.h"



static uint sgErrSeqno = 10;		// Used to generate error seqno

AosErrMsg::AosErrMsg()
{
	mErrSeqno = sgErrSeqno++;
}


AosErrMsg::AosErrMsg(const OmnString &errid)
{
	mErrSeqno = sgErrSeqno++;
	mContents << "Time: " << OmnGetTime() << '\n'
		<< "ErrorID: " << errid << '\n';
}


AosErrMsg::~AosErrMsg()
{
}


AosErrMsg &
AosErrMsg::field(const OmnString &name, const u64 value)
{
	mContents << name << ": " << value << "\n";
	return *this;
}


AosErrMsg &
AosErrMsg::field(const OmnString &name, const OmnString &value)
{
	mContents << name << ": " << value << "\n";
	return *this;
}


