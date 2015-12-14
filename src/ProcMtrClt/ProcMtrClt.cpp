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
//
// Modification History:
// 09/24/2010: Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "ProcMtrClt/ProcMtrClt.h"

#include "Porting/Sleep.h"
#include "Porting/Process.h"
#include "SEUtil/SeXmlParser.h"
#include "SEUtil/XmlTag.h"
#include "UtilComm/TcpClient.h"
#include "UtilComm/ConnBuff.h"
#include "UtilMsg/ReqType.h"


AosProcMtrClt::AosProcMtrClt()
:
mLenInd(eAosTLT_FirstFourHigh)
{
	mProcid = OmnGetCrtPID();
}


AosProcMtrClt::~AosProcMtrClt()
{
}


bool 
AosProcMtrClt::config(const AosXmlTagPtr &config)
{
	mAppName = config->getAttrStr(AOSTAG_APPNAME);
	mAddr = config->getAttrStr(AOSTAG_ADDR);
	mPort = config->getAttrInt(AOSTAG_PORT, -1);
	aos_assert_r(mPort > 0, false);
	mNumPorts = config->getAttrInt(AOSTAG_NUM_PORTS, 1);
	OmnString lenind = config->getAttrStr(AOSTAG_LENGTH_IND);
	if (lenind != "")
	{
		mLenInd = AosConvertTcpLenType(lenind, eAosTLT_FirstFourHigh);
	}
	return true;
}


bool
AosProcMtrClt::start()
{
	mConn = OmnNew OmnTcpClient("", mAddr, mPort, mNumPorts, mLenInd);
	aos_assert_r(mConn, false);
	
	sendInitHeartbeat();
	return true;
}


bool
AosProcMtrClt::connect()
{
	OmnString errmsg;
	if (mConn && mConn->isConnGood()) return true;
	if (!mConn)
	{
		mConn = OmnNew OmnTcpClient("", mAddr, mPort, mNumPorts, mLenInd);
		aos_assert_r(mConn, false);
		mConn->connect(errmsg);
	}

	if (mConn->isConnGood()) return true;

	// Re-create the connection
	mConn = OmnNew OmnTcpClient("", mAddr, mPort, mNumPorts, mLenInd);
	aos_assert_r(mConn, false);
	mConn->connect(errmsg);
	if (mConn->isConnGood()) return true;
	mConn = 0;
	return false;
}


bool
AosProcMtrClt::sendInitHeartbeat()
{
	OmnString errmsg;
	while (!mConn->connect(errmsg))
	{
		OmnSleep(mRetryFreq);
	}

	OmnString msg = "<heartbeat";
	msg << AOSTAG_TYPE << "=\"" << AOSVALUE_HEARTBEAT << "\" "
		<< AOSTAG_PROCID << "=\"" << mProcid << "\""
		<< AOSTAG_APPNAME << "=\"" << mAppName << "\""
		<< "/>";

	mConn->smartSend(msg.data(), msg.length());
	return true;
}


bool
AosProcMtrClt::threadFunc(
		OmnThrdStatus::E &state,
		const OmnThreadPtr &thread)
{
	// It reads the connection. When the connection is broken,
	// it will keep on trying. Otherwise, when it receives something,
	// it will process it.
	OmnConnBuffPtr buff;
	while (state == OmnThrdStatus::eActive)
	{
		if (!mConn || !mConn->isConnGood())
		{
			connect();
			OmnSleep(1);
			continue;
		}

		if (mConn)
		{
			mConn->smartRead(buff);
			AosXmlParser parser;
			AosXmlTagPtr req = parser.parse(buff, "");
			procReq(req);
		}
	}

	return true;
}


bool
AosProcMtrClt::procReq(const AosXmlTagPtr &req)
{
	AosReqType::E type = AosReqType::toEnum(req->getAttrStr(AOSTAG_REQTYPE));
	switch (type)
	{
	case AosReqType::eHeartbeatResp:
		 return true;

	case AosReqType::eHeartbeatReq:
		 procHeartbeat(req);
		 return true;

	default:
		 OmnAlarm << "Unrecognized request type: " << req->getAttrStr(AOSTAG_REQTYPE)
			 << enderr;
		 return false;
	}

	OmnShouldNeverComeHere;
	return false;
}


bool
AosProcMtrClt::procHeartbeat(const AosXmlTagPtr &req)
{
	// Currently we just send a response back. What we should do is
	// to periodically check the heartbeat. If one of the heartbeats
	// fails, it should report failing.
	OmnString resp = "<resp ";
	resp << AOSTAG_TRANSID << "=\"" << req->getAttrStr(AOSTAG_TRANSID)
		<< "\" " << AOSTAG_STATUS << "=\"200"
		<< "\" " << AOSTAG_PROCID << "=\"" << mProcid 
		<< "/>";
	mConn->smartSend(resp);
	return true;
}

