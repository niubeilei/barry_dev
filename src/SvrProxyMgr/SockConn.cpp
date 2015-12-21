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
// Created: 04/26/2013 by Ketty 
////////////////////////////////////////////////////////////////////////////
#include "SvrProxyMgr/SockConn.h"

#include "alarm_c/alarm.h" 
#include "Porting/Sleep.h"
#include "Porting/Socket.h"
#include "Thread/CondVar.h" 
#include "Thread/Mutex.h" 
#include "Thread/Thread.h" 
#include "TransBasic/Trans.h"
#include "Rundata/Rundata.h"
#include "TransUtil/Ptrs.h"
#include "SvrProxyMgr/SvrInfo.h"
#include "SvrProxyMgr/SvrProxy.h"
#include "SvrProxyMgr/ConnHandler.h"
#include "TransBasic/ConnMsg.h"
#include "SvrProxyUtil/ConnMsgAck.h"
#include "SvrProxyUtil/ConnMsgWaitSeq.h"
#include "SvrProxyUtil/ConnMsgGetWaitSeq.h"
#include "TransBasic/AppMsg.h"
#include "Util/Buff.h"

#include <unistd.h>
#include <fcntl.h>
#include <error.h>

// "aaaaaaaa:"	is for tester send.
// "bbbbbbbb:"  is for tester recv.
//static bool mTesterShow = false;

int sgMaxEpollEvent = 5;

AosSockConn::AosSockConn(
		const u32 max_cache_size,
		const AosRecvEventHandlerPtr &recv_hd,
		const AosSvrInfoPtr &svr_info,
		const bool show_log)
:
mSock(-1),
mLock(OmnNew OmnMutex()),
mSendLock(OmnNew OmnMutex()),
mRecvLock(OmnNew OmnMutex()),
mRecvCondVar(OmnNew OmnCondVar()),
mMaxCacheSize(max_cache_size),
mRecvHandler(recv_hd),
mSvrInfo(svr_info),
mEpollRecvVar(OmnNew OmnCondVar()),
mEpollSendVar(OmnNew OmnCondVar()),
mSendMsgLenPos(0),
mCrtSendRlbSeq(1),
mSendQueueSize(0),
mSendQueueNum(0),
mWaitAckNum(0),
mRecvMsgLen(0),
mRecvMsgLenPos(0),
mCrtRecvRlbSeq(0),
mRecvQueueSize(0),
mShowLog(false),
mThreadStatus1(false),
mThreadStatus2(false),
mRecvHeartbeatTime(0),
mTestData(0),    
mTestDataLen(0)  
{

	mRecvBuff = OmnNew AosBuff(eRecvBuffSize, 0 AosMemoryCheckerArgs);
	mEpollFd = epoll_create(sgMaxEpollEvent);
	mEpollSendFd = epoll_create(sgMaxEpollEvent);

	/*
	setNonBlocking();
	struct epoll_event recv_event;
	recv_event.data.fd = mSock;
	recv_event.events = EPOLLIN | EPOLLET;
	epoll_ctl(mEpollFd, EPOLL_CTL_ADD, mSock, &recv_event);

	struct epoll_event 	send_event;
	send_event.data.fd = mSock;
	send_event.events = EPOLLOUT | EPOLLET;
	epoll_ctl(mEpollSendFd, EPOLL_CTL_ADD, mSock, &send_event);
	*/

	OmnThreadedObjPtr thisPtr(this, false);
	mThread = OmnNew OmnThread(thisPtr, "SockConnWaitThrd",
			eEpollWaitThrdId, true, true, __FILE__, __LINE__);
	mSendThrd = OmnNew OmnThread(thisPtr, "SockConnSendThrd",
			eEpollSendThrdId, true, true, __FILE__, __LINE__);
}


AosSockConn::~AosSockConn()
{
	stop();
}


bool
AosSockConn::start()
{
	return startThrd();
}


bool
AosSockConn::stop()
{
	if(mSock == -1)	return true;
	
	closeConnPriv(mSock);
	
	cleanQueue();

	mLock->lock();
	mSock = -1;
	mThread->stop();
	mSendThrd->stop();
	mLock->unlock();
	return true;
}

bool
AosSockConn::startThrd()
{
	if(mThread->getStatus() != OmnThrdStatus::eActive)
	{
		mThread->start();
	}
	if(mSendThrd->getStatus() != OmnThrdStatus::eActive)
	{
		mSendThrd->start();
	}
	return true;
}

bool
AosSockConn::closeConnPriv(const int crt_sock)
{
	aos_assert_r(crt_sock != -1, false);

	OmnScreen << "SockConn:: sock:" << crt_sock
		<< "; Close conn;"
		<< "; errno:" << errno
		<< "; " << strerror(errno)
		<< endl;

	struct epoll_event ev;
	//ev.data.fd = mSock;
	ev.data.fd = crt_sock;
	ev.events = EPOLLIN | EPOLLET;
	//epoll_ctl(mEpollFd, EPOLL_CTL_DEL, mSock, &ev);
	epoll_ctl(mEpollFd, EPOLL_CTL_DEL, crt_sock, &ev);
		
	ev.events = EPOLLOUT | EPOLLET;
	//epoll_ctl(mEpollSendFd, EPOLL_CTL_DEL, mSock, &ev);
	epoll_ctl(mEpollSendFd, EPOLL_CTL_DEL, crt_sock, &ev);

	::close(crt_sock);
	return true;
}

void
AosSockConn::resetConnSock()
{
	mSock = -1;
	mThread->stop();
	mSendThrd->stop();
}

bool
AosSockConn::setNonBlocking()
{
	int opts = fcntl(mSock,F_GETFL);
	if(opts<0)
	{
		OmnAlarm << "error!" << enderr;
		return false;
	}
	opts = opts|O_NONBLOCK;
	if(fcntl(mSock,F_SETFL,opts)<0)
	{
		OmnAlarm << "error!" << enderr;
		return false;
	}
	return true;
}


bool
AosSockConn::swap(const int new_sock)
{
	//aos_assert_r(new_sock != 0 && mSock == -1, false);
	aos_assert_r(new_sock != 0, false);
	
	cleanQueue();
	
	mLock->lock();
	if(mSock != -1)
	{
		//::close(mSock);
		OmnScreen << "SockConn; swap. But the conn is good:"
			<< "; old_conn:" << mSock 
			<< "; new_conn:" <<new_sock 
			<< endl;
		closeConnPriv(mSock);
	}
	
	mSock = new_sock;
	setNonBlocking();

	struct epoll_event recv_event;
	recv_event.data.fd = mSock;
	recv_event.events = EPOLLIN | EPOLLET;
	epoll_ctl(mEpollFd, EPOLL_CTL_ADD, mSock, &recv_event);
	
	struct epoll_event send_event;
	send_event.data.fd = mSock;
	send_event.events = EPOLLOUT | EPOLLET;
	epoll_ctl(mEpollSendFd, EPOLL_CTL_ADD, mSock, &send_event);

	startThrd();
	mLock->unlock();
	return true;
}

