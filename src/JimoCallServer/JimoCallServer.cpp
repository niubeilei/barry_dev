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
// 2014/11/30 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "JimoCallServer/JimoCallServer.h"

#include "AppMgr/App.h"
#include "CubeComm/CubeCommTcpSvr.h"
#include "CubeComm/CubeCommUdp.h"
#include "Porting/Sleep.h"
#include "JimoAPI/JimoPackage.h"
#include "JimoCall/JimoCall.h"
#include "JimoCall/NullCallPackage.h"
#include "SEInterfaces/JimoCallPackage.h"
#include "Rundata/Rundata.h"
#include "UtilComm/ConnBuff.h"
//#include "JimoRaft/RaftMsg.h"			//for debugging heartbeat problem for raft, by White, 2015-09-16 09:37:47
//#include "JimoRaft/RaftMsgAppendEntryReq.h"	//for debugging heartbeat problem for raft, by White, 2015-09-16 09:37:47

static AosJimoCallServer* sgJimoCallServer = NULL;

AosJimoCallServer::AosJimoCallServer(AosRundata *rdata)
:
mLock(OmnNew OmnMutex()),
mLockRaw(mLock.getPtr()),
mCondVar(OmnNew OmnCondVar()),
mCondVarRaw(mCondVar.getPtr()),
mMaxQueueSize(eDftMaxQueueSize)
{
	bool rslt = config(rdata);
	if (!rslt)
	{
		OmnThrowException(rdata->getErrmsg());
	}
}


AosJimoCallServer::~AosJimoCallServer()
{
}


bool
AosJimoCallServer::config(AosRundata *rdata)
{
	AosXmlTagPtr conf = OmnApp::getAppConfig();
	aos_assert_r(conf, false);

	AosXmlTagPtr tag = conf->getFirstChild("jimocall_server");
	aos_assert_r(tag, false);

	bool rslt = mEndPointInfo.init(rdata, tag);
	aos_assert_r(rslt, false);

	OmnCommListenerPtr caller(this, false);
	mConnType = tag->getAttrStr("conn_type", "udp");
	if (mConnType == "udp")
	{
		mCubeComm = OmnNew AosCubeCommUdp(mEndPointInfo, caller);
	}
	else if (mConnType == "tcp")
	{
		mCubeComm = OmnNew AosCubeCommTcpSvr(mEndPointInfo);
	}
	else
	{
		OmnAlarm << "type error" << enderr;
		return false;
	}

	mCubeCommRaw = mCubeComm.getPtr();
	OmnCommListenerPtr thisptr(this, false);
	rslt = mCubeCommRaw->startReading(rdata, thisptr);
	aos_assert_r(rslt, false);

	//thread
	for (int i=0; i<10; i++)
	{
		OmnThreadedObjPtr thisPtr(this, false);
		OmnThreadPtr thread = OmnNew OmnThread(thisPtr, 
				"JimoCallServerThread", i, true, true, __FILE__, __LINE__);
		mThreads.push_back(thread);
		thread->start();
	}
	return true;
}

bool
AosJimoCallServer::setSelf(AosJimoCallServer *server)
{
	aos_assert_r(server, false);
	if (sgJimoCallServer)
		delete sgJimoCallServer;

	sgJimoCallServer = server;
	return true;
}

AosJimoCallServer *
AosJimoCallServer::getSelf()
{
	return sgJimoCallServer;
}

bool		
AosJimoCallServer::msgRead(const OmnConnBuffPtr &connbuff)
{
	AosJimoCallPtr jimo_call = OmnNew AosJimoCall(connbuff, this);
	// AosBuffPtr buff = OmnNew AosBuff(connbuff);

	// jimo_call.setJimoCallServer(this);
	// jimo_call.setConnBuff(connbuff);
	// jimo_call.setBSON(buff);
	// jimo_call.setFromEPInfo(connbuff->getRemoteAddr(), connbuff->getRemotePort());
	while (1)
	{
		mLockRaw->lock();
		if (mQueue.size() >= mMaxQueueSize)
		{
			mLockRaw->unlock();
			OmnSleep(1);
			continue;
		}
		mQueue.push(jimo_call);
		//for debugging heartbeat problem for raft, by White, 2015-09-16 09:37:47
//		AosBuffPtr buff = jimo_call->getBSON().getBuff();
//		OmnString s = "JimoCallServer've got message at timestamp:";
//		s << OmnGetTimestamp();
//		buff->dumpHex(s);
//		i64 iOriginIdx = buff->getCrtIdx();
//		i64 i = 28;
//		buff->setCrtIdx(i);
//		AosRaftMsg::RaftMsgType msgType = (AosRaftMsg::RaftMsgType)(buff->getU32(0));
//		if (AosRaftMsg::eMsgHeaderbeat == msgType)
//		{
//			buff->setCrtIdx(i);
//			AosRaftMsg *msg;
//			msg = new AosRaftMsgAppendEntryReq();
//			msg->serializeFrom(NULL, buff.getPtrNoLock());
//			OmnScreen << "Heartbeat pushed to queue at timestamp:" << OmnGetTimestamp() << " message:"
//					<< msg->toString() << endl;
//		}
//		buff->setCrtIdx(iOriginIdx);
		//buff->reset();
		mCondVar->signal();
		mLockRaw->unlock();
		return true;
	}

	OmnShouldNeverComeHere;
	return false;
}


bool
AosJimoCallServer::threadFunc(OmnThrdStatus::E &state, const OmnThreadPtr &thread)
{
	AosJimoCallPtr jimo_call;
    while (state == OmnThrdStatus::eActive)
    {
    	mLockRaw->lock();
		if (mQueue.size() <= 0)
		{
			mCondVarRaw->wait(*mLockRaw);
			mLockRaw->unlock();
			continue;
		}
		jimo_call = mQueue.front();
		mQueue.pop();
		mLockRaw->unlock();
		proc(jimo_call);
	}

	return true;
}


