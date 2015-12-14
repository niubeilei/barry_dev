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
#include "CubeComm/CubeCommTcpSvr.h"

#include "CubeComm/CubeCommTcpSvrProc.h"


AosCubeCommTcpSvr::AosCubeCommTcpSvr(const AosEndPointInfo epinfo)
:
AosCubeComm("cubecomm_tcp_svr", 0),
mEPInfo(epinfo)
{
	OmnCommListenerPtr thisptr(this, false);
	AosNetReqProcPtr proc = OmnNew AosCubeCommTcpSvrProc(thisptr);
	mReqDistr = OmnNew AosReqDistr(proc);
	OmnString ipAddr = mEPInfo.mIpAddr.toString();
	OmnString port;
	port << mEPInfo.mPort << '-' << mEPInfo.mPort;
	bool rslt = mReqDistr->config(ipAddr, port, mName, 100);
	if (!rslt)
	{
		OmnThrowException("failed to create cube comm tcp svr");
	}
		
	mReqDistr->start();
}


AosCubeCommTcpSvr::~AosCubeCommTcpSvr()
{
}


bool 	
AosCubeCommTcpSvr::connect(AosRundata *rdata)
{
	return true;
}


bool 	
AosCubeCommTcpSvr::close()
{
	return true;
}


bool 	
AosCubeCommTcpSvr::startReading(
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
AosCubeCommTcpSvr::stopReading()
{
	return true;
}


bool 	
AosCubeCommTcpSvr::sendTo(
		AosRundata *rdata, 
		const AosEndPointInfo &remote_epinfo,
		AosBuff *buff)
{
	AosWebRequestPtr webReq = rdata->getReq();
	aos_assert_r(webReq, false);

	OmnTcpClientPtr conn = webReq->getConn();
	aos_assert_r(conn, false);

	return conn->smartSend(buff->data(), buff->dataLen());
}


bool
AosCubeCommTcpSvr::proc(const OmnConnBuffPtr &connbuff)
{
	return mCaller->msgRead(connbuff);
}