//felicia, 2013/06/19
bool
AosSockConn::signalRecvCond()
{
	//mLock->lock();
	//mRecvCondVar->signal();
	//mLock->unlock();
	return true;	
}

bool    
AosSockConn::signal(const int threadLogicId)
{
	/*
	if (threadLogicId == eEpollWaitThrdId)
	{
		//mLock->lock();
		mEpollRecvVar->signal();
		//mLock->unlock();
	}

	if (threadLogicId == eEpollSendThrdId)
	{
		//mLock->lock();
		mEpollSendVar->signal();
		//mLock->unlock();
	}
	*/
	return true;
}

bool    
AosSockConn::checkThread111(OmnString &err, const int thrdLogicId)
{
	bool threadStatus;
	if (thrdLogicId == eEpollWaitThrdId)
	{
		threadStatus = mThreadStatus1;
		mThreadStatus1 = false;
	}
	else if (thrdLogicId == eEpollSendThrdId)
	{
		threadStatus = mThreadStatus2;
		mThreadStatus2 = false;
	}
	else
	{
		OmnAlarm << "Invalid thread id !" << enderr;
	}
	return threadStatus;
}

bool
AosSockConn::threadFunc(
		OmnThrdStatus::E &state,
		const OmnThreadPtr &thread)
{
	u64 thrd_id = thread->getLogicId();

	if(thrd_id == eEpollWaitThrdId)
	{
		return epollWaitThrdFunc(state, thread);
	}

	if(thrd_id == eEpollSendThrdId)
	{
		return epollSendThrdFunc(state, thread);
	}

	OmnAlarm << "Invalid thread logic id: " << thrd_id << enderr;
	return false;
}


bool
AosSockConn::epollWaitThrdFunc(
		OmnThrdStatus::E &state,
		const OmnThreadPtr &thread)
{
	struct epoll_event events[sgMaxEpollEvent];

	while(state == OmnThrdStatus::eActive)
	{
		if(mSock <=0)	return true;
		
		int nfds = epoll_wait(mEpollFd, events, sgMaxEpollEvent, 1000);
		mThreadStatus1 = true;
			
		if(nfds == -1)
		{
			if(errno != EINTR && errno != EAGAIN)
			{
				int err = errno;
				OmnAlarm << "epoll_wait error!"
					<< "; errno:" << err << enderr;
			}
			continue;
		}
		
		if(nfds == 0)
		{
			continue;
		}

	
		aos_assert_r(nfds == 1, false);
		int crt_sock = events[0].data.fd;
		if(crt_sock != mSock)
		{
			continue;
		}

		if(events[0].events & EPOLLIN)
		{
			if(mSvrInfo->isNetSvr()) updateHeartbeat();
			mRecvLock->lock();
			bool rslt = recvMsg(crt_sock);
			mRecvLock->unlock();
			if (!rslt)
			{
				if (mSvrInfo->isNetSvr())
				{
					OmnAlarm << "=========debug==========may be recv msg failed, if this is NetSvr, try to reconnect the server: " << enderr;
					mSvrInfo->repaireConnect();
				}
			}
		}
	}
	return true;
}


bool
AosSockConn::updateHeartbeat()
{
	u64 crt_time = OmnGetTimestamp();
	if (mRecvHeartbeatTime == 0)
	{
		mRecvHeartbeatTime = crt_time;
		return true;
	}
	u64 delay = crt_time - mRecvHeartbeatTime;
	if(delay < 1 * 1000 * 1000) return true; 

	mRecvHeartbeatTime = crt_time;
	mSvrInfo->updateSockLastRecvTime();
	return true;
}


bool
AosSockConn::epollSendThrdFunc(
		OmnThrdStatus::E &state,
		const OmnThreadPtr &thread)
{
	struct epoll_event events[sgMaxEpollEvent];

	while(state == OmnThrdStatus::eActive)
	{
		if(mSock <=0)	return true;
	
		//u64 t1 = OmnGetTimestamp();
		int nfds = epoll_wait(mEpollSendFd, events, sgMaxEpollEvent, 1000);
		//u64 t2 = OmnGetTimestamp();
		mThreadStatus2 = true;

		if(nfds == -1)
		{
			OmnScreen << "epoll_wait error!"
					<< "; errno:" << errno
					<< "; " << strerror(errno) << endl;
			if(errno != EINTR && errno != EAGAIN)
			{
				int err = errno;
				OmnAlarm << "epoll_wait error!"
					<< "; errno:" << err
					<< "; " << strerror(errno) << enderr;
			}
			continue;
		}
		
		if(nfds == 0)
		{
			// means timeout.
		}

		bool rslt = false;
		for(int i=0;i<nfds;++i)
		{
			// this mSock may be changed by swap.
			int crt_sock = events[i].data.fd;
			//aos_assert_r(crt_sock == mSock, false);
			if(crt_sock != mSock)
			{
				// this mSock maybe changed by swap.
				continue;
			}

				
			if(events[i].events & EPOLLOUT)
			{
				rslt = sendMsg(crt_sock);
				aos_assert_r(rslt, false);
			}
		}
	}
	return true;
}


bool
AosSockConn::recvBigMsg(const int crt_sock, AosConnMsgPtr &msg)
{
	msg = 0;
	aos_assert_r(mUnfinishRecvData && mRecvBuff, false);

	u32 crt_d_len = mUnfinishRecvData->dataLen();
	u32 remain_size = mUnfinishRecvData->buffLen() - crt_d_len;
	aos_assert_r(remain_size > mRecvBuff->buffLen(), false);

	char *data = mUnfinishRecvData->data() + crt_d_len;
	u32 bytes_recv = readFromSock(crt_sock, data, remain_size);
	mUnfinishRecvData->setDataLen(crt_d_len + bytes_recv);
	if(bytes_recv < remain_size)	return true;


	// msg data recved success.
	msg = AosConnMsg::serializeFromStatic(mUnfinishRecvData, true);
	aos_assert_r(msg, false);
	
	//if(mTesterShow)
	if(mShowLog)
	{
		OmnScreen << "SockConn:: sock:" << mSock 
			<< "; name:" << mName
			<< "; bbbbbbbb: recv big msg: recv app msg succ," 
			<< "; opr:" << msg->getStrType()
			<< "; need_buff_len:" << mUnfinishRecvData->buffLen() 
			<< endl;
	}
	mUnfinishRecvData = 0;
	return true;
}