bool
// AosJimoCallServer::proc(const OmnConnBuffPtr &connbuff)
AosJimoCallServer::proc(const AosJimoCallPtr &jimo_call)
{
	/*
	AosBuffPtr buff = OmnNew AosBuff(connbuff);

	AosJimoCall jimo_call;
	jimo_call.setJimoCallServer(this);
	jimo_call.setConnBuff(connbuff);
	jimo_call.setBSON(buff);
	jimo_call.setFromEPInfo(connbuff->getRemoteAddr(), connbuff->getRemotePort());
	*/

	OmnConnBuffPtr connbuff = jimo_call->getConnBuff();
	AosBuffPtr buff = jimo_call->getBSON().getBuff();

	AosRundataPtr rdataptr = AosRundata::serializeFromBuffForJimoCall(buff.getPtr());
	aos_assert_r(rdataptr, false);
	AosRundata *rdata = rdataptr.getPtrNoLock();

	if (mConnType == "tcp")
	{
		OmnTcpClientPtr conn = connbuff->getConn();
		aos_assert_r(conn, false);
		AosWebRequestPtr webReq = OmnNew AosWebRequest(conn, connbuff);
		rdata->setRequest(webReq);
	}

	bool rslt = jimo_call->msgRead(rdata, buff.getPtr());
	aos_assert_r(rslt, false);

	if (jimo_call->getMsgID() != OmnMsgId::eJimoCallReq)
	{
		AosLogError(rdata, false, AosErrmsgId::eInvalidMessage)
			<< AosFN::eValue1 << jimo_call->getMsgID()
			<< AosFN::eValue2 << jimo_call->getJimoCallID() << enderr;
		return false;
	}

	AosJimoCallPackage * package = getJimoCallPackage(rdata, jimo_call);
	aos_assert_r(package, false);
	rslt = package->jimoCall(rdata, *(jimo_call.getPtr()));
	aos_assert_r(rslt, false);

	return true;
}


OmnString	
AosJimoCallServer::getCommListenerName() const
{
	return "JimoCallServer";
}


void 		
AosJimoCallServer::readingFailed()
{
}


AosJimoCallPackage *
AosJimoCallServer::getJimoCallPackage(
		AosRundata *rdata,
		const AosJimoCallPtr &jimo_call)
{
	// 1. Get the package ID from 'buff';
	OmnString package_name = jimo_call->getPackageID();
	if ( package_name == "" ) 
	{
		sendJimoCallFailed(rdata, AosErrmsgId::eMissingPackageID);
		return 0;
	}

	mLockRaw->lock();
	// Check whether it is in the map.
	itr_t itr = mPackages.find(package_name);
	if (itr != mPackages.end())
	{
		// It is in the map. 
		AosJimoCallPackage *package = itr->second.getPtrNoLock();
		mLockRaw->unlock();

		// Check whether it is a "Null Package". If it is, it means
		// this package ID is an invalid package ID. 
		if (!package->isValid()) 
		{
			// It is an invalid package ID. 
			sendJimoCallFailed(rdata, AosErrmsgId::eJimoCallPackageNotFound);
			return 0;
		}

		// It is a valid package. Return it.
		return package;
	}

	// Did not find it in the map. Check whether we can create it.
	// All packages are named jimos. The name is created by the
	// following function. It uses the name (jimo name) to create
	// the jimo. If it can create the jimo, it is a valid package. 
	// Otherwise, it is an invalid package. 
	mLockRaw->unlock();

	AosJimoCallPackagePtr ptr;
	AosJimoPtr jimo = AosCreateJimoByClassname(rdata, package_name, 1);
	if (jimo)
	{
		ptr = dynamic_cast<AosJimoCallPackage *>(jimo.getPtr());
	}

	if (!ptr)
	{
		sendJimoCallFailed(rdata, AosErrmsgId::eInternalError);
		ptr = OmnNew AosNullJimoCallPackage(1);
	}
			
	mLockRaw->lock();
	itr = mPackages.find(package_name);
	if (itr == mPackages.end())
	{
		mPackages[package_name] = ptr;
	}
	else
	{
		ptr = itr->second;
	}

	mLockRaw->unlock();
	return ptr.getPtrNoLock();
}


OmnString 
AosJimoCallServer::composeJimoCallPackageName(const int package_id)
{
	// JimoCall Packages are identified by Package IDs, which are
	// small integers, which are managed by a container. 
	OmnString name = "_zky_jimocall_";
	name << package_id;
	return name;
}


bool	
AosJimoCallServer::signal(const int threadLogicId)
{
	return true;
}


bool
AosJimoCallServer::sendJimoCallFailed(
		AosRundata *rdata, 
		const AosErrmsgId::E errid)
{
	OmnNotImplementedYet;
	return false;
}


bool
AosJimoCallServer::JimoCallBack(
		AosRundata *rdata, 
		AosJimoCall &jimo_call,
		AosBuff *buff)
{
	aos_assert_r(mCubeCommRaw, false);

	// u32         mEpId;
	// OmnString   mEpNam
	// OmnIpAddr   mIpAdd
	// int         mPort;
	// AosEndPointInfo epinfo = mCubeCommRaw.getEPInfo();
	// jimo_call.setFromEPInfo(epinfo.mIpAddr, epinfo.mPort, epinfo.mEpId);

	AosEndPointInfo from_epinfo = jimo_call.getFromEPInfo();
	bool rslt = mCubeCommRaw->sendTo(rdata, from_epinfo, buff);
	aos_assert_r(rslt, false);

	return true;
}

