////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: RetransTrans.cpp
// Description:
//	This object implements the retransmission. Retransmission is
//  defined as:
//		First: 	500ms
//		Second:	2000ms
//		Third:	4000ms
//		Fourth:	8000ms
//		Fifth:	12000ms
//		Sixth:	16000ms    
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#include "RetransMgr/RetransTrans.h"

#include "Alarm/Alarm.h"
#include "Debug/Debug.h"
#include "Message/Msg.h"
#include "RetransMgr/RetransRequester.h"
#include "UtilComm/CommGroup.h"
#include "Util1/Time.h"



OmnRetransTrans::OmnRetransTrans(const OmnMsgPtr &msg,
								 const OmnRetransRequesterPtr &requester,
								 const OmnRetransSchedule &retransSchedule,
								 void *userData)
:
mSchedule(retransSchedule),
mState(0),
mMsg(msg),
mRequester(requester),
// mStartTime(msec),			Not implemented yet
mIsValid(true),
mUserData(userData)
{
	// 
	// mStartTime not set yet
	//
	OmnNotImplementedYet;
}


OmnRetransTrans::~OmnRetransTrans()
{
}


bool
OmnRetransTrans::isRetransTriggered(const int64_t &msec)
{
	//
	// It checks whether a new retransmission is triggered.
	//
	if (mState < 0 || mState >= mSchedule.entries())
	{
		OmnAlarm << "Invalid state: " << mState
			<< ":" << mSchedule.entries() << enderr;
		return false;
	}

	return (msec - mStartTime >= mSchedule[mState]);
}