bool
AosSockConn::recvMsg(const int crt_sock)
{
	bool rslt = false;
	vector<AosConnMsgPtr> recved_msg;
	AosConnMsgPtr msg;
	while(1)
	{
		recved_msg.clear();
		if(mUnfinishRecvData)
		{
			u32 crt_d_len = mUnfinishRecvData->dataLen();
			u32 remain_size = mUnfinishRecvData->buffLen() - crt_d_len;
			if(remain_size > mRecvBuff->buffLen())
			{
				rslt = recvBigMsg(crt_sock, msg);
				aos_assert_r(rslt, false);
				if(!msg)
				{
					// means can't recv anymore.
					break;
				}
					
				recved_msg.push_back(msg);
				rslt = procRecvedMsg(recved_msg);
				aos_assert_r(rslt, false);
				continue;
			}
		}
		
		u32 recv_size = mRecvBuff->buffLen();
		memset(mRecvBuff->data(), 0, mRecvBuff->buffLen());
		u32 bytes_recv = readFromSock(crt_sock, mRecvBuff->data(), recv_size);
		mRecvBuff->setDataLen(bytes_recv);
		//if(mTesterShow)
		if(mShowLog)
		{
			OmnScreen << "SockConn:: sock:" << mSock 
				<< "; name:" << mName
				<< "; bbbbbbbb: recv Buff:" << bytes_recv << endl;
		}

		rslt = procRecvBuff(recved_msg);
		if (!rslt)
		{
			OmnAlarm << "procRecvBuff error" << enderr;
		}
		rslt = procRecvedMsg(recved_msg);
		aos_assert_r(rslt, false);

		if(bytes_recv < recv_size)
		{
			// means can't recv anymore.
			break;
		}
	}

	return true;
}


u32	
AosSockConn::readFromSock(const int crt_sock, char * data, u32 read_size)
{
	int bytesrecv  = ::read(crt_sock, data, read_size);
	if(bytesrecv > 0)	
	{
		appendTestBuffForRecv(data, bytesrecv);
		return bytesrecv;
	}
		
	// Ketty 2013/07/31
	if(errno == EAGAIN)	return 0;
		
	OmnScreen << "read error! errno:" << errno
		<< "; " << strerror(errno) << endl;
	//if(errno == EINTR) return 0;	 // Interrupted system call.
	
	//mRecvLock->unlock();		// ketty tmp.
	//closeConn(crt_sock);
	//mRecvLock->lock();
	return 0;
}


bool
AosSockConn::procRecvBuff(vector<AosConnMsgPtr> &recved_msg)
{
	//static u32 recv_num = 1;
	aos_assert_r(mRecvBuff, false);
	u32 data_len = mRecvBuff->dataLen();
	if(data_len == 0)	return true;
	
	bool rslt;
	mRecvBuff->setCrtIdx(0);
	
	u32 msg_len = 0;
	AosBuffPtr msg_buff;
	AosConnMsgPtr msg;
	if(mRecvMsgLenPos != 0)
	{
		rslt = recvUnfinishMsgLen(msg_len);
		aos_assert_r(rslt, false);
		if(!msg_len)	return true;

		rslt = getMsgFromBuff(msg_len, msg);
		aos_assert_r(rslt, false);		
		if(!msg)	return true;
		
		recved_msg.push_back(msg);
	
		//if(mTesterShow)
		if(mShowLog)
		{
			OmnScreen << "SockConn:: sock:" << mSock 
				<< "; name:" << mName
				<< "; bbbbbbbb: unfinish len: recv app msg succ," 
				<< "; opr:" << msg->getStrType()
				<< "; buff_len:" << msg->getConnBuff()->dataLen()
				<< endl;
		}
	}
	
	if(mUnfinishRecvData)
	{
		rslt = recvUnfinishMsg(msg);
		aos_assert_r(rslt, false);
		if(!msg)	return true;

		recved_msg.push_back(msg);
		
		//if(mTesterShow)
		if(mShowLog)
		{
			OmnScreen << "SockConn:: sock:" << mSock 
				<< "; name:" << mName
				<< "; bbbbbbbb: unfinish msg: recv app msg succ," 
				<< "; opr:" << msg->getStrType()
				<< "; buff_len:" << msg->getConnBuff()->dataLen()
				<< endl;
		}
	}

	while(mRecvBuff->getCrtIdx() < data_len)
	{
		rslt = getMsgLenFromBuff(msg_len);
		aos_assert_r(rslt, false);
		if(!msg_len)	return true;

		rslt = getMsgFromBuff(msg_len, msg); 
		aos_assert_r(rslt, false);
		if(!msg)	return true;

		recved_msg.push_back(msg);
		
		//if(mTesterShow)
		if(mShowLog)
		{
			OmnScreen << "SockConn:: sock:" << mSock 
				<< "; name:" << mName
				<< "; bbbbbbbb: recv app msg succ," 
				<< "; opr:" << msg->getStrType()
				<< "; buff_len:" << msg->getConnBuff()->dataLen()
				<< endl;
		}
	}
	return true;
}


bool
AosSockConn::recvUnfinishMsgLen(u32 &msg_len)
{
	msg_len = 0;
	aos_assert_r(mRecvBuff, false);
	if(mRecvMsgLenPos == 0)	return true;
	
	int remain_size = sizeof(mRecvMsgLen) - mRecvMsgLenPos;
	aos_assert_r(remain_size >0, false);

	u32 data_len = mRecvBuff->dataLen();
	u32 cpy_len = data_len < (u32)remain_size ? data_len : remain_size;

	char *data = (char *)&mRecvMsgLen + mRecvMsgLenPos;
	memcpy(data, mRecvBuff->data(), cpy_len);
	if(data_len < (u32)remain_size)
	{
		// means need continue read len.
		mRecvMsgLenPos += cpy_len;
		OmnAlarm << "Maybe some error!" << enderr;
		return true;
	}

	// len recved success.
	msg_len = mRecvMsgLen;
	
	mRecvMsgLen = 0;
	mRecvMsgLenPos = 0;
	mRecvBuff->setCrtIdx(cpy_len);
		
	//if(mTesterShow)
	if(mShowLog)
	{
		OmnScreen << "SockConn:: sock:" << mSock 
			<< "; name:" << mName
			<< "; bbbbbbbb: recv UnfinishMsgLen: " << mRecvBuff->getCrtIdx() << endl; 
	}
	return true;
}


bool
AosSockConn::recvUnfinishMsg(AosConnMsgPtr &msg)
{
	msg = 0;
	aos_assert_r(mRecvBuff && mUnfinishRecvData, false);
	
	u32 udn = mUnfinishRecvData->dataLen();
	int remain_size = mUnfinishRecvData->buffLen() - udn;
	aos_assert_r(remain_size >0, false);

	u32 data_len = mRecvBuff->dataLen();
	u32 cpy_len = data_len < (u32)remain_size ? data_len : remain_size;

	char *data = mUnfinishRecvData->data() + udn;
	memcpy(data, mRecvBuff->data(), cpy_len);
	mUnfinishRecvData->setDataLen(udn + cpy_len);

	if(data_len < (u32)remain_size)
	{
		// means need continue read data.
		//OmnAlarm << "Maybe some error!" << enderr;
		//OmnScreen << "Maybe some error!" << endl;
		return true;
	}
	
	// msg data recved success.
	msg = AosConnMsg::serializeFromStatic(mUnfinishRecvData, true);
	aos_assert_r(msg, false);

	mUnfinishRecvData = 0;
	mRecvBuff->setCrtIdx(cpy_len);

	//if(mTesterShow)
	if(mShowLog)
	{
		OmnScreen << "SockConn:: sock:" << mSock 
			<< "; name:" << mName
			<< "; bbbbbbbb: recv UnfinishMsg: " << mRecvBuff->getCrtIdx() << endl; 
	}
	return true;
}


