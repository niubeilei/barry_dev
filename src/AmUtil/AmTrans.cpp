////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2007
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// Description:
//
// Modification History:
// 4/5/2007: Created by cding
////////////////////////////////////////////////////////////////////////////

#include "AmUtil/AmTrans.h"
#include "AmUtil/AmTagId.h"
#include "AmUtil/AmExternalSvr.h"
#include "UtilComm/TcpClient.h"
#include "UtilComm/ConnBuff.h"


AosAmTrans::AosAmTrans(const OmnTcpClientPtr conn, const OmnConnBuffPtr buff)
:
mConn(conn)//,
//mBuff(buff)
{
	// 
	// Need to determine whether the buff is a request or a response. 
	// 
	AosAmMsgPtr msg = OmnNew AosAmMsg(*buff);
	mMsgQueue.append(msg);
}


AosAmTrans::AosAmTrans(const AosAmTransPtr &trans)
{
	mConn = trans->getConn();
	AosAmMsgPtr msg = OmnNew AosAmMsg(*trans->getMsg()->getBuffer());
	mMsgQueue.append(msg);
	mExternalSvr = trans->getExternalSvr();
}


AosAmTrans::~AosAmTrans()
{
}

// get the trans connection pointer
OmnTcpClientPtr AosAmTrans::getConn() const
{
	return mConn;
}


bool 
AosAmTrans::setErrMsg(const OmnString &value)
{
	AosAmMsgPtr msg;
	mMsgQueue.reset();
	for(unsigned int i = 0; i < mMsgQueue.entries(); i++)
	{
		msg = mMsgQueue.crtValue();
		if(msg->isResponse())
		{
			msg->addTag(AosAmTagId::eErrmsg, value);
		}
		mMsgQueue.next();
	}

	return true;
}


OmnRslt 
AosAmTrans::sendOut()
{
	int i, nSize = mMsgQueue.entries();
	OmnRslt rslt;
	AosAmMsgPtr msg;

	mMsgQueue.reset();
	for(i = 0; i < nSize; i++)
	{
		msg = mMsgQueue.crtValue();
		if(msg->isResponse())
		{
			rslt = mConn->writeToSock(msg->getData(), msg->getDataLength());
			if(!rslt)
			{
				break;
			}
		}
		mMsgQueue.next();
	}
	return rslt;
}


bool 
AosAmTrans::setMsg(const AosAmMsgPtr &msg) 
{
OmnAlarm << "add record!!!!!!!  " << mMsgQueue.entries() << enderr;
	mMsgQueue.append(msg);
	return true; 
}


bool 
AosAmTrans::hasResponse()
{
	AosAmMsgPtr msg;
	int i, nSize = mMsgQueue.entries();

	mMsgQueue.reset();
	for(i = 0; i < nSize; i++)
	{
		msg = mMsgQueue.crtValue();
		if(msg->isResponse())
		{
			return true;
		}
		mMsgQueue.next();
	}
	return false;
}

AosAmExternalSvrPtr 
AosAmTrans::getExternalSvr()
{
	return mExternalSvr; 
}


bool 
AosAmTrans::setExternalSvr(const AosAmExternalSvrPtr & server)
{
	mExternalSvr = server;
	return true;
}
