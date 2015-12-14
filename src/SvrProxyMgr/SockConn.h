////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
//
// Modification History:
// Created: 04/26/2013 by Ketty 
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_SvrProxyMgr_SockConn_h
#define AOS_SvrProxyMgr_SockConn_h

#include "SvrProxyMgr/Ptrs.h"
#include "SvrProxyMgr/ConnHandler.h"
#include "SvrProxyUtil/Ptrs.h"
#include "TransBasic/ConnMsg.h"
#include "TransUtil/Ptrs.h"
#include "Thread/ThreadedObj.h"
#include "Thread/Ptrs.h"
#include "Util/Ptrs.h"

#include <queue>
#include <list>
#include <map>
#include <vector>
#include <sys/epoll.h>
#include <sys/uio.h>
using namespace std;

#define HEARTBEAT         "<headrbeat />"

class AosSockConn: public OmnThreadedObj
{
	OmnDefineRCObject;
	
	enum
	{
		eEpollWaitThrdId = 1,
		eEpollSendThrdId = 2,
		
		eMinCacheSize = 1000000, 
		//eMinWaitAckNum = 3, 
		eMaxWaitAckNum = 20, 
		
		eSendIovArraySize = 10,		// eSendIovArraySize % 2 must be 0.
		eRecvBuffSize = 1024 * 4,
	};
	
private:
	int					mStaleSock;
	int					mSock;
	OmnMutexPtr			mLock;
	OmnMutexPtr			mSendLock;
	OmnMutexPtr			mRecvLock;
	OmnCondVarPtr		mSendCondVar;
	OmnCondVarPtr		mRecvCondVar;
	u32					mMaxCacheSize;
	
	AosRecvEventHandlerPtr mRecvHandler;
	AosSvrInfoPtr		mSvrInfo;

	OmnThreadPtr		mThread;	
	int					mEpollFd;
	OmnCondVarPtr		mEpollRecvVar;
	
	OmnThreadPtr		mSendThrd;
	int					mEpollSendFd;
	OmnCondVarPtr		mEpollSendVar;

	vector<AosBuffPtr>	mSendBuffs;
	u8					mSendMsgLenPos;

	u32 				mCrtSendRlbSeq;
	list<AosConnMsgPtr> mSendQueue;
	int					mSendQueueSize;
	u32					mSendQueueNum;
	list<AosAppMsgPtr>  mWaitAckCache;
	int					mWaitAckNum;

	AosBuffPtr			mRecvBuff;	
	AosBuffPtr			mUnfinishRecvData;
	u32					mRecvMsgLen;
	u8					mRecvMsgLenPos;

	u32 				mCrtRecvRlbSeq;
	queue<AosAppMsgPtr>	mRecvQueue;
	int					mRecvQueueSize;
	map<u32, AosAppMsgPtr> mRecvCache;
	
	bool				mShowLog;
	bool 				mThreadStatus1;
	bool 				mThreadStatus2;
	
	u64					mRecvHeartbeatTime;
	OmnString 			mName;

	// for test by andy                      
	char*               mTestData; 
	i64                 mTestDataLen;

public:
	AosSockConn(const u32 max_cache_size,
		const AosRecvEventHandlerPtr &event_hd,
		const AosSvrInfoPtr &svr_info,
		const bool show_log);
	~AosSockConn();

	// ThreadedObj Interface
	virtual bool    threadFunc(OmnThrdStatus::E &state, const OmnThreadPtr &thread);
//	virtual bool    checkThread(OmnString &err, const int thrdLogicId) const{ return true;};
	virtual bool    checkThread111(OmnString &err, const int thrdLogicId);
	virtual bool    signal(const int threadLogicId);

	bool	signalRecvCond();

	bool	start();
	bool	stop();
	bool 	swap(const int new_sock);
	int		getSock(){ return mSock;};
	bool	isGood(){ return mSock != -1; };
	void 	resetConnSock();
	void 	setName(const OmnString &name) {mName = name;}

	bool	smartSend(const AosAppMsgPtr &msg, bool &is_ignore, bool &bad_conn);
	AosAppMsgPtr smartReadSync();
	AosAppMsgPtr smartReadAsync();

private:
	bool	startThrd();
	//bool 	closeConn(const int crt_sock);
	bool 	closeConnPriv(const int crt_sock);
	bool 	setNonBlocking();
	bool 	epollWaitThrdFunc(OmnThrdStatus::E &state, const OmnThreadPtr &thread);
	bool 	checkWaitAckCacheLocked();
	bool 	sendConnMsg(const AosConnMsgPtr &msg);
	bool 	sendConnMsgLocked(const AosConnMsgPtr &msg);

	bool 	epollSendThrdFunc(OmnThrdStatus::E &state, const OmnThreadPtr &thread);

	bool 	recvMsg(const int crt_sock);
	u32		readFromSock(const int crt_sock, char * data, u32 read_size);
	bool 	recvBigMsg(const int crt_sock, AosConnMsgPtr &msg);
	bool 	procRecvBuff(vector<AosConnMsgPtr> &recved_msg);
	bool 	recvUnfinishMsgLen(u32 &msg_len);
	bool 	recvUnfinishMsg(AosConnMsgPtr &msg);
	bool 	getMsgLenFromBuff(u32 &msg_len);
	bool 	getMsgFromBuff(const u32 msg_len, AosConnMsgPtr &msg);
	
	bool 	procRecvedMsg(vector<AosConnMsgPtr> &recved_msg);
	bool 	appMsgRecved(map<u32, AosAppMsgPtr> &app_msgs);
	void 	seqGetAppMsg(map<u32, AosAppMsgPtr> &msg_map);
	void 	pushToRecvQueue(const AosAppMsgPtr &msg);
	
	bool 	sendAck(const u32 seqno);
	bool 	ackRecved(const u32 seqno);
	bool 	sendWaitSeq();
	bool 	waitSeqRecved(const u32 seqno);
	
	bool 	sendMsg(int crt_sock);
	bool 	collectSendBuffs(iovec ** send_iov_ptr, u32 &iov_num, u32 &total_size);
	bool 	moveSendBuffsPos(int bytes_send);
	bool 	moveUnfinishBuffPos(int &bytes_send, const AosBuffPtr &buff);
	AosConnMsgPtr getSendMsgLocked();
	int 	sendToSock(const int crt_sock, iovec * iov, int iov_num);
	void 	cleanQueue();

	bool 	updateHeartbeat();

	//Jozhi Testing=================
	bool	appendTestBuffForRecv(const char* data, const int len);
	bool	appendTestBuff(const char* data, const int len);
	bool	appendTestIOvBuff(const char* data, const int len);
	
};
#endif