bool
AosSockConn::getMsgLenFromBuff(u32 &msg_len)
{
	msg_len = 0;
	aos_assert_r(mRecvBuff, false);
	
	u32 data_len = mRecvBuff->dataLen();
	u32 crt_idx = mRecvBuff->getCrtIdx();
	if(data_len - crt_idx < 4)
	{
		// means msg_len not recv finished.
		u32 cpy_len = data_len - crt_idx; 
		char *data = (char *)&mRecvMsgLen;
		memcpy(data, mRecvBuff->data() + crt_idx, cpy_len);
		mRecvMsgLenPos = cpy_len;
		return true;
	}
	
	msg_len = mRecvBuff->getU32(0);
	//if(mTesterShow)
	if(mShowLog)
	{
		OmnScreen << "SockConn:: sock:" << mSock 
			<< "; name:" << mName
			<< "; bbbbbbbb: getMsgLen:" << msg_len << endl;
	}
	aos_assert_r(msg_len != 0, false);
	return true;
}


bool
AosSockConn::getMsgFromBuff(const u32 msg_len, AosConnMsgPtr &msg)
{
	msg = 0;
	aos_assert_r(mRecvBuff && msg_len, false);

	u32 data_len = mRecvBuff->dataLen();
	if(data_len - mRecvBuff->getCrtIdx() < msg_len)
	{
		// means msg_buff not recv finished.
		mUnfinishRecvData = OmnNew AosBuff(msg_len, 0 AosMemoryCheckerArgs);
		memset(mUnfinishRecvData->data(), 0, msg_len);

		u32 crt_idx = mRecvBuff->getCrtIdx();
		u32 cpy_len = data_len - crt_idx; 
		memcpy(mUnfinishRecvData->data(), mRecvBuff->data() + crt_idx, cpy_len);
		mUnfinishRecvData->setDataLen(cpy_len);
		return true;
	}
		
	// recv this msg finish.
	// Ketty temp...
	//AosBuffPtr msg_buff = mRecvBuff->getBuff(msg_len, false AosMemoryCheckerArgs);
	AosBuffPtr msg_buff = mRecvBuff->getBuff(msg_len, true AosMemoryCheckerArgs);
	msg = AosConnMsg::serializeFromStatic(msg_buff, true);
	aos_assert_r(msg, false);

	return true;
}


bool
AosSockConn::procRecvedMsg(vector<AosConnMsgPtr> &recved_msg)
{
	bool rslt = false;
	bool get_wait_seq = false;
	u32 ack_seq = 0;
	u32 svr_wait_seq = 0;

	map<u32, AosAppMsgPtr> app_msgs;
	for(u32 i=0; i<recved_msg.size(); i++)
	{
		AosConnMsgPtr msg = recved_msg[i];
		if(msg->isAppMsg())
		{
			AosAppMsgPtr a_msg = (AosAppMsg *)msg.getPtr();		
			u32 msg_seq = a_msg->getRlbSeq();
			app_msgs.insert(make_pair(msg_seq, a_msg));
			
			if(mShowLog)
			{
				OmnScreen << "SockConn:: sock:" << mSock << "; svr" 
					<< "; name:" << mName
					<< "; recv Msg:" << msg_seq
					<< "; type:" << a_msg->getStrType()
					<< "; mCrtRecvRlbSeq: " << mCrtRecvRlbSeq 
					<< endl;
			}
			continue;
		}

		MsgType::E type = msg->getType();
		if(type == MsgType::eConnAck)
		{
			AosConnMsgAckPtr ack_msg = (AosConnMsgAck *)msg.getPtr();
			u32 seq = ack_msg->getAckSeq();
			if(seq > ack_seq)	ack_seq = seq;
			continue;
		}

		if(type == MsgType::eGetWaitSeq)
		{
			get_wait_seq = true;
			continue;
		}
		
		if(type == MsgType::eWaitSeq)
		{
			AosConnMsgWaitSeqPtr w_msg = (AosConnMsgWaitSeq *)msg.getPtr();
			u32 seq = w_msg->getWaitSeq();
			if(seq > svr_wait_seq)	svr_wait_seq = seq;
			continue;
		}
		
		//if(memcmp(buff->data(), HEARTBEAT, buff->dataLen()) == 0)
		OmnAlarm << "Invalid msgType:" << type << enderr;
	}

	if(get_wait_seq)
	{
		if(mShowLog)
		{
			OmnScreen << "SockConn:: sock:" << mSock << "; svr" 
				<< "; name:" << mName
				<< "; getWaitSeq"
				<< endl;
		}
		rslt = sendWaitSeq();
		aos_assert_r(rslt, false);
	}
	if(ack_seq)
	{
		rslt = ackRecved(ack_seq);
		aos_assert_r(rslt, false);
	}
	if(svr_wait_seq && svr_wait_seq > ack_seq)
	{
		rslt = waitSeqRecved(svr_wait_seq);
		if (!rslt)
		{
			OmnAlarm << "waitSeqRecved error" << enderr;
		}

	}

	rslt = appMsgRecved(app_msgs);
	aos_assert_r(rslt, false);
	return true;
}


bool
AosSockConn::appMsgRecved(map<u32, AosAppMsgPtr> &app_msgs)
{
	if(app_msgs.size() == 0)	return true;
	
	bool rslt = false;
	u32 old_recv_seq = mCrtRecvRlbSeq;
	mLock->lock();
	if(mRecvCache.size())
	{
		mRecvCache.insert(app_msgs.begin(), app_msgs.end());
		seqGetAppMsg(mRecvCache);
	}
	else
	{
		seqGetAppMsg(app_msgs);
		if(app_msgs.size() != 0)
		{
			mRecvCache.insert(app_msgs.begin(), app_msgs.end());
		}
	}

	mRecvCondVar->signal();
	mLock->unlock();
	
	if(mCrtRecvRlbSeq != old_recv_seq)
	{
		// send Ack.
		rslt = sendAck(mCrtRecvRlbSeq);	
		aos_assert_r(rslt, false);
	}

	if(mRecvCache.size() >= eMaxWaitAckNum)
	{
		if(mShowLog)
		{
			OmnScreen << "SockConn:: sock:" << mSock << "; svr" 
				<< "; name:" << mName
				<< "; too many recvCacheSize: " << mRecvCache.size()
				<< "; sendWaitSeq;"
				<< endl;
		}
		sendWaitSeq();
	}

	if(mRecvHandler)
	{
		AosSockConnPtr thisptr(this, false);
		mRecvHandler->msgRecvEvent(thisptr);
	}

	mLock->lock();
	while((u32)mRecvQueueSize > mMaxCacheSize)
	{
		bool timeout = true;
		mEpollRecvVar->timedWait(mLock, timeout, 5);
		mThreadStatus1 = true;
	}
	mLock->unlock();
	
	return true;
}


