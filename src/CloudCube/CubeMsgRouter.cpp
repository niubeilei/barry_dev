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
// There is one and only one instance on each physical machine. 
// This class is responsible for sending and receiving messages 
// for cubes.
//
// Modification History:
// 2013/03/02 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "CloudCube/CubeMessager.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "API/AosApi.h"
#include "CloudCube/CubeMgr.h"
#include "CloudCube/CubeMsgProc.h"
#include "CloudCube/CubeCaller.h"
#include "TransServer/TransModuleSvr.h"
#include "TransUtil/TransDistributor.h"
#include "TransBasic/Trans.h"
#include "TransUtil/XmlTrans.h"
#include "TransUtil/TinyTrans.h"
#include "TransUtil/BuffTrans.h"
#include "TransUtil/ServerFix.h"


AosCubeMessager::AosCubeMessager()
{
	smPhysicalId = AosGetSelfServerId();
	if (smPhysicalId < 0 || (u32)smPhysicalId >= AosGetNumPhysicals())
	{
		OmnThrowException("Invalid physical id");
		return;
	}
}


AosCubeMessager::~AosCubeMessager()
{
}


bool
AosCubeMessager::start()
{
	// Create the trans server
	AosTransProcPtr thisptr(this, false);
	mTransServer = OmnNew AosTransModuleSvr(AosModuleId::eCubeMsgRouter, thisptr);

	// Create the trans client
	AosTransDistributorPtr distributor = OmnNew AosServerFix(AosGetNumPhysicals());
	mTransClient = OmnNew AosTransModuleClt(distributor, AosModuleId::eCubeMsgRouter, 0);
	aos_assert_r(mTransClient, false);

	return true;
}


bool
AosCubeMessager::config(const AosXmlTagPtr &config)
{
	return true;
}


bool
AosCubeMessager::stop()
{
    return true;
}


bool 
AosCubeMessager::proc(
		const AosXmlTransPtr &trans,
		const AosXmlTagPtr &trans_doc,
		const AosRundataPtr &rdata)
{
	// A message is received. It adds the message to the message queue.
	mLock->lock();
	mTransQueue.push(trans.getPtr());
	mCondVar->signal();
	mLock->unlock();
	return true;
}


bool 
AosCubeMessager::proc(
		const AosTinyTransPtr &trans, 
		const AosRundataPtr &rdata)
{
	mLock->lock();
	mTransQueue.push(trans.getPtr());
	mCondVar->signal();
	mLock->unlock();
	return true;
}


bool 
AosCubeMessager::proc(
		const AosBuffTransPtr &trans, 
		const AosRundataPtr &rdata)
{
	mLock->lock();
	mTransQueue.push(trans.getPtr());
	mCondVar->signal();
	mLock->unlock();
	return true;
}


bool
AosCubeMessager::threadFunc(OmnThrdStatus::E &state, const OmnThreadPtr &thread)
{
    while (state == OmnThrdStatus::eActive)
    {
		mLock->lock();
		if (mTransQueue.size() == 0)
		{
			mCondVar->wait(mLock);
			mLock->unlock();
			continue;
		}

		AosTrans1Ptr trans = mTransQueue.front();
		mTransQueue.pop();
		mLock->unlock();

		procOneTrans(trans);
	}

	OmnScreen << "Leaving CubeMsgRouter thread" << endl;
	return true;
}


bool
AosCubeMessager::procOneTrans(const AosTrans1Ptr &trans)
{
	AosTrans1::TransType type = trans->getType();
	switch (type)
	{
	case AosTrans1::eXmlTrans:
		 procXmlTrans((AosXmlTrans*)trans.getPtr());
		 break;

	case AosTrans1::eBuffTrans:
		 procBuffTrans((AosBuffTrans*)trans.getPtr());
		 break;

	case AosTrans1::eIILTrans:
		 procTinyTrans((AosTinyTrans*)trans.getPtr());
		 break;

	default:
		 OmnAlarm << "Unrecognized message type: " << type << enderr;
		 return false;
	}

	OmnShouldNeverComeHere;
	return false;
}


bool
AosCubeMessager::procBuffTrans(const AosBuffTransPtr &trans)
{
	AosBuffPtr buff = trans->getBuff();
	aos_assert_r(buff, false);

	buff->reset();
	AosBuffMsgPtr msg = AosBuffMsg::createMsg(buff);
	if (msg)
	{
		procBuffMsg(msg);
	}

	OmnNotImplementedYet;
	return false;
}


