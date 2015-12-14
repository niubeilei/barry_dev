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
// Created: 04/17/2013 by Ketty 
////////////////////////////////////////////////////////////////////////////
#include "SvrProxyMgr/SvrInfo.h"

#include "API/AosApi.h"
#include "SEInterfaces/SvrProxyObj.h"
#include "SvrProxyMgr/SockConn.h"
#include "Porting/Sleep.h"
#include "TransUtil/TransAckMsg.h"

#include "TransUtil/TransSvrDeathMsg.h"
//#include "SysMsg/TriggerResendMsg.h"

//AosResendCheckThrdPtr AosSvrInfo::sgResendThrd = OmnNew AosResendCheckThrd();

AosSvrInfo::AosSvrInfo(
		const u32 max_cache_size,
		const AosRecvEventHandlerPtr &recv_hd,
		bool show_log)
:
mLock(OmnNew OmnMutex()),
mSvrDeath(false),
mShowLog(show_log)
{
	AosSvrInfoPtr thisptr(this, false);
	mConn = OmnNew AosSockConn(max_cache_size, recv_hd, thisptr, mShowLog);
}

AosSvrInfo::~AosSvrInfo()
{
}


bool
AosSvrInfo::stop()
{
	if(!mConn) return false;
	return mConn->stop();
}


bool
AosSvrInfo::setConnSock(const int sock_id)
{
	mLock->lock();
	bool rslt = setConnSockPriv(sock_id);	
	mLock->unlock();
	aos_assert_r(rslt, false);	
	return true;
}

bool
AosSvrInfo::resetConnSock()
{
	mLock->lock();
	mConn->resetConnSock();
	mLock->unlock();
	return true;
}

	
bool
AosSvrInfo::setConnFailed()
{
	return stop();
}


bool
AosSvrInfo::setConnSockPriv(const int sock_id)
{
	aos_assert_r(sock_id > 0, false);	

	if(isConnGood())
	{
		OmnScreen << "setConn. But the conn is good:"
			<< "; old_conn:" << mConn->getSock()
			<< endl;
	}
	mConn->setName(mName);

	bool rslt = mConn->swap(sock_id);
	aos_assert_r(rslt, false);
	
	return true;
}


int
AosSvrInfo::getSockId()
{
	return mConn->getSock();
}

bool
AosSvrInfo::setSvrDeath()
{
	if(mSvrDeath)	return true;
	
	mSvrDeath = true;
	stop();
	return true;
}


bool
AosSvrInfo::isConnGood()
{
	if(!mConn)  return false;
	return mConn->isGood();
}

bool
AosSvrInfo::isSvrReady()
{
	return isConnGood();
}


AosAppMsgPtr
AosSvrInfo::smartReadSync()
{
	return mConn->smartReadSync();
}


bool
AosSvrInfo::sendAppMsg(const AosAppMsgPtr &msg, bool &succ)
{
	if(msg->getType() == MsgType::eTransAckMsg)
	{
		//AosTransAckMsg* ack_msg = (AosTransAckMsg*)msg.getPtr();
		
		//OmnScreen << "SvrProxy RRecv trans ack :"
		//	<< (ack_msg->getReqId()).toString() << endl;
	}

	if(mSvrDeath)
	{
		succ = false;
		return true;
	}
	return sendAppMsgPriv(msg, succ);
}


bool
AosSvrInfo::sendAppMsgPriv(const AosAppMsgPtr &msg, bool &succ)
{
	bool is_ignore, bad_conn;
	while(1)
	{
		sendMsgPriv(msg, is_ignore, bad_conn);
		if(bad_conn)
		{
			succ = false;
			return true;
		}
		if(!is_ignore)	break;

		OmnSleep(1);		// this msg is ignored. try again.
		OmnScreen << "this msg is been ignore. try again."
			<< "; msg_type:" << msg->getType()
			<< endl;
	}
	succ = true;
	return true;
}

bool
AosSvrInfo::sendTrans(const AosTransPtr &trans)
{
	//OmnScreen << "SvrProxy Send trans:" 
	//	<< trans->getTransId().toString()
	//	<< "; svr_death:" << mSvrDeath
	//	<< "; is_svr_ready:" << isSvrReady()
	//	<< endl;

	if(mSvrDeath) return sendSvrDeathMsg(trans, AosGetSelfServerId());	
	if(!isSvrReady())	return true;
	
	bool rslt;
	mLock->lock();
	ResendWaiterPtr waiter = getResendWaiter(trans->getCltKey(), true);
	aos_assert_rl(waiter, mLock, false);
	if(!waiter->mWaitAllResend)	
	{
		//OmnScreen << "SvrProxy Send trans2222:" << trans->getTransId().toString() << endl;
		rslt = sendOneTrans(trans);	
		aos_assert_rl(rslt, mLock, false);
		mLock->unlock();
		return true;
	}
	
	// means this clt is wait all resend trans.
	OmnScreen << "send trans add to waiter." 
		<< ". trans_type:" << trans->getType()
		<< ". trans_id:" << trans->getTransId().toString()
		<< ". from_svr_id:" << trans->getFromSvrId()
		<< ". from_proc_id:" << trans->getFromProcId()
		<< ". to_svr_id:" << trans->getToSvrId()
		<< ". to_proc_id:" << trans->getToProcId()
		<< endl;

	(waiter->mAllTrans).insert(make_pair(trans->getTransId(), trans));
	mLock->unlock();

	bool wait_is_ipc_conn;
	u32 wait_id;
	rslt = getId(wait_is_ipc_conn, wait_id);
	//aos_assert_r(wait_id, false);
	
	AosSvrProxyObjPtr svr_proxy = AosSvrProxyObj::getSvrProxy();
	aos_assert_r(svr_proxy, false);
	svr_proxy->addWaitResendEnd(trans->getFromSvrId(),
			trans->getFromProcId(), wait_is_ipc_conn, wait_id);
	return true;
}

