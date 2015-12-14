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
// 2013/01/09 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "UtilComm/TcpEpollReader.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "Debug/Debug.h"
#include "Debug/Error.h"
#include "Porting/IPv6.h"
#include "Porting/Select.h"
#include "Porting/SockLen.h"
#include "Porting/BindErr.h"
#include "Porting/GetErrnoStr.h"
#include "Porting/Sleep.h"
#include "Tracer/Tracer.h"
#include "Thread/Thread.h"
#include "Thread/Mutex.h"
#include "Util/OmnNew.h"
#include "UtilComm/TcpClient.h"
#include "UtilComm/TcpServerEpoll.h"
#include "UtilComm/Ptrs.h"
#include "UtilComm/ConnBuff.h"
#include "UtilComm/TcpListener.h"
#include "UtilComm/TcpMsgReader.h"
#include "UtilComm/CommTypes.h"
#include "Util/Buff.h"
#include "Util1/Wait.h"

#include <sys/un.h>
#include <fcntl.h>

const int cEventsLen = 10;


AosTcpEpollReader::AosTcpEpollReader(OmnTcpServerEpoll *server)
:
mLock(OmnNew OmnMutex()),
mCondVar(OmnNew OmnCondVar()),
mEpollServer(server),
mNumThreads(1),
mMaxQueueLen(0),
mTotalRead(0),
mCrtSecRead(0),
mCrtSec(0)
{
	for (int i=0; i<mNumThreads; i++)
	{
		OmnThreadedObjPtr thisPtr(this, false);
		OmnThreadPtr thread = OmnNew OmnThread(thisPtr, "TcpEpollReader", 0, false, true, __FILE__, __LINE__);
		thread->start();
		mThreads.push_back(thread);
	}
}


AosTcpEpollReader::~AosTcpEpollReader()
{
}


bool
AosTcpEpollReader::threadFunc(OmnThrdStatus::E &state, const OmnThreadPtr &thread)
{
    OmnTrace << "Enter TcpEpollReader::threadFunc." << endl;

	OmnTcpClientPtr conn;
	OmnConnBuffPtr buff;
    while (state == OmnThrdStatus::eActive )
    {
		mLock->lock();
		if (mConns.size() <= 0)
		{
			mCondVar->wait(mLock);
			mLock->unlock();
			continue;
		}

		conn = mConns.front();
		mConns.pop();
		mLock->unlock();

		while (1)
		{
			int nn = conn->smartRead(buff);
			if (nn == -1)
			{
				// Either the remote has closed the connection or something
				// wrong with the connection. We will close the connection.
				mEpollServer->closeConn(conn);
				OmnScreen << "Read error : "  << conn->getRemoteIpAddr().toString() << ":" << 
					conn->getRemotePort() << endl;
				break;
			}                                                                
		
			if (!conn->isReadReady())
			{
				// This means there are no contents in the connection now
				// and the message is not complete yet. It finishes the
				// current reading.
				break;
			}
			else
			{
				if (conn->getBlockingType())
				{
					OmnConnBuffPtr buff = conn->getDataBuff();
					aos_assert_r(mListener, false);
					mListener->msgRecved(buff, conn);
					conn->clearBuffedData();
					break;
				}
			}

			aos_assert_r(mListener, false);
			OmnConnBuffPtr buff = conn->getDataBuff();
			aos_assert_r(buff, false);
			int len = buff->getDataLength();
			if (len > 0)
			{
				u64 crt_sec = OmnTime::getCrtSec();
				mLock->lock();
				mTotalRead += (u32)len;
				if (crt_sec == mCrtSec)
				{
					mCrtSecRead += (u32)len;
				}
				else
				{
					mCrtSec = crt_sec;
					if (mCrtSecRead > 0)
					{
						mReadHistory.push(mCrtSecRead);
					}
					
					if (mReadHistory.size() > eHistoryLength)
					{
						mReadHistory.pop();
					}
					
					mCrtSecRead = (u32)len;
				}
				mLock->unlock();

//OmnScreen << "read finish " << conn->getSock() << endl;
				mListener->msgRecved(buff, conn);
			}
			conn->clearBuffedData();
		}
	}
	return true;
}


bool
AosTcpEpollReader::signal(const int threadLogicId)
{
	return true;
}


bool
AosTcpEpollReader::checkThread(OmnString &, const int tid) const
{
	// Currently not managed by ThreadMgr. Always return true
    return true;
}


void 
AosTcpEpollReader::setListener(const OmnTcpListenerPtr &listener)
{
	mListener = listener;
}


void
AosTcpEpollReader::printStatus()
{
	OmnScreen << "Read Statistics for Epool: " << mEpollServer->getName() << endl
		<< "============ Read Stat ==========" << endl
		<< "Total Read:      " << mTotalRead << endl
		<< "History: ";
	mLock->lock();
	queue<u64> history = mReadHistory;
	mLock->unlock();

	while (history.size() > 0)
	{
		u64 value = history.front();
		history.pop();
		cout << " " << value;
	}
	cout << "=================================" << endl << endl;
}