void
AosSockConn::seqGetAppMsg(map<u32, AosAppMsgPtr> &msg_map)
{
	map<u32, AosAppMsgPtr>::iterator itr = msg_map.begin();
	for(; itr != msg_map.end(); itr++)
	{
		u32 msg_seq = (itr->second)->getRlbSeq();
		if(msg_seq < mCrtRecvRlbSeq + 1)	continue;
		if(msg_seq > mCrtRecvRlbSeq + 1)	break;
			
		mCrtRecvRlbSeq = msg_seq;
		pushToRecvQueue(itr->second);
	}

	msg_map.erase(msg_map.begin(), itr);
}


void
AosSockConn::pushToRecvQueue(const AosAppMsgPtr &msg)
{
	//return;
	mRecvQueue.push(msg);
	mRecvQueueSize += msg->getSize();
	if(mShowLog)
	{
		OmnScreen << "SockConn:: sock:" << mSock << "; svr" 
			<< "; name:" << mName
			<< "; recv Msg. push to Recv Queue:"
			<< "; type:" << msg->getStrType()
			//<< msg->getRlbSeq() 
			<< endl;
		
		if(msg->isTrans())
		{
			AosTransPtr trans = (AosTrans *)msg.getPtr();
			OmnScreen << "Trans Perf" 
				<< "; name:" << mName
				<< "; trans_id:" << trans->getTransId().toString()
				<< "; time:" << OmnGetTimestamp()
				<< "; SockConn recv trans. push to RecvQueue."
				<< endl;
		}
	}
}


bool
AosSockConn::sendAck(const u32 seqno)
{
	aos_assert_r(seqno > 0, false);
	if(mShowLog)
	{
		OmnScreen << "SockConn:: sock:" << mSock << "; svr" 
			<< "; name:" << mName
			<< "; send ack:" << seqno
			<< endl;
	}

	AosConnMsgAckPtr msg = OmnNew AosConnMsgAck(seqno);
	return sendConnMsg(msg);
}


bool
AosSockConn::ackRecved(const u32 seqno)
{
	aos_assert_r(seqno > 0, false);
	if(mShowLog)
	{
		OmnScreen << "SockConn:: sock:" << mSock << "; clt" 
			<< "; name:" << mName
			<< "; recv Ack:" << seqno
			<< endl;
	}

	mLock->lock();
	AosAppMsgPtr msg;
	u32 m_seq = 0;

	u32 msgs_size = 0;
	u32 num = 0;
	list<AosAppMsgPtr>::iterator itr = mWaitAckCache.begin();
	for(; itr != mWaitAckCache.end(); itr++) 
	{
		msg = *itr;
		msgs_size += msg->getSize();
		num++;
		
		m_seq = msg->getRlbSeq();
		if(m_seq < seqno)	continue;

		if(m_seq == seqno)
		{
			itr++;
			break;
		}
		// maybe this ack has recved.
		//OmnAlarm << "can't find ack_seq:" << seqno << enderr;
		msgs_size -= msg->getSize();
		num--;
		break;
	}

	mWaitAckCache.erase(mWaitAckCache.begin(), itr);
	mSendQueueSize -= msgs_size;
	mWaitAckNum -= num;
	aos_assert_rl(mWaitAckNum >= 0, mLock, false);

	mLock->unlock();
	if(mSendQueueSize <= eMinCacheSize && mSvrInfo)
	{
		// push the mSvrInfo send the ignore trans.
		mSvrInfo->sendIgnoreTransPublic();
	}

	return true;
}


bool
AosSockConn::sendWaitSeq()
{
	u32 wait_seq = mCrtRecvRlbSeq + 1;
	
	if(mShowLog)
	{
		OmnScreen << "SockConn:: sock:" << mSock << "; svr" 
			<< "; name:" << mName
			<< "; sendWaitSeq." << wait_seq 
			<< endl; 
	}
	AosConnMsgWaitSeqPtr msg = OmnNew AosConnMsgWaitSeq(wait_seq);
	return sendConnMsg(msg);
}


bool
AosSockConn::waitSeqRecved(const u32 seqno)
{
	aos_assert_r(seqno > 0, false);

	mLock->lock();
	//if(!mWaitAckCache.size())
	if(!mWaitAckNum)
	{
		mLock->unlock();
		return true;
	}

	list<AosAppMsgPtr>::iterator itr = mWaitAckCache.begin();
	AosAppMsgPtr msg;
	u32 m_seq = 0;

	u32 num = 0;
	u32 max_seq = 0;
	while(itr != mWaitAckCache.end()) 
	{
		num++;
		msg = *itr++;
		m_seq = msg->getRlbSeq(); 
		if(m_seq == seqno)	break;
		if(max_seq < m_seq) max_seq = m_seq;
	}
	if(itr == mWaitAckCache.end() && seqno < max_seq)
	{
		// if max_seq < seqno maybe this seqno not send yet.
		OmnAlarm << "can't find msg seq:" << seqno << enderr;
		mLock->unlock();
		return false;
	}

	mWaitAckCache.erase(mWaitAckCache.begin(), itr);
	mWaitAckNum -= (num - 1);
	aos_assert_rl(mWaitAckNum >= 0, mLock, false);
	mLock->unlock();
	
	sendConnMsg(msg.getPtr());
	return true;	
}


bool
AosSockConn::sendMsg(int crt_sock)
{
	iovec * send_iov = 0;
	u32 iov_num = 0;
	u32 total_size = 0;
	bool rslt;
	
	while(1)
	{
		mSendLock->lock();
		rslt = collectSendBuffs(&send_iov, iov_num, total_size);
		aos_assert_rl(rslt, mSendLock, false);

		if(iov_num == 0)
		{
			aos_assert_rl(total_size == 0, mSendLock, false);
			while(mSendQueueNum == 0)
			{
				// when is wakeup. mSock maybe has changed.
				//OmnScreen << "send time wait!!!! " << mSock << endl;
				bool timeout = true;
				mEpollSendVar->timedWait(mSendLock, timeout, 5);
				mThreadStatus2 = true;
				if(mSock != -1 && mSock != crt_sock)	crt_sock = mSock;
			}
			mSendLock->unlock();
			
			continue;
		}

		aos_assert_r(iov_num > 0 &&  send_iov, false);
		int bytes_send = sendToSock(crt_sock, send_iov, iov_num); 
		OmnDelete []send_iov;
		
		if(bytes_send <= 0)
		{
			mSendLock->unlock();
			
			OmnScreen << "send error! errno:" << errno
				<< "; " << strerror(errno) << endl;
	
			//if(errno != EAGAIN)
			//{
			//	closeConn(crt_sock);
			//}
			break;
		}
	
		moveSendBuffsPos(bytes_send);
		mSendLock->unlock();

		if((u32)bytes_send < total_size)		 break;	 // means can't send anymore.
	}

	return true;
}


