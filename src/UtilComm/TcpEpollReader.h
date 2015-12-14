////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: TcpServerEpoll.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////
#ifndef Omn_UtilComm_TcpEpollReader_h 
#define Omn_UtilComm_TcpEpollReader_h 

#include "Porting/Socket.h"
#include "Thread/ThreadedObj.h"
#include "Thread/Ptrs.h"
#include "Thread/Mutex.h"
#include "Thread/CondVar.h"
#include "Util/ValList.h"
#include "Util/SPtr.h"
#include "Util/Array10.h"
#include "Util/IpAddr.h"
#include "UtilComm/Tcp.h"
#include "UtilComm/CommTypes.h"
#include "UtilComm/Ptrs.h"
#include "Util/RCObjImp.h"
#include <sys/epoll.h>
#include <vector>
#include <queue>
using namespace std;

class AosTcpEpollReader : public OmnThreadedObj
{
	OmnDefineRCObject;

private:
	enum
	{
		eHistoryLength = 10
	};

	OmnMutexPtr				mLock;
	OmnCondVarPtr			mCondVar;
	OmnTcpServerEpoll	   *mEpollServer;
	OmnTcpListenerPtr		mListener;
	vector<OmnThreadPtr>	mThreads;
	queue<OmnTcpClientPtr>	mConns;
	int						mNumThreads;
	u32						mMaxQueueLen;
	u64						mTotalRead;
	u64						mCrtSecRead;
	u64						mCrtSec;
	queue<u64>				mReadHistory;

public:
	AosTcpEpollReader(OmnTcpServerEpoll *server);
	~AosTcpEpollReader();
   
	virtual bool threadFunc(OmnThrdStatus::E &, const OmnThreadPtr &);
	virtual bool signal(const int threadLogicId);
    virtual bool checkThread(OmnString &errmsg, const int tid) const; 

	bool addConn(const OmnTcpClientPtr &conn)
	{
		mLock->lock();
		if (mConns.size() > mMaxQueueLen) mMaxQueueLen = mConns.size();
		mConns.push(conn);
		mCondVar->signal();
		mLock->unlock();
		return true;
	}

	void setListener(const OmnTcpListenerPtr &listener);
	void printStatus();
};
#endif

