////////////////////////////////////////////////////////////////////////////
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// Description:
//   
//
// Modification History:
// 08/02/2011: Created by Jozhi
////////////////////////////////////////////////////////////////////////////

#ifndef Aos_MsgServer_SysUser_h
#define Aos_MsgServer_SysUser_h

#include "UtilComm/TcpClient.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Thread/CondVar.h"
#include "Rundata/Rundata.h"
#include <queue>

using namespace std;

class AosSysUser : virtual public OmnRCObject
{
	OmnDefineRCObject;

public:
	enum
	{
		eOffline = 0,
		eOnline = 1,
		eDefaultWaitSec = 60 
	};

	OmnMutexPtr			mLock;
	queue<OmnString> 	mMsgs;
	int 				mWaitTimeSec;
	OmnTcpClientPtr 	mConn;
	int 				mStatus;
	u32					mLastRecvTime;
	OmnString			mCallBack;
	bool 				mWait;
	OmnString 			mCid;

public:
	AosSysUser();
	AosSysUser(const OmnString &cid);
	~AosSysUser();

	bool updateOnlineInfo(
			const OmnTcpClientPtr &conn,
			const OmnString &callback)
	{
		mLock->lock();
		mConn = conn;
		mCallBack = callback;
		mLastRecvTime = OmnGetSecond();
		if (mStatus == eOffline)
		{
OmnScreen << "user[" << mCid << "] Online" << endl;
		}
		mStatus = eOnline;
		mLock->unlock();
		return true;
	};

	bool setStatus(const int status)
	{
		mLock->lock();
		if (status == eOffline)
		{
		}
		mStatus = status;
		mLock->unlock();
		return true;
	};

	bool setConnection(const OmnTcpClientPtr &conn)
	{
		mLock->lock();
		mConn = conn;
		mLock->unlock();
		return true;
	};

	bool setWait(const bool wait)
	{
		mLock->lock();
		mWait = wait;
		mLock->unlock();
		return true;
	};
	
	bool getWait()
	{
		return mWait;
	};

	OmnTcpClientPtr getConnection()
	{
		return mConn;
	};

	u32 getLastRecvTime()
	{
		return mLastRecvTime;
	};

	int getStatus()
	{
		return mStatus;
	};

	OmnString getCallBack()
	{
		return mCallBack;
	};

	OmnString getCid()
	{
		return mCid;
	};

	bool readMsgFromDb(
			u64 &docid,
			OmnString &msg,
			const OmnString &appname,
			const AosRundataPtr &rdata);
	
	bool saveMsgToDb(
			const OmnString &recver_cid,
			const OmnString &msg,
			const OmnString &appname,
			const AosRundataPtr &rdata);

	bool readMsg(OmnString &msg);
	bool sendMsg(const OmnString &msg);
};
#endif

