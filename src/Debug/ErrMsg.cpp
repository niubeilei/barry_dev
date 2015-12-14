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
#if 0
#include "Debug/ErrMsg.h"

#include "Message/Msg.h"
#include "Thread/Mutex.h"
#include "UtilComm/ConnBuff.h"
#include "UtilComm/CommGroup.h"



static uint sgErrSeqno = 10;		// Used to generate error seqno

OmnErrMsg::OmnErrMsg()
{
	mErrSeqno = sgErrSeqno++;
}


OmnErrMsg::OmnErrMsg(const OmnString &errid)
{
	mErrorID = erroid;
	mErrSeqno = sgErrSeqno++;
}


OmnErrMsg::~OmnErrMsg()
{
}
#endif
