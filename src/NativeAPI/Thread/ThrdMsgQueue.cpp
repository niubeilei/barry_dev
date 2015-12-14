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
// 4/6/2007: Created by cding
////////////////////////////////////////////////////////////////////////////
#include "Thread/ThrdMsgQueue.h"

#include "Thread/Mutex.h"
#include "Thread/CondVar.h"
#include "Thread/ThrdTrans.h"
#include "Util/OmnNew.h"


AosThrdMsgQueue::AosThrdMsgQueue()
:
mLock(OmnNew OmnMutex()),
mRequesterLock(OmnNew OmnMutex())
{
}


AosThrdMsgQueue::~AosThrdMsgQueue()
{
}


bool
AosThrdMsgQueue::waitForMsg(const OmnThrdTransPtr &trans, const u32 timer)
{
	// 
	// This function assumes all messages contain a transaction
	// ID. This function will check whether there is any 
	// message whose transaction ID matches 'trans' ID. If yes, 
	// it retrieves the message into 'trans' and returns. 
	// Otherwise, it will wait until 'timer' expires. 
	//
	mLock->lock();
	mMsgs.reset();
	u32 transId = trans->getTransId();
	Entry entry;
	while (mMsgs.hasMore())
	{
		entry = mMsgs.crtValue();
		if (entry.mTransId == transId)
		{
			// 
			// Found the message. 
			//
			trans->msgRcved(entry.mBuff);
			mMsgs.eraseCrt();
			mLock->unlock();
			return true;
		}

		mMsgs.next();
	}

	// 
	// No messages in the queue. Need to wait. 
	//
	int index = -1;
	mRequesterLock->lock();
	for (int i=0; i<mRequesters.entries(); i++)
	{
		if (!mRequesters[i].mUsed)
		{
			index = i;
			break;
		}
	}

	if (index < 0)
	{
		// 
		// Did not find an unused entry. Append one.
		//
		Requester entry;
		entry.mLock = OmnNew OmnMutex();
		entry.mCondVar = OmnNew OmnCondVar();
		entry.mUsed = false;
		mRequesters.append(entry);
		index = mRequesters.entries()-1;
	}
	mRequesterLock->unlock();

	bool isTimedout;
	mRequesters[index].mLock->lock();
	mRequesters[index].mTransId = transId;
	mRequesters[index].mUsed = true;
	mRequesters[index].mTrans = trans;
	mRequesters[index].mCondVar->timedWait(
		mRequesters[index].mLock, isTimedout, timer);

	// 
	// The wait is waken up. It can be either timed out or 
	// a message has been received. If it is timed out, 
	// 'isTimedout' is true. 
	//
	mRequesters[index].mUsed = false;
	mRequesters[index].mLock->unlock();
	return !isTimedout;
}


bool	
AosThrdMsgQueue::msgRcved(const OmnConnBuffPtr &buff, const u32 transId)
{
	// 
	// The connection received a message. The message is stored in
	// 'buff'. The message's transaction is 'transId'. This function
	// checks whether anyone is waiting for this message. If yes, 
	// it wakes that requester. Otherwise, it puts the message into 
	// the transaction queue.
	//
	mRequesterLock->lock();
	for (int i=0; i<mRequesters.entries(); i++)
	{
		if (mRequesters[i].mUsed && mRequesters[i].mTransId == transId)
		{
			mRequesters[i].mLock->lock();
			mRequesters[i].mTrans->msgRcved(buff);
			mRequesters[i].mCondVar->signal();
			mRequesters[i].mLock->unlock();
			mRequesterLock->unlock();
			return true;
		}
	}
	mRequesterLock->unlock();

	// 
	// No one is waiting for the message. Need to put it into the
	// transaction queue.
	//
	Entry entry(buff, transId);
	mLock->lock();
	mMsgs.append(entry);
	mLock->unlock();
	return true;
}

