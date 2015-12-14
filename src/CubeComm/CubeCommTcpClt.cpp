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
// 2014/11/08 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "CubeComm/CubeCommTcpClt.h"


AosCubeCommTcpClt::AosCubeCommTcpClt(const OmnCommListenerPtr &caller)
:
AosCubeComm("cubecomm_tcp_clt", caller)
{
}


AosCubeCommTcpClt::~AosCubeCommTcpClt()
{
}


bool 	
AosCubeCommTcpClt::connect(AosRundata *rdata)
{
	return true;
}


bool 	
AosCubeCommTcpClt::close()
{
	mLockRaw->lock();
	map<int, queue<OmnTcpClientPtr> >::iterator itr	= mConns.begin();
	while (itr != mConns.end())
	{
		while (!itr->second.empty())
		{
			OmnTcpClientPtr conn = itr->second.front();
			itr->second.pop();
			conn->closeConn();
		}
		itr++;
	}
	mConns.clear();
	mLockRaw->unlock();		
	return true;
}


bool 	
AosCubeCommTcpClt::startReading(
		AosRundata *rdata, 
		const OmnCommListenerPtr &caller)
{
	mCaller = caller;
	if (!mCaller)
	{
		AosLogError(rdata, false, AosErrmsgId::eInternalError) << enderr;
		return false;
	}

	return true;
}


bool 	
AosCubeCommTcpClt::stopReading()
{
	if (mThread.isNull())
	{
		return true;
	}

	return mThread->stop();
}


OmnTcpClientPtr
AosCubeCommTcpClt::getConn(const AosEndPointInfo remote_epinfo)
{
	OmnTcpClientPtr conn;
	mLock->lock();
	map<int, queue<OmnTcpClientPtr> >::iterator itr = mConns.find(remote_epinfo.mEpId);
	if (itr != mConns.end() && !itr->second.empty())
	{
		conn = itr->second.front();
		itr->second.pop();
		mLock->unlock();
		return conn;
	}
	mLock->unlock();
		
	conn = OmnNew OmnTcpClient("nn", remote_epinfo.mIpAddr,
		remote_epinfo.mPort, 1, eAosTLT_FirstFourHigh);
	OmnString errmsg = "";
	bool rslt = conn->connect(errmsg);
	if (!rslt)
	{
		//comment it out for now
		//OmnAlarm << "error:" << errmsg << enderr;
		return 0;
	}
	return conn;
}


void 
AosCubeCommTcpClt::returnConn(
		const AosEndPointInfo remote_epinfo,
		const OmnTcpClientPtr &conn)
{
	mLock->lock();
	map<int, queue<OmnTcpClientPtr> >::iterator itr = mConns.find(remote_epinfo.mEpId);
	if (itr != mConns.end())
	{
		itr->second.push(conn);
	}
	else
	{
		queue<OmnTcpClientPtr> que;
		que.push(conn);
		mConns[remote_epinfo.mEpId] = que;
	}
	mLock->unlock();
}


bool 	
AosCubeCommTcpClt::sendTo(
		AosRundata *rdata, 
		const AosEndPointInfo &remote_epinfo,
		AosBuff *buff)
{
	OmnTcpClientPtr conn = getConn(remote_epinfo);

	//commit it out for now
	if (!conn) return false;

	aos_assert_r(conn, false);

	OmnRslt rslt = conn->smartSend(buff->data(), buff->dataLen());
	aos_assert_r(rslt, false);

	//temparorily comment out the reading statements. 
	//reasons:
	//  1. for raft message sending, there is no need
	//     to get the response
	//  2. sometimes, the other side may not send response
	//     timely or hit some exception, therefore not able
	//     to send the response. In this case, the sender
	//     will block
	//  3. later on, if we can find a better solution to
	//     solve raft block issue, the following code may
	//     not be commented out

	OmnConnBuffPtr connbuff;
	rslt = conn->smartRead(connbuff);
	if (rslt)
	{
		returnConn(remote_epinfo, conn);
		msgRead(connbuff);
	}

	return true;
}


bool
AosCubeCommTcpClt::proc(const OmnConnBuffPtr &connbuff)
{
	return mCaller->msgRead(connbuff);
}

