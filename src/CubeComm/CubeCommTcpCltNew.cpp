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
// This cube comm provides async sends. After a JimoCall is sent,
// the function returns. When a response is received, it checks
// the JimoCall ID. If the JimoCall ID is found, it calls the
// JimoCall back.
//
// Modification History:
// 2014/11/08 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "CubeComm/CubeCommTcpCltNew.h"

#include "JimoCall/JimoCall.h"
#include "UtilComm/TcpCommClt.h"


AosCubeCommTcpCltNew::AosCubeCommTcpCltNew(
		const OmnCommListenerPtr &caller)
:
AosCubeComm("cubecomm_tcp_clt", caller)
{
}


AosCubeCommTcpCltNew::~AosCubeCommTcpCltNew()
{
}


bool
AosCubeCommTcpCltNew::connect(AosRundata *rdata)
{
	return true;
}


bool
AosCubeCommTcpCltNew::close()
{
	mLockRaw->lock();
	itr_t itr = mConns.begin();
	while (itr != mConns.end())
	{
		while (!itr->second.empty())
		{
			OmnTcpCommCltPtr conn = itr->second.front();
			itr->second.pop();
			conn->closeConn();
		}
		itr++;
	}
	mConns.clear();
	mLockRaw->unlock();
	return true;
}


OmnTcpCommCltPtr
AosCubeCommTcpCltNew::getConn(const AosEndPointInfo remote_epinfo)
{
	OmnTcpCommCltPtr conn;
	mLock->lock();
	itr_t itr = mConns.find(remote_epinfo.mEpId);
	if (itr != mConns.end() && !itr->second.empty())
	{
		conn = itr->second.front();
		itr->second.pop();
		mLock->unlock();
		return conn;
	}
	mLock->unlock();

	conn = OmnNew OmnTcpCommClt(remote_epinfo.mIpAddr,
		remote_epinfo.mPort, 1, eAosTLT_FirstFourHigh, __FILE__, __LINE__, true);
	OmnString errmsg = "";
	bool rslt = conn->connect(errmsg);

	if (!rslt)
	{
		//comment it out for now
		//OmnAlarm << "error:" << errmsg << enderr;
		return 0;
	}
	conn->setBlockingType(false);  //by White 2015-8-20 16:20
	OmnCommListenerPtr thisptr(this, false);
	conn->startReading(thisptr);
	return conn;
}


void
AosCubeCommTcpCltNew::returnConn(
		const AosEndPointInfo remote_epinfo,
		const OmnTcpCommCltPtr &conn)
{
	mLock->lock();
	itr_t itr = mConns.find(remote_epinfo.mEpId);
	if (itr != mConns.end())
	{
		itr->second.push(conn);
	}
	else
	{
		queue<OmnTcpCommCltPtr> que;
		que.push(conn);
		mConns[remote_epinfo.mEpId] = que;
	}
	mLock->unlock();
}


bool
AosCubeCommTcpCltNew::sendTo(
		AosRundata *rdata,
		const AosEndPointInfo &remote_epinfo,
		AosBuff *buff)
{
	OmnTcpCommCltPtr conn = getConn(remote_epinfo);

	//commit it out for now
	if (!conn) return false;

	aos_assert_r(conn, false);

	bool rslt = conn->smartSend(buff->data(), buff->dataLen());
//	aos_assert_r(rslt, false);
	if (!rslt)
	{
		OmnScreen << "failed on sending msg to endpoint:" << remote_epinfo.toStr() << endl;
		return false;
	}
	OmnScreen << "msg sent to endpoint:" << remote_epinfo.toStr() << endl;
	returnConn(remote_epinfo, conn);
	return true;
}


bool
AosCubeCommTcpCltNew::proc(const OmnConnBuffPtr &connbuff)
{
	return mCaller->msgRead(connbuff);
}


bool
AosCubeCommTcpCltNew::msgRead(const OmnConnBuffPtr &connbuff)
{
	aos_assert_r(mCaller, false);

	mLockRaw->lock();
	mQueue.push(connbuff);
	mCondVar->signal();
	mLockRaw->unlock();
	return true;
}

