////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: Msg.cpp
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#include "Message/Msg.h"

#include "Alarm/Alarm.h"
#include "Debug/Debug.h"
#include "Logger/LoggerMsg.h"
#include "Logger/LogListener.h"
#include "Thread/Mutex.h"
#include "TransMgr/Trans.h"
#include "Util/Ptrs.h"
#include "Util/SerialTo.h"
#include "Util/SerialFrom.h"


static uint				sgMsgSeqno = 0;
static OmnMutex			sgLock;
// static OmnMsgLoggerPtr	sgMsgLogger;

OmnMsg::OmnMsg()
:
mTransId(0),
mModuleId(OmnModuleId::eInvalid),
mSenderPort(0),
mRecverPort(0),
mSenderId(0),
mFlag(0)
{
	sgLock.lock();
	mSeqno = sgMsgSeqno++;
	sgLock.unlock();

	/*
	mTimestamps[0] = 0;
	mTimestamps[1] = 0;
	mTimestamps[2] = 0;
	mTimestamps[3] = 0;
	mTimestamps[4] = 0;
	mTimestamps[5] = 0;
	mTimestamps[6] = 0;
	mTimestamps[7] = 0;
	mTimestamps[8] = 0;
	mTimestampName[0] = 'x';
	mTimestampName[1] = 'x';
	mTimestampName[2] = 'x';
	mTimestampName[3] = 'x';
	mTimestampName[4] = 'x';
	mTimestampName[5] = 'x';
	mTimestampName[6] = 'x';
	mTimestampName[7] = 'x';
	mTimestampName[8] = 'x';
	*/
}


OmnMsg::OmnMsg(const OmnIpAddr &recvAddr, const int recvPort)
:
mTransId(0),
mModuleId(OmnModuleId::eInvalid),
mSenderPort(0),
mRecverAddr(recvAddr),
mRecverPort(recvPort),
mSenderId(0),
mFlag(0)
{
	sgLock.lock();
	mSeqno = sgMsgSeqno++;
	sgLock.unlock();

}


OmnMsg::OmnMsg(const OmnIpAddr &senderIpAddr,
			   const int senderPort,
			   const OmnIpAddr &receiverIpAddr,
			   const int receiverPort)
:
mTransId(0),
mModuleId(OmnModuleId::eInvalid),
mSenderAddr(senderIpAddr),
mSenderPort(senderPort),
mRecverAddr(receiverIpAddr),
mRecverPort(receiverPort),
mSenderId(0),
mFlag(0)
{
	sgLock.lock();
	mSeqno = sgMsgSeqno++;
	sgLock.unlock();

}


OmnMsg::~OmnMsg()
{
}


void
OmnMsg::reset()
{
	mTransId = 0;
	mModuleId = OmnModuleId::eInvalid;
	mSenderAddr = OmnIpAddr::eInvalidIpAddr;
	mSenderPort = 0;
	mRecverAddr = OmnIpAddr::eInvalidIpAddr;
	mRecverPort = 0;
	mSenderId = 0;
	mRecverDn = "";
	mFlag = 0;
}


OmnRslt			
OmnMsg::serializeFrom(OmnSerialFrom &s)
{
	try
	{
		s >> mTransId
		  >> mModuleId
		  >> mRecverDn
		  >> mSenderId
		  >> mFlag;

		OmnPopMsgSendReason;
	}

	catch (const OmnExcept &e)
	{
		OmnAlarm << "Failed to serialize from: " << e.toString() << enderr;
		return e.getRslt();
	}

    OmnConnBuffPtr buff = s.getDataBuff();
	mSenderAddr = buff->getRemoteAddr();
	mSenderPort = buff->getRemotePort();
	mRecverAddr = buff->getLocalAddr();
	mRecverPort = buff->getLocalPort();
	
	return true;
}

OmnRslt			
OmnMsg::serializeTo(OmnSerialTo &s) const
{
	try
	{
		s << mTransId;
		s << mModuleId;
		s << mRecverDn;
		s << mSenderId;
		s << mFlag;

		OmnPushMsgSendReason;
	}

	catch (const OmnExcept &e)
	{
		OmnAlarm << "Failed to serialize from: " << e.toString() << enderr;
		return e.getRslt();
	}

	return true;
}


OmnString 
OmnMsg::toString() const
{
	OmnString str;
	str << "\n----------------"
		<< "\n    Seqno:    " << mSeqno
		<< "\n    TransId:  " << mTransId
		<< "\n    ModuleId: " << mModuleId
		<< "\n    Sender:   " << mSenderAddr.toString() << ":" << mSenderPort
		<< "\n    Recver:   " << mRecverAddr.toString() << ":" << mRecverPort
		<< "\n    RecverDn: " << mRecverDn 
		<< "\n    SenderId: " << mSenderId
		<< "\n    Flag:     " << mFlag
		<< "\n================"
		<< OmnDisplayMsgSendReason;
	return str;
}


void
OmnMsg::clone(const OmnMsgPtr &msg) const
{
	msg->mTransId    = mTransId;
	msg->mModuleId   = mModuleId;
	msg->mSenderAddr = mSenderAddr;
	msg->mSenderPort = mSenderPort;
	msg->mRecverAddr = mRecverAddr;
	msg->mRecverPort = mRecverPort;
	msg->mRecverDn   = mRecverDn;
	msg->mSenderId   = mSenderId;
	msg->mFlag       = mFlag;
}


void		
OmnMsg::setTrans(const OmnTransPtr &trans)
{
	mTrans = trans;
}


OmnTransPtr	
OmnMsg::getTrans() const
{
	return mTrans;
}

