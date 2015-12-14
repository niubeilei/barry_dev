////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: CommMgr.cpp
// Description:
//	This is a singleton class that manages the entire application's 
//  sending and receiving over TCP/UDP.    
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#include "UtilComm/CommMgr.h"


#include "Message/Msg.h"
#include "Porting/Sleep.h"
#include "Thread/Mutex.h"
#include "Thread/CondVar.h"
#include "Thread/Thread.h"
#include "Util/OmnNew.h"
#include "UtilComm/Comm.h"
#include "UtilComm/ConnBuff.h"
#include "UtilComm/CommGroup.h"
#include "UtilComm/CommListener.h"





OmnCommMgr::OmnCommMgr()
{
	mCommLock = OmnNew OmnMutex();
}


OmnCommMgr::~OmnCommMgr()
{
	//
	// Stop all the reading threads
	//
	mCommLock->lock();
	const int s = mComms.entries();
	mComms.reset();
	for (int i=0; i<s; i++)
	{
		(mComms.crtValue())->forceStop();
	}
	mCommLock->unlock();

	//
	// Need to wait for 3 seconds before all threads stopped
	//
	OmnSleep(3);
}


/*


OmnRslt
OmnCommMgr::sendTo(const OmnMsgPtr &msg)
{
	//
	// Call this function to send a message. The message must already
	// contain sending and receiving IP addresses and ports, NIID.
	// This function checks whether there is any OmnComm supporting 
	// that NIID. If not, it is an error. This function will create
	// an error message and sends the error message ID to msg.
	//
	// If yes, it will ask that OmnComm to send the message, which 
	// may or may not be successful.
	//

	//
	// Retrieve the NIID
	//
	int niid = msg->getSendingNIID();
	if (niid <= 0)
	{
		//
		// Invalid NIID
		//
		int errid = OmnError::log(OmnFileLine, msg->getSeqno(), OmnErrId::eIncorrectNIID);
		msg->setErrSeqno(errid);
		return errid;
	}

	//
	// Retrieve the Comm
	//
	OmnCommPtr comm = getCommByNIID(niid);
	if (comm.isNull())
	{
		//
		// Failed to find the communicator for that NIID.
		//
		int errid = OmnError::log(OmnFileLine, msg->getSeqno(),
			OmnErrId::eFailedToRetrieveCommunicatorForGivenNIID);
		msg->setErrSeqno(errid);
		return errid;
	}

	//
	// Ask the comm to send it.
	//
	return comm->sendTo(msg);
}
*/


bool
OmnCommMgr::addComm(const OmnCommPtr &comm)
{
	//
	// It does the following:
	//
	// 1. Make sure it is not already in this class
	// 2. Add the comm in the queue
	//

	//
	// 1. Make sure it is not already in.
	//
	mCommLock->lock();
	const int s = mComms.entries();
	mComms.reset();
	for (int i=0; i<s; i++)
	{
		if (comm == mComms.crtValue())
		{
			//
			// It is already there
			//
			mCommLock->unlock();
			OmnAlarm << "Comm Already Exist In CommMgr" << enderr;
			return false;
		}
	}

	//
	// 2. Add the comm in
	//
	mComms.append(comm);
	mCommLock->unlock();

	return true;
}


bool
OmnCommMgr::removeComm(const OmnCommPtr &comm)
{
	//
	// 1. Remove it from mComms
	// 2. Remove the reading thread
	//
	mCommLock->lock();
	const int s = mComms.entries();
	mComms.reset();
	for (int i=0; i<s; i++)
	{
		if (mComms.crtValue() == comm)
		{
			mComms.eraseCrt();
			mCommLock->unlock();
			return true;
		}
	}

	//
	// Did not find it.
	//
	mCommLock->unlock();
	return false;
}


/*
bool 
OmnCommMgr::threadFunc(OmnThrdStatus::E &state, const OmnThreadPtr &thread)
{
	//
	// Currently all threads are reading threads. 
	//
	switch (thread->getLogicId())
	{
	case eCommRestoreThreadId:
		 return restoreCommThreadFunc(state, thread);

	default:
		 OmnError::log(OmnFileLine, OmnErrId::eUnrecognizedThraedId,
			OmnStrUtil::itoa(thread->getLogicId()));
		 return false;
	}
}
*/