bool
AosSockConn::collectSendBuffs(iovec ** send_iov_ptr, u32 &iov_num, u32 &total_size)
{
	iov_num = 0;
	total_size = 0;
	if(mSendMsgLenPos != 0)
	{
		aos_assert_r(mSendBuffs.size() >=1, false);	
		aos_assert_r(mSendBuffs[0] && mSendBuffs[0]->getCrtIdx() == 0, false);
	}

	for(u32 i=0; i<mSendBuffs.size(); i++)
	{
		// this buffs are don't send at last time.
		AosBuffPtr buff = mSendBuffs[i];
		aos_assert_r(buff, false);
		
		if(i == 0)
		{
			u32 crt_idx = buff->getCrtIdx();	
			
			if(crt_idx != 0)
			{
				// the len has send.
				total_size += buff->dataLen() - crt_idx;
				aos_assert_r(mSendMsgLenPos == 0, false);
				iov_num += 1;
				continue;
			}
			
			u32 len_size = mSendMsgLenPos ? 4 - mSendMsgLenPos : 4;
			total_size += (len_size + buff->dataLen());
			iov_num += 2;
			continue;
		}
		
		// Ketty 2013/10/22
		//aos_assert_r(mSendMsgLenPos ==0 && buff->getCrtIdx() == 0, false);	
		aos_assert_r(buff->getCrtIdx() == 0, false);	
		total_size += (4 + buff->dataLen());
		iov_num += 2;
	}
	
	mLock->lock();
	while(total_size < eRecvBuffSize)
	{
		AosConnMsgPtr msg = getSendMsgLocked();
		if(!msg)	break;
			
		AosBuffPtr msg_buff = msg->getConnBuff();
		aos_assert_rl(msg_buff, mLock, false);

		msg_buff->reset();
	
		msg = AosConnMsg::serializeFromStatic(msg_buff, true);
		aos_assert_r(msg, false);
		
		if(mShowLog)
		{
			//AosTrans* trans = (AosTrans*)msg.getPtr();
			OmnScreen << "SockConn:: sock:" << mSock
				<< "; name:" << mName
				<< ";  collect sendBuff. append SendBuff" 
				<< "; this:" << msg_buff.getPtr()
				<< "; len:" << msg_buff->dataLen()
				//<< "; trans_id:" << trans->getTransId().toString()
				<< endl;
		}

		msg_buff->setCrtIdx(0);
		mSendBuffs.push_back(msg_buff);
		total_size += (4 + msg_buff->dataLen());	// 4 is  the len_size.
		iov_num +=2;
	}
	mLock->unlock();
	
	if(mSendBuffs.size() == 0)	return true;

	aos_assert_r(iov_num, false);
	iovec * send_iov = OmnNew iovec[iov_num];
	
	u32 iov_idx = 0;
	for(u32 i= 0; i< mSendBuffs.size(); i++)
	{
		AosBuffPtr buff = mSendBuffs[i]; 
		aos_assert_r(buff, false);
		
		if(i == 0)
		{
			u32 crt_idx = buff->getCrtIdx();	
			if(crt_idx != 0)
			{
				// means this mSendBuff has send. but not finish.
				send_iov[iov_idx].iov_base = buff->data() + crt_idx; 
				int remain_size = buff->dataLen() - crt_idx;
				send_iov[iov_idx].iov_len = remain_size; 
				iov_idx++;
				
				//if(mTestRemainSendSize && mTestRemainSendSize != remain_Size)
				//{
				//	OmnAlarm << "error!"
				//		<< "actual remain:" << mTestRemainSendSize
				//		<< "; crt_remain:" << remain_size
				//		<< "; crt_idx:" << crt_idx
				//		<< enderr;
				//}

				continue;
			}
			
			u32 len_size = mSendMsgLenPos ? 4 - mSendMsgLenPos : 4;
		
			send_iov[iov_idx+1].iov_base = buff->data(); 
			send_iov[iov_idx+1].iov_len = buff->dataLen(); 
			send_iov[iov_idx].iov_base = (((char*)(&(send_iov[iov_idx+1].iov_len))) + mSendMsgLenPos);
			send_iov[iov_idx].iov_len = len_size;
	
			iov_idx += 2;
			continue;
		}
	
		// Ketty 2013/10/22
		//aos_assert_r(mSendMsgLenPos ==0 && buff->getCrtIdx() == 0, false);	
		aos_assert_r(buff->getCrtIdx() == 0, false);	
		send_iov[iov_idx+1].iov_base = buff->data(); 
		send_iov[iov_idx+1].iov_len = buff->dataLen(); 
		send_iov[iov_idx].iov_base = &(send_iov[iov_idx+1].iov_len);
		send_iov[iov_idx].iov_len = 4;
		
		iov_idx += 2;
	}
	aos_assert_r(iov_idx == iov_num, false);

	//if(mTesterShow)
	if(mShowLog)
	{
		for(u32 i=0; i<iov_num; i++)
		{
			OmnScreen << "SockConn:: sock:" << mSock 
				<< "; name:" << mName
				<< "; aaaaaaaa: add to iov:"
				<< "; iov_idx:" << i 
				<< "; buff_len:" << send_iov[i].iov_len
				<< endl;
		}
	}
	*send_iov_ptr = send_iov;
	return true;
}

bool
AosSockConn::appendTestIOvBuff(const char* data, const int len)
{
	return true;
	AosBuffPtr buff = 0;
	if (AosSvrProxy::smTestIOVBuff.find(mName) == AosSvrProxy::smTestIOVBuff.end())
	{
		buff = OmnNew AosBuff(1024*1024*1024 AosMemoryCheckerArgs);
		AosSvrProxy::smTestIOVBuff[mName] = buff;
	}
	else
	{
		buff = AosSvrProxy::smTestIOVBuff.find(mName)->second;
	}

	aos_assert_r(len > 0, false);
	buff->setData1(data, len, true);
	if (buff->dataLen() > 1024*1024*1024)
	{
		buff = OmnNew AosBuff(1024*1024*1024 AosMemoryCheckerArgs);
		AosSvrProxy::smTestIOVBuff[mName] = buff;
	}
	return true;
}


bool	
AosSockConn::appendTestBuffForRecv(const char* data, const int len)
{
	return true;
	i64 size = (i64)1024*1024*1024*2;
	if (mTestData == 0)
	{
		mTestData = OmnNew char[size];
		memset(mTestData, 0, size);
	}
	if (!mUnfinishRecvData && mRecvMsgLenPos == 0)
	{
		if (mTestDataLen > size/2)
		{
			memset(mTestData, 0, size);
			mTestDataLen = 0;
		}
	}
	memcpy(mTestData+mTestDataLen, data, len);
	mTestDataLen += len;
	return true;
}


bool
AosSockConn::appendTestBuff(const char* data, const int len)
{
	return true;
	AosBuffPtr buff = 0;
	if (AosSvrProxy::smTestBuff.find(mName) == AosSvrProxy::smTestBuff.end())
	{
		buff = OmnNew AosBuff(1024*1024*1024 AosMemoryCheckerArgs);
		AosSvrProxy::smTestBuff[mName] = buff;
	}
	else
	{
		buff = AosSvrProxy::smTestBuff.find(mName)->second;
	}

	aos_assert_r(len > 0, false);
	buff->setCharStr(data, len);
	if (buff->dataLen() > 1024*1024*1024)
	{
		buff = OmnNew AosBuff(1024*1024*1024 AosMemoryCheckerArgs);
		AosSvrProxy::smTestBuff[mName] = buff;
	}
	return true;
}


