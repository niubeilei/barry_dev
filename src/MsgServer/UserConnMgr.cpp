////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// Modification History:
// 	Created: 05/09/2010 by jozhi Peng
////////////////////////////////////////////////////////////////////////////
#include "MsgServer/UserConnMgr.h"

#include "SingletonClass/SingletonImpl.cpp"
#include "Thread/Mutex.h"
#include "Thread/Thread.h"
#include "Porting/Sleep.h"
#include "MsgServer/MsgReqProc.h"

OmnSingletonImpl(AosUserConnMgrSingleton,
                 AosUserConnMgr,
                 AosUserConnMgrSelf,
                "AosUserConnMgr");


AosUserConnMgr::AosUserConnMgr()
:
mLock(OmnNew OmnMutex())
{
	OmnThreadedObjPtr thisPtr(this, false);
	mThread = OmnNew OmnThread(thisPtr, "UserConnMgrThrd", eSendThrdId, true, true, __FILE__, __LINE__);
	mThread->start();
}


AosUserConnMgr::~AosUserConnMgr()
{
}

bool
AosUserConnMgr::threadFunc(
		OmnThrdStatus::E &state,
		const OmnThreadPtr &thread)
{
	while (state == OmnThrdStatus::eActive)
	{
		mLock->lock();
		OmnUserMapItr it = mSysUsers.begin();
		u32 crttime = OmnGetSecond();
		while(it != mSysUsers.end())
		{
			AosSysUserPtr user = it->second;
			if (user->getStatus() == AosSysUser::eOffline)
			{
				it++;
				continue;
			}

			u32 lltime = user->getLastRecvTime();
			u32 delttime = crttime - lltime;
			bool wait = user->getWait();

			OmnTcpClientPtr conn = user->getConnection();
			aos_assert_r(conn, false);
			int user_sock =  conn->getSock();
			if (delttime >= eOfflineTime || user_sock<0)
			{
OmnScreen << "********user Socket*********" << user_sock << endl;
OmnScreen << "**********[" << it->first << "] DisConnect **********" << endl;
				user->setStatus(AosSysUser::eOffline);
				conn->closeConn();
				continue;
			}

			if ((delttime>=eTimeOutTime && delttime<eOfflineTime) && wait)
			{
				//send heartbeat
				OmnString callback = user->getCallBack();
				OmnString cnts;
				cnts << "<status type=\"hearbeat\"/>";
				AosMsgReqProc::sendClientResp(conn, callback, cnts);
			}
			it++;
		}
		mLock->unlock();
		OmnSleep(10);
	}
	return true;
}


bool
AosUserConnMgr::signal(const int threadLogicId)
{
	return true;
}


bool
AosUserConnMgr::checkThread(OmnString &err, const int thrdLogicId) const
{
	return true;
}


bool      	
AosUserConnMgr::start()
{
	return true;
}


bool        
AosUserConnMgr::stop()
{
	return true;
}

bool
AosUserConnMgr::config(const AosXmlTagPtr &def)
{
	return true;
}


bool
AosUserConnMgr::start(const AosXmlTagPtr &config)
{
	return true;
}


bool
AosUserConnMgr::getSysUser(
		const OmnString &cid, 
		AosSysUserPtr &user,
		bool &wait)
{
	mLock->lock();
	OmnUserMapItr it = mSysUsers.find(cid);
	if (it == mSysUsers.end())
	{
		user = OmnNew AosSysUser(cid);
		mSysUsers.insert(make_pair(cid, user));
	}
	else
	{
		user = it->second;
		aos_assert_rl(user, mLock, false);
	}
	wait = user->getWait();
	if (wait)
	{
		user->setWait(!wait);
	}
	mLock->unlock();
	return true;
}


void
AosUserConnMgr::removeSysUser(const OmnString &cid)
{
	mLock->lock();
	mSysUsers.erase(cid);
	mLock->unlock();
}
