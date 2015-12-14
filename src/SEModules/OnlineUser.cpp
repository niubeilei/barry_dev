////////////////////////////////////////////////////////////////////////////

// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// Description:
// This class is used to manage an online user. There shall be one
// instance of this class for each of the online users. When an online
// user wants to pull messages, it sends a request, which will eventually
// call the function: readMsg(...). If there are already messages, it
// reads the messages and returns. Otherwise, it will wait there. 
//
// When someone has something to send to an online user, it will call
// the member function msgReceived(...). If the user was trying to read
// messags, this function will wake up the user, which will read the
// messages and return.
//
// Modification History:
// 11/22/2010: Created by James
////////////////////////////////////////////////////////////////////////////
#include "SEModules/OnlineUser.h"



AosOnlineUser::AosOnlineUser()
:
mLock(OmnNew OmnMutex()),
mCondVar(OmnNew OmnCondVar()),
mWaitTimeSec(eDefaultWaitSec)
{

}


AosOnlineUser::~AosOnlineUser()
{
}


bool
AosOnlineUser::readMsg(OmnString &msg, bool &istimeout)
{
	// This function checks whether there are outstanding messages.
	// If yes, it returns the messages and returns. Otherwise, 
	// it will wait for messages. 
	msg = "";
	while (1)
	{
		mLock->lock();
		if (mMsgs.empty()) 
		{
			mCondVar->timedWait(mLock, istimeout, mWaitTimeSec);
			mLock->unlock();
			if (istimeout)
			{
				return true;
			}
			continue;
		}
		msg = mMsgs.front();
		mMsgs.pop();
		mLock->unlock();
		return true;
	}

	OmnShouldNeverComeHere;
	return false;
}

bool
AosOnlineUser::sendMsg(const OmnString &msg)
{
	mLock->lock();
	mMsgs.push(msg);
	mCondVar->signal();
	mLock->unlock();
	return true;
}