bool
AosSockConn::moveSendBuffsPos(int bytes_send)
{
	if(bytes_send == 0)	return true;
	u32 i = 0;
	int erase_idx = -1;
	
	AosBuffPtr buff = mSendBuffs[0];
	aos_assert_r(buff, false);
	
	if(mSendMsgLenPos != 0 || buff->getCrtIdx() != 0)
	{
		moveUnfinishBuffPos(bytes_send, buff);	
		if(buff->getCrtIdx() < buff->dataLen())	return true;
		//if(bytes_send == 0)	return true;

		// This buff must erase.
		i = 1;	
		erase_idx = 0;
	}

	for(; i<mSendBuffs.size(); i++)
	{
		buff = mSendBuffs[i];
		aos_assert_r(buff, false);
		aos_assert_r(bytes_send >=0, false);
		if(bytes_send == 0) break;

		if(bytes_send <= 4)
		{
			// means the size send only length indiactor.
			mSendMsgLenPos = bytes_send;
			bytes_send = 0;
			//if(mTesterShow)
			if(mShowLog)
			{
				OmnScreen << "SockConn:: sock:" << mSock 
					<< "; name:" << mName
					<< "; aaaaaaaa: sendBuff len not finish:" 
					<< "; buff:" << buff.getPtr()
					<< "; remain_len_size:" << 4 - mSendMsgLenPos
					<< endl;
			}
			break;
		}

		bytes_send -= 4;
		if (bytes_send < buff->dataLen())
		{
			buff->setCrtIdx(bytes_send);
			bytes_send = 0;	
			//if(mTesterShow)
			if(mShowLog)
			{
				OmnScreen << "SockConn:: sock:" << mSock 
					<< "; name:" << mName
					<< "; aaaaaaaa: sendBuff not finish:" 
					<< "; buff:" << buff.getPtr()
					<< "; remain_buff_size:" << buff->dataLen() - buff->getCrtIdx()
					<< "; buff_len:" << buff->dataLen()
					<< endl;
			}
			//mTestRemainSendSize = buff->dataLen() - buff->getCrtIdx();
			break;
		}

		// This buff must erase.
		bytes_send -= buff->dataLen();
		erase_idx = i;
		
		//if(mTesterShow)
		if(mShowLog)
		{
			OmnScreen << "SockConn:: sock:" << mSock 
				<< "; name:" << mName
				<< "; aaaaaaaa: sendBuff succ "
				<< "; len:" << buff->dataLen()
				<< "; ptr:" << buff.getPtr()
				<< endl;
		}
	}
	if(i == mSendBuffs.size())	aos_assert_r(bytes_send == 0, false);

	mSendBuffs.erase(mSendBuffs.begin(), mSendBuffs.begin() + (erase_idx + 1));
	return true;	
}
		
bool
AosSockConn::moveUnfinishBuffPos(int &bytes_send, const AosBuffPtr &buff)
{
	aos_assert_r(mSendMsgLenPos !=0 || buff->getCrtIdx() != 0, false);
	
	// when come here. the first buff's len_size has send.
	if(mSendMsgLenPos != 0)
	{
		if(bytes_send < 4-mSendMsgLenPos)
		{
			// means the size send not finished.
			mSendMsgLenPos += bytes_send;	
			bytes_send = 0;
			//if(mTesterShow)
			if(mShowLog)
			{
				OmnScreen << "SockConn:: sock:" << mSock 
					<< "; name:" << mName
					<< "; aaaaaaaa: sendBuff len continue not finish:" 
					<< "; buff:" << mSendBuffs[0].getPtr()
					<< "; remain_len_size:" << 4 - mSendMsgLenPos
					<< endl;
			}
			return true;	
		}
	
		bytes_send -= 4-mSendMsgLenPos;
		mSendMsgLenPos = 0;
	}

	u32 remain_size = buff->dataLen() - buff->getCrtIdx();
	aos_assert_r(bytes_send >=0, false);
		
	if ((u32)bytes_send < remain_size)
	{
		buff->setCrtIdx(buff->getCrtIdx() + bytes_send);
		bytes_send = 0;
		
		//if(mTesterShow)
		if(mShowLog)
		{
			OmnScreen << "SockConn:: sock:" << mSock 
				<< "; name:" << mName
				<< "; aaaaaaaa: sendBuff continue not finish:" 
				<< "; buff:" << buff.getPtr()
				<< "; remain_buff_size:" << buff->dataLen() - buff->getCrtIdx()
				<< "; buff_len:" << buff->dataLen()
				<< endl;
		}
		//mTestRemainSendSize = buff->dataLen() - buff->getCrtIdx();
		return true;
	}
	
	buff->setCrtIdx(buff->getCrtIdx() + remain_size);
	bytes_send -= remain_size;
	//if(mTesterShow)
	if(mShowLog)
	{
		OmnScreen << "SockConn:: sock:" << mSock 
			<< "; name:" << mName
			<< "; aaaaaaaa: continue sendBuff succ" 
			<< "; len:" << buff->dataLen()
			<< "; ptr:" << buff.getPtr()
			<< endl;
	}
	return true;
}


AosConnMsgPtr
AosSockConn::getSendMsgLocked()
{
	if(mSendQueueNum == 0)	return 0;

	AosConnMsgPtr msg = mSendQueue.front();
	aos_assert_r(msg, 0);
	mSendQueue.pop_front();	
	mSendQueueNum--;

	if(!msg->isAppMsg())
	{
		if(mShowLog)
		{
			OmnScreen << "SockConn:: sock:" << mSock << "; clt" 
				<< "; name:" << mName
				<< "; send conn msg:" 
				<< endl;
		}
		return msg;	
	}

	// add to WaitAck Cache.
	AosAppMsgPtr a_msg = (AosAppMsg *)msg.getPtr();
	mWaitAckCache.push_back(a_msg);
	mWaitAckNum++;

	if(mShowLog)
	{
		OmnScreen << "SockConn:: sock:" << mSock << "; clt" 
			<< "; name:" << mName
			<< "; send app msg:" << a_msg->getRlbSeq()
			<< "; type:" << a_msg->getStrType()
			<< "; wait_ack"
			<< "; wait_ack_num:" << mWaitAckNum
			<< endl;
		
		if(a_msg->isTrans())
		{
			AosTransPtr trans = (AosTrans *)a_msg.getPtr();
			OmnScreen << "Trans Perf" 
				<< "; trans_id:" << trans->getTransId().toString()
				<< "; time:" << OmnGetTimestamp()
				<< "; SockConn ready send."
				<< endl;
		}
	}
		
	//if(mTesterShow)
	if(mShowLog)
	{
		OmnScreen << "SockConn:: sock:" << mSock 
			<< "; name:" << mName
			<< "; aaaaaaaa: send app_msg" 
			<< "; opr:" << a_msg->getStrType()
			<< "; buff_len:" << msg->getConnBuff()->dataLen() 
			<< "; SockConn ready send."
			<< endl;
	}
	return msg;
}