bool
AosSvrInfo::sendSvrDeathMsg(const AosTransPtr &orig_trans, const int death_svr_id)
{
	if(mShowLog)
	{
		//OmnScreen << "trans send failed; may be target death."
		//	<< "; trans_id:" << trans_id.toString()
		//	<< "; to_sid:" << trans->getToSvrId() 
		//	<< "; to_pid: " << trans->getToProcId()
		//	<< endl;
	}

	int to_svr_id = orig_trans->getFromSvrId();
	u32 to_proc_id = orig_trans->getFromProcId();
	AosTransId req_id = orig_trans->getTransId();
	AosAppMsgPtr sys_msg = OmnNew AosTransSvrDeathMsg(to_svr_id,
			to_proc_id, req_id, death_svr_id);

	AosSvrProxyObjPtr svr_proxy = AosSvrProxyObj::getSvrProxy();
	aos_assert_r(svr_proxy, false);
	bool succ;
	bool rslt = svr_proxy->sendAppMsg(sys_msg, succ);
	aos_assert_r(rslt, false);
	return true;
}

bool
AosSvrInfo::sendOneTrans(const AosTransPtr &trans)
{
	aos_assert_r(trans, false);
	
	if (mShowLog)
	{
	OmnScreen << "send trans." 
		<< ". trans_type:" << trans->getType()
		<< ". trans_id:" << trans->getTransId().toString()
		<< ". from_svr_id:" << trans->getFromSvrId()
		<< ". from_proc_id:" << trans->getFromProcId()
		<< ". to_svr_id:" << trans->getToSvrId()
		<< ". to_proc_id:" << trans->getToProcId()
		<< endl;
	}
	
	bool rslt, is_ignore, bad_conn;
	if(hasIgnore())
	{
		OmnScreen << "ignore trans: " << trans->getTransId().toString() << endl;
		// means there has ignore trans.
		addIgnoreTrans(trans);

		rslt = sendIgnoreTrans();
		aos_assert_r(rslt, false);
		return true;
	}

	rslt = sendMsgPriv(trans.getPtr(), is_ignore, bad_conn);
	aos_assert_r(rslt, false);
	if(bad_conn)	return true;

	if(is_ignore)
	{
		addIgnoreTrans(trans);
		return true;
	}
	
	return true;
}


bool
AosSvrInfo::sendAllTrans(map<AosTransId, AosTransPtr> & rs_trans)
{
	// means all resend has come.
	//if(mShowLog) OmnScreen << "IMP: send all resend trans." << endl;
	OmnScreen << "IMP: send all resend trans." << endl;

	bool rslt, is_ignore, bad_conn;
	//setitr_f itr = rs_trans.begin();
	map<AosTransId, AosTransPtr>::iterator itr = rs_trans.begin();
	for(; itr != rs_trans.end(); itr++)
	{
		//AosTransPtr tt = *(itr);
		AosTransPtr tt = itr->second;
		rslt = sendMsgPriv(tt.getPtr(), is_ignore, bad_conn);
		aos_assert_r(rslt, false);
	
		OmnScreen << "IMP: send trans. " << tt->getTransId().toString() << endl;
		
		if(bad_conn) return true;
		if(is_ignore) break;
	}

	// add the remain trans to mIgnoreTrans.
	for(; itr != rs_trans.end(); itr++)
	{
		//AosTransPtr tt = *itr;
		AosTransPtr tt = itr->second;
		addIgnoreTrans(tt);
	}
	
	return true;
}


bool
AosSvrInfo::addIgnoreTrans(const AosTransPtr &trans)
{
	aos_assert_r(trans, false);
	TransIdExpand ide = {trans->getTransId(), (u32)trans->getToSvrId(), (u32)trans->getToProcId()};
	set<TransIdExpand>::iterator itr = mIgnoreTransIds.find(ide);
	if(itr != mIgnoreTransIds.end()) return true;

	mIgnoreTransIds.insert(ide);
	mIgnoreTrans.push_back(trans);
	if(mShowLog)
	{
		OmnScreen << "this trans is ignored"
			<< "; trans_id:" << trans->getTransId().toString()
			<< "; to_sid:" << trans->getToSvrId() 
			<< "; to_pid:" << trans->getToProcId() 
			<< endl;
	}
	return true;
}