/*
bool
OmnCommMgr::readingThreadFunc(OmnThrdStatus::E &state, const OmnThreadPtr &thread)
{
	OmnTrace << "Enter OmnCommMgr::readingThreadFunc()." << endl;

	OmnConnBufferPtr buff;
	while (state == OmnThrdStatus::eActive)
	{
		//
		// Retrieve the comm for this thread.
		//
		int threadLogicId = thread->getLogicId();
		OmnCommPtr comm = getCommByThreadLogicId(threadLogicId);
		if (comm.isNull())
		{
			//
			// Failed to retrieve the comm.
			//
			int errid = OmnError::log(OmnFileLine, 0, OmnErrId::eNoCommForCommMgrReadingThread);
			thread->setErrSeqno(errid);
			return false;
		}

		//
		// Found the comm. Need to read from that comm
		//
		while (state == OmnThrdStatus::eActive)
		{
			if (!comm->readFrom(buff, eReadTimerSec).mStatus)
			{
				//
				// Failed to read
				//
				OmnError::log(OmnFileLine, 0, comm->getLastReadErrId(),
					comm->getLastReadErrMsg());

				//
				// Currently we wait for eReadFailIntervalTimerSec seconds
				// and then try it again.
				//
				OmnSleep(eReadFailIntervalTimerSec);
				continue;
			}

			if (buff->getDataLength() == 0)
			{
				//
				// It is timeout
				//
				OmnTrace << "Reading timeout " << endl;
				continue;
			}

			//
			// It read successfully. Distributed according to the message category.
			// 
			switch (buff->getMsgCat())
			{
			case OmnMsgId::eMgcpMsg:
				 addMgcpMsg(OmnMgcpMsg::createMgcpMsg(buff));
				 OmnTrace << "Received an MGCP message" << endl;
				 break;

//			case OmnMsgId::eNMSMsg:
//				 addNmsMsg(OmnNmsMsgPtr((OmnNmsMsg*)msg.getPtr()));
//				 break;

			default:
				 OmnError::log(OmnFileLine, OmnErrId::eNotImplementedYet);
			}
		}
	}

	OmnTrace << "Leave OmnCommMgr::readingThreadFunc()" << endl;
	return true;
}

bool 
OmnCommMgr::signal(const int threadLogicId)
{
	//
	// Currently there is nothing this function needs to do
	//
	return true;
}
*/


OmnCommPtr
OmnCommMgr::getCommByNIID(const int niid)
{
	//
	// It retrieves a comm by NIID. 
	//
	mCommLock->lock();
	const int s = mComms.entries();
	mComms.reset();
	for (int i=0; i<s; i++)
	{
		if ((mComms.crtValue())->getNiid() == niid)
		{
			//
			// Found it.
			//
			OmnCommPtr comm = mComms.crtValue();
			mCommLock->unlock();
			return comm;
		}
	}

	//
	// Did not find it.
	//
	mCommLock->unlock();
	return 0;
}


/*
void
OmnCommMgr::addMgcpMsg(const OmnMgcpMsgPtr &msg)
{
	mMgcpQueueLock->lock();
	mMgcpQueue.append(msg);
	mMgcpQueueCondVar->signal();
	mMgcpQueueLock->unlock();
}


void
OmnCommMgr::addNmsMsg(const OmnNmsMsgPtr &msg)
{
	mNmsQueueLock->lock();
	mNmsQueue.append(msg);
	mNmsQueueCondVar->signal();
	mNmsQueueLock->unlock();
}


void
OmnCommMgr::addMiscMsg(const OmnMsgPtr &msg)
{
	mMiscQueueLock->lock();
	mMiscQueue.append(msg);
	mMiscQueueCondVar->signal();
	mMiscQueueLock->unlock();
}
*/

bool 
OmnCommMgr::restoreCommThreadFunc(OmnThrdStatus::E &state, const OmnSPtr<OmnThread> &thread)
{
	OmnTrace << "Enter OmnCommMgr::restoreCommThreadFunc" << endl;

	/*
	while (state == OmnThrdStatus::eActive)
	{
		mCommToRestoreLock->lock();
		if (mCommToRestore.entries() <= 0)
		{
			//
			// No more to restore
			//
			mCommToRestoreCondVar->wait(mCommToRestoreLock);
			continue;
		}

		mCommToRestoreLock->unlock();
		RestoreEntry restoreEntry;
		OmnCommPtr comm;
		list<OmnCommPtr>::iterator itr = mCommToRestore.begin();
		for (uint i=0; i<mCommToRestore.entries(); i++, itr++)
		{
			mCommToRestoreLock->lock();
			//QQQQ
			if (i < mCommToRestore.entries())
			{
				restoreEntry = *itr;
				comm = restoreEntry.mComm;
				mCommToRestoreLock->unlock();
			}
			else
			{
				//
				// No more to try. Go back and try it again
				//
				mCommToRestoreLock->unlock();
				continue;
			}

			//
			// Found one to restore.
			//
			OmnRslt rslt = comm->reconnect();
			if (rslt.failed())
			{
				//
				// Failed to restore. Check whether it should remove the comm
				//
				if ((OmnCommMgr::E)rslt.getReason() != eContinueToTry)
				{
					//
					// This means the comm should be removed from the reconnection list.
					//
					mCommToRestoreLock->lock();
					for (uint k=0; k<mCommToRestore.entries(); k++)
					{
						if (mCommToRestore[k].mComm == comm)
						{
							mCommToRestore.removeAt(k);
							break;
						}
					}
					mCommToRestoreLock->unlock();
				}
				else
				{
					continue;
				}
			}
			else
			{
				//
				// Reconnection is successful. Remove the entry and call the group.
				//
				mCommToRestoreLock->lock();
				for (uint k=0; k<mCommToRestore.entries(); k++)
				{
					if (mCommToRestore[k].mComm == comm)
					{
						mCommToRestore.removeAt(k);
						mCommToRestoreLock->unlock();
						(restoreEntry.mGroup)->commRestored(comm);
						break;
					}
				}
				mCommToRestoreLock->unlock();
			}
		}				
	}
	*/

	OmnTrace << "Leave OmnCommMgr::restoreCommThreadFunc" << endl;
	return true;
}


bool        
OmnCommMgr::restoreComm(const OmnCommPtr &ni)
{
	OmnAlarm << "Not Implemented Yet" << enderr;
	return false;
}