int
AosSockConn::sendToSock(const int crt_sock, iovec * iov, int iov_num)
{
	aos_assert_r(iov && iov_num >= 0, false);
	for(u32 i=0; i<(u32)iov_num; i++)
	{
		aos_assert_r(iov[i].iov_base, 0);
	}
	
	int bytes_write = ::writev(crt_sock, iov, iov_num);
	if (bytes_write > 0)
	{
		int need_write = bytes_write;
		for(u32 i=0; i<(u32)iov_num; i++)
		{
			if (need_write == 0) 
				break;
			if ((int)iov[i].iov_len > need_write)
			{
				appendTestIOvBuff((char*)iov[i].iov_base, need_write);
				break;
			}
			else 
			{
				appendTestIOvBuff((char*)iov[i].iov_base, iov[i].iov_len);
				need_write -= iov[i].iov_len;
			}
		}
	}
	return bytes_write;
}


bool
AosSockConn::checkWaitAckCacheLocked()
{
	if(mWaitAckNum < eMaxWaitAckNum) return true;
	//if(mWaitAckCache.size() < eMaxWaitAckNum)	return true;
	
	// means there maybe some conn error.
	if(mSendQueueNum)
	{
		AosConnMsgPtr msg = mSendQueue.front();
		if(msg->getType() == MsgType::eGetWaitSeq)	return true;
	}

	if(mShowLog)
	{
		OmnScreen << "SockConn:: sock:" << mSock << "; clt" 
			<< "; name:" << mName
			<< "; waitAckCache size:" << mWaitAckNum
			<< "; getSvrWaitSeq."
			<< endl; 
	}
	
	AosConnMsgGetWaitSeqPtr msg = OmnNew AosConnMsgGetWaitSeq();
	return sendConnMsgLocked(msg);
}


bool
AosSockConn::sendConnMsg(const AosConnMsgPtr &msg)
{
	mLock->lock();
	bool rslt = sendConnMsgLocked(msg);
	mLock->unlock();
	
	aos_assert_r(rslt, false);
	return true;
}


bool
AosSockConn::sendConnMsgLocked(const AosConnMsgPtr &msg)
{
	aos_assert_r(msg, false);
	list<AosConnMsgPtr>::iterator itr = mSendQueue.begin();
	for(; itr != mSendQueue.end(); itr++)
	{
		if((*itr)->isAppMsg())	break;
	}
	mSendQueue.insert(itr, msg);
	mSendQueueNum++;

	// Ketty temp.
	//mEpollSendVar->signal();
	return true;
}


bool
AosSockConn::smartSend(const AosAppMsgPtr &msg, bool &is_ignore, bool &bad_conn)
{
	is_ignore = false;
	bad_conn = false;

	mLock->lock();
	if(mSock <=0)
	{
		mLock->unlock();
		bad_conn = true;
		return true;
	}
	
	if((u32)mSendQueueSize > mMaxCacheSize)
	{
		OmnScreen << "!!!!!! Warning !!!!!" << endl;
		if((u32)mSendQueueSize > mMaxCacheSize) checkWaitAckCacheLocked();
		mLock->unlock();
		is_ignore = true;
		return true;
	}

	u32 msg_seq = mCrtSendRlbSeq++;
	msg->setRlbSeq(msg_seq);
	mSendQueue.push_back(msg.getPtr());
	mSendQueueSize += msg->getSize();
	mSendQueueNum++;

	if(mShowLog)
	{
		OmnScreen << "SockConn:: sock:" << mSock << "; clt" 
			<< "; name:" << mName
			<< "; smartSend: " << msg_seq
			<< "; type:" << msg->getStrType() 
			//<< "; trans_id:" << trans_id
			<< endl;
		if(msg->isTrans())
		{
			AosTransPtr trans = (AosTrans *)msg.getPtr();
			OmnScreen << "Trans Perf" 
				<< "; trans_id:" << trans->getTransId().toString()
				<< "; time:" << OmnGetTimestamp()
				<< "; SockConn smartsend."
				<< endl;
		}
	}

	mEpollSendVar->signal();
	mLock->unlock();
	return true;
}


AosAppMsgPtr
AosSockConn::smartReadAsync()
{
	if(mSock <= 0 ) return 0;

	mLock->lock();
	if(mRecvQueue.size() == 0)
	{
		aos_assert_rl(mRecvQueueSize == 0, mLock, 0);
		mLock->unlock();
		return 0;
	}
	
	AosAppMsgPtr msg = mRecvQueue.front();
	mRecvQueue.pop();
	aos_assert_rl(msg, mLock, 0);
	
	mRecvQueueSize -= msg->getSize();
	aos_assert_rl(mRecvQueueSize >=0, mLock, 0);
	if(mRecvQueueSize <= eMinCacheSize)	mEpollRecvVar->signal(); 
	
	mLock->unlock();
	return msg;
}


AosAppMsgPtr
AosSockConn::smartReadSync()
{
	if(mSock <= 0 ) return 0;

	mLock->lock();
	while(mRecvQueue.size() == 0)
	{
		aos_assert_rl(mRecvQueueSize == 0, mLock, 0);
		bool timeout = false;
		mRecvCondVar->timedWait(mLock, timeout, 5);
		//mSvrInfo->setThreadStatus();
	}

	AosAppMsgPtr msg = mRecvQueue.front();
	mRecvQueue.pop();
	aos_assert_rl(msg, mLock, 0);
	
	mRecvQueueSize -= msg->getSize();
	if(mRecvQueueSize <= eMinCacheSize)	mEpollRecvVar->signal(); 
	
	mLock->unlock();
	return msg;
}


void
AosSockConn::cleanQueue()
{
	OmnScreen << "SockConn:: clean send queue."
		<< "; mSock:" << mSock << endl;
	
	mSendLock->lock();
	mLock->lock();
//OmnScreen << "============== mSendBuffs size: " << mSendBuffs.size() << endl;
	mSendBuffs.clear();
	mSendMsgLenPos = 0;
	mCrtSendRlbSeq = 1;
	mSendQueue.clear();
	mSendQueueSize = 0;
	mSendQueueNum = 0;
//OmnScreen << "=============== mWaitAckCache size:" << mWaitAckCache.size() << endl;
	mWaitAckCache.clear();
	mWaitAckNum = 0;
	mLock->unlock();
	mSendLock->unlock();

	mRecvLock->lock();
	mLock->lock();
	memset(mRecvBuff->data(), 0, mRecvBuff->dataLen());

	//mRecvBuff = 0;
	mUnfinishRecvData = 0;
	mRecvMsgLen = 0;
	mRecvMsgLenPos = 0;
	mCrtRecvRlbSeq = 0;
	//for(u32 i=0; i<mRecvQueue.size(); i++)
//OmnScreen << "=============== mRecvQueue size:" << mRecvQueue.size() << endl;
	u32 nn = mRecvQueue.size();
	for(u32 i=0; i<nn; i++)
	{
		mRecvQueue.pop();
	}
	//mRecvQueue.clear();
	mRecvQueueSize = 0;
//OmnScreen << "=============== mRecvQueue size:" << mRecvCache.size() << endl;
	mRecvCache.clear();
	mLock->unlock();
	mRecvLock->unlock();
}