bool
AosSvrInfo::sendIgnoreTrans()
{
	if(!hasIgnore())	return true;
	bool rslt, is_ignore, bad_conn;
	
	while(!mIgnoreTrans.empty())
	{
		AosTransPtr trans = mIgnoreTrans.front();
		rslt = sendMsgPriv(trans.getPtr(), is_ignore, bad_conn);
		aos_assert_r(rslt, false);
		if(is_ignore || bad_conn)	break;
		
		TransIdExpand ide = {trans->getTransId(), (u32)trans->getToSvrId(), (u32)trans->getToProcId()};
		set<TransIdExpand>::iterator itr = mIgnoreTransIds.find(ide);
		aos_assert_r(itr != mIgnoreTransIds.end(), false);
		mIgnoreTransIds.erase(itr);
		mIgnoreTrans.pop_front();
		
		if(mShowLog)
		{
			OmnScreen << "send the ignore trans"
				<< "; trans_id:" << trans->getTransId().toString()
				<< "; to_sid:" << trans->getToSvrId()
				<< "; to_pid:" << trans->getToProcId() 
				<< endl;
		}
	}

	return true;
}


bool
AosSvrInfo::sendMsgPriv(
		const AosAppMsgPtr &msg, 
		bool &is_ignore,
		bool &bad_conn)
{
	bool rslt = mConn->smartSend(msg, is_ignore, bad_conn);
	aos_assert_r(rslt, false);
		
	if(is_ignore && mShowLog)
	{
		OmnScreen << "SvrInfo; send msg ignore this msg. sock:" << mConn->getSock() 
			<< "; smartSend:  maybe queue is full"
			<< "; msg_type:" << msg->getType()
			<< endl;
	}
	
	return true;
}


bool
AosSvrInfo::svrIsUp()
{
	mSvrDeath = false;
	
	bool rslt = setWaitResendTrans();
	aos_assert_r(rslt, false);
	return true;
}

bool
AosSvrInfo::setWaitResendTrans()
{
	//if(mShowLog)
	{
		OmnScreen << "IMP; set wait resend trans." << endl;
	}

	mLock->lock();
	map<u64, ResendWaiterPtr>::iterator itr = mResendWaiter.begin();
	ResendWaiterPtr waiter;
	for(; itr != mResendWaiter.end(); itr++)
	{
		waiter = itr->second;
		waiter->mWaitAllResend = true;
		(waiter->mAllTrans).clear();
	}

	mIgnoreTransIds.clear();
	mIgnoreTrans.clear();
	mLock->unlock();
	return true;
}


bool
AosSvrInfo::sendIgnoreTransPublic()
{
	// this func is called when SockConn is empty.
	if(!hasIgnore())	return true;
	
	mLock->lock();
	bool rslt = sendIgnoreTrans();
	mLock->unlock();
	aos_assert_r(rslt, false);
	return true;
}

AosSvrInfo::ResendWaiterPtr
AosSvrInfo::getResendWaiter(const u64 clt_key, const bool create)
{
	ResendWaiterPtr waiter;
	map<u64, ResendWaiterPtr>::iterator itr = mResendWaiter.find(clt_key);
	if(itr != mResendWaiter.end())
	{
		waiter = itr->second;
		return waiter;
	}
	if(!create)	return 0;
	
	waiter = OmnNew ResendWaiter(clt_key);
	mResendWaiter.insert(make_pair(clt_key, waiter));
	return waiter;
}


bool
AosSvrInfo::hasIgnore()
{
	return !mIgnoreTransIds.empty();
}

bool
AosSvrInfo::recvResendEnd(const u32 from_sid, const u32 from_pid)
{
	u64 clt_key = from_sid;
	clt_key = (clt_key << 32) + from_pid;
	
	mLock->lock();
	ResendWaiterPtr waiter = getResendWaiter(clt_key);
	if(!waiter)
	{
		mLock->unlock();
		return true;
	}

	bool rslt = sendAllTrans(waiter->mAllTrans);
	aos_assert_rl(rslt, mLock, false);

	waiter->mWaitAllResend = false;
	(waiter->mAllTrans).clear();
	mLock->unlock();
	
	bool wait_is_ipc_conn;
	u32 wait_id;
	rslt = getId(wait_is_ipc_conn, wait_id);
	//aos_assert_r(wait_id, false);
	
	AosSvrProxyObjPtr svr_proxy = AosSvrProxyObj::getSvrProxy();
	aos_assert_r(svr_proxy, false);
	svr_proxy->removeWaitResendEnd(from_sid, from_pid,
			wait_is_ipc_conn, wait_id);
	
	OmnScreen << "recv resend end."
		<< "; from_sid:" << from_sid
		<< "; from_pid:" << from_pid
		<< endl;
	return true;
}