bool
AosCubeMessage::procBuffMsg(const AosBuffMsgPtr &msg)
{
	AosRundataPtr rdata = msg->getRundata();
	aos_assert_r(rdata, false);
	bool is_request = msg->isRequest();

	if (is_request)
	{
		// It is a request.
		AosMsgId::E msgid = msg->getMsgId();
		aos_assert_rr(AosMsgId::isValid(msgid), rdata, false);
		mLock->lock();
		AosCubeMsgProcPtr proc = mMsgProcs[msgid];
		if (!proc)
		{
			mLock->unlock();
			OmnString errmsg = "Requester not found: ";
			errmsg << msgtype;
			logError(rdata, errmsg, AosFileAndLine);
			return false;
		}

		mLock->unlock();
		return proc->procMsg(rdata, msg);
	}

	// It is a response.
	cmapitr_t itr = mCallers.find(trans->getTransId());
	if (itr == mCallers.end())
	{
		// Did not find. 
		OmnString errmsg = "Response with no proc: ";
		errmsg << trans->getTransId();
		logError(rdata, errmsg, AosFileAndLine);
		return false;
	}

	AosCubeCallerPtr proc = itr->second;
	mCallers.erase(itr);
	mLock->unlock();
	proc->procResp(rdata, trans.getPtr(), xmldoc);
	return true;
}


bool
AosCubeMessager::procTinyTrans(const AosTinyTransPtr &trans)
{
	OmnNotImplementedYet;
	return false;
}


bool
AosCubeMessager::procXmlTrans(const AosXmlTransPtr &trans)
{
	AosXmlTagPtr xmldoc = trans->getXmlData();
	if (!xmldoc)
	{
		OmnAlarm << "internal error" << enderr;
		return false;
	}

	bool is_request = xmldoc->getAttrBool(AOSTAG_REQUEST_FLAG, false);
	AosRundataPtr rdata = AosRundata::serializeFromMsg(xmldoc AosMemoryCheckerArgs);
	aos_assert_r(rdata, false);

	if (is_request)
	{
		// It is a request.
		OmnString msgtype = xmldoc->getAttrStr(AOSTAG_MESSAGE_TYPE);
		mLock->lock();
		pmapitr_t itr = mMsgProcs.find(msgtype);
		if (itr == mMsgProcs.end())
		{
			mLock->unlock();
			OmnString errmsg = "Requester not found: ";
			errmsg << msgtype;
			logError(rdata, errmsg, AosFileAndLine);
			return false;
		}

		AosCubeMsgProcPtr proc = itr->second;
		return proc->procMsg(rdata, trans, xmldoc);
	}

	// It is a response.
	cmapitr_t itr = mCallers.find(trans->getTransId());
	if (itr == mCallers.end())
	{
		// Did not find. 
		OmnString errmsg = "Response with no proc: ";
		errmsg << trans->getTransId();
		logError(rdata, errmsg, AosFileAndLine);
		return false;
	}

	AosCubeCallerPtr proc = itr->second;
	mCallers.erase(itr);
	mLock->unlock();
	proc->procResp(rdata, trans.getPtr(), xmldoc);
	return true;
}


bool
AosCubeMessager::addCaller(
		const AosRundataPtr &rdata,
		const AosTrans1Ptr &trans, 
		const AosCubeCallerPtr &caller)
{
	u64 transid = trans->getTransId();
	mLock->lock();
	cmapitr_t itr = mCallers.find(transid);
	if (itr != mCallers.end())
	{
		mLock->unlock();
		OmnString errmsg = "Transaction already registered: ";
		errmsg << transid;
		logError(rdata, errmsg, AosFileAndLine);
		return false;
	}

	mCallers[transid] = caller;
	mLock->unlock();
	return true;
}


bool
AosCubeMessager::registerMsgProc(const AosCubeMsgProcPtr &proc)
{
	aos_assert_r(proc, false);
	OmnMsgId::E msgid = proc->getMsgId();
	aos_assert_r(OmnMsgId::isValid(msgid), false);
	mLock->lock();
	if (mMsgProcs[msgid])
	{
		OmnAlarm << "Message already registered: " << msgid << enderr;
		mLock->unlock();
		return false;
	}

	mMsgProcs[msgid] = proc;
	mLock->unlock();
	return true;
}


AosTrans1Ptr
AosCubeMessager::sendMsg(
		const AosRundataPtr &rdata,
		const u32 recv_cubeid,
		const AosBuffPtr &buff,
		const AosCubeCallerPtr &caller)
{
	aos_assert_rr(mTransClient, rdata, 0);

	int physical_id = AosGetCubeMgr()->cubeId2Physicalid(recv_cubeid);
	aos_assert_rr(AosIsValidPhysicalIdNorm(physical_id), rdata, 0);

	AosTrans1Ptr trans;
	trans = mTransClient->addTrans(rdata, buff, false, physical_id);

	if (caller)
	{
		addCaller(rdata, trans, caller);
	}
	return trans;
}


bool
AosCubeMessager::logError(
		const AosRundataPtr &rdata, 
		const OmnString &errmsg, 
		const char *filename, 
		const int line)
{
	OmnNotImplementedYet;
	return false;
}


