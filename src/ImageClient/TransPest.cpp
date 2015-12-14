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
// 	Created: 2010/11/10  by ice
////////////////////////////////////////////////////////////////////////////
#include "TransPest/TransPest.h"

#include "UtilComm/TcpCommClt.h"
#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "Debug/Debug.h"
#include "Porting/Sleep.h"
#include "Util/File.h"
#include "Thread/Mutex.h"
#include "Thread/ThreadMgr.h"
#include "Thread/Thread.h"
#include "Util/OmnNew.h"
#include "sys/stat.h"
#include "XmlUtil/SeXmlParser.h"
#include "XmlUtil/XmlTag.h"

OmnMutex 	AosTransPest::sLock; 
u32 		AosTransPest::sCrtTransId = OmnGetSecond(); 


AosTransPest::AosTransPest()
:
mLock(OmnNew OmnMutex()),
mCondVar(OmnNew OmnCondVar),
mFileSize(0),
mCrtSeqno(0)
{
}



AosTransPest::~AosTransPest()
{
OmnScreen << "-----------------To stop reading ..." << endl;
	OmnCommListenerPtr thisPtr(this, false);
	mConn->stopReading(thisPtr);
	mConn->closeConn();
	mConn->destroyConn();
	mConn = 0;

	OmnThreadMgr::getSelf()->removeThread(mSendThread);
	OmnThreadMgr::getSelf()->removeThread(mCheckThread);
	OmnThreadMgr::getSelf()->removeThread(mHeartbeatThread);
}


bool
AosTransPest::init(const AosXmlTagPtr &config)
{
	mFileName = config->getAttrStr(AOSCONFIG_DOC_FILENAME,"");
	aos_assert_r(mFileName != "",false);
	mDirName = config->getAttrStr(AOSCONFIG_DIRNAME, "");

	mRemoteAddr = config->getAttrStr(AOSCONFIG_REMOTE_ADDR);
	aos_assert_r(mRemoteAddr != "",false);
	mRemotePort = config->getAttrInt(AOSCONFIG_REMOTE_PORT, -1);
	aos_assert_r(mRemotePort != -1,false);
	connect();

	OmnThreadedObjPtr thisPtr(this, false);
	mHeartbeatThread = OmnNew OmnThread(thisPtr, "HeartBeatThrd", eHeartbeatThrdId, true, true, __FILE__, __LINE__);
	mSendThread = OmnNew OmnThread(thisPtr, "SendThrd", eSendThrdId, true, true, __FILE__, __LINE__);
	mCheckThread = OmnNew OmnThread(thisPtr, "CheckThrd", eCheckThrdId, true, true, __FILE__, __LINE__);
	mHeartbeatThread->start();
	mSendThread->start();
	mCheckThread->start();

	return true;
}


bool    
AosTransPest::signal(const int threadLogicId)
{
	return true;
}


bool    
AosTransPest::checkThread(OmnString &err, const int thrdLogicId) const
{
	return true;
}


bool
AosTransPest::threadFunc(
		OmnThrdStatus::E &state,
		const OmnThreadPtr &thread)
{
	if (thread->getLogicId() == eHeartbeatThrdId)
	{
		return heartbeatThrdFunc(state, thread);
	}

	if (thread->getLogicId() == eSendThrdId)
	{
		return sendThrdFunc(state, thread);
	}
	if (thread->getLogicId() == eCheckThrdId)
	{
		return checkThrdFunc(state, thread);
	}

	OmnAlarm << "Invalid thread logic id: " << thread->getLogicId() << enderr;
	return false;
}


bool
AosTransPest::heartbeatThrdFunc(
		OmnThrdStatus::E &state,
		const OmnThreadPtr &thread)
{
	while (state == OmnThrdStatus::eActive)
	{
		sendHeartbeat();
		OmnSleep(eHearbeatThrdFreq);
	}

	return true;
}


bool
AosTransPest::sendThrdFunc(
		OmnThrdStatus::E &state,
		const OmnThreadPtr &thread)
{
	//Send a trans form the sendQueue,
	//the sending trans must put in the procQueue.
	Trans * trans = 0;
	while (state == OmnThrdStatus::eActive) 
	{
		mLock->lock();
		if (mSendQueue.entries() == 0)
		{
			// There is no more requests.
			mCondVar->wait(mLock);
			mLock->unlock();
			continue;
		}
		trans = mSendQueue.pop();
		mLock->unlock();

		bool rslt = sendRequestPriv(trans);
		if (!rslt)
		{
			OmnAlarm << "Failed the processing!" << enderr;
		}
	}
	
    return true;	
}


bool
AosTransPest::checkThrdFunc(
		OmnThrdStatus::E &state,
		const OmnThreadPtr &thread)
{
	//we check the procQueue's trans once  by 30s
	//At the beginning the trans's sending 0,
	//after 30s ,sending will add 1
	//when we detecting the sending > 2 
	//it means the trans is too old,it is timout
	//we need resend the trans
	while (state == OmnThrdStatus::eActive)
	{
		mLock->lock();
		map<u64, Trans*>::iterator itr = mProcQueue.begin();
		while (itr != mProcQueue.end())
		{
			if (itr->second->sending <= 2)
			{
				itr->second->sending++;
				continue;
			}

			// The transaction timed out. Put it into the sending queue.
			Trans* tr = itr->second;	
			mProcQueue.erase(itr);

			mSendQueue.push(tr);
			mCondVar->signal();
			itr++;
		}
		mLock->unlock();
		OmnSleep(eCheckThrdFreq);
	}
	return true;
}


bool
AosTransPest::sendRequestPriv(Trans *trans)
{
	// When the server wants to send a trans , it will send a transid 
	// first. It then sends the itself. 
	OmnString trRequest;
	trRequest << trans->tid << "\n"
			  << trans->trans;
	OmnRslt rest =  mConn->sendTo(trRequest.data(), trRequest.length());
	if (!rest)
	{
		reconnect();
		mLock->lock();
		mSendQueue.push(trans);
		mLock->unlock();
		return false;
	}

		
	
/*	
	// 1. send trans id
	OmnString transid;
	transid << trans->tid;
	OmnRslt rest =  mConn->sendTo(transid.data(), transid.length());
	if (!rest)
	{
		reconnect();
		mLock->lock();
		mSendQueue.push(trans);
		mLock->unlock();
		return false;
	}

	// 2. send trans contents
	rest = mConn->sendTo(trans->trans.data(),trans->trans.length());
	if (!rest)
	{
		reconnect();
		mLock->lock();
		mSendQueue.push(trans);
		mLock->unlock();
		return false;
	}
*/	
	// The request was sent to the remote machine. Add this transaction
	// into mProcQueue
	mLock->lock();
	mProcQueue.insert(make_pair(trans->tid, trans));
	trans->sending = 1;
	mLock->unlock();
	return true;	
}


void
AosTransPest::connect()
{
	OmnString errmsg;
 	bool rslt;
	do
	{
		if (errmsg !="")
		{
			OmnAlarm << errmsg << enderr;
			OmnSleep(eReconnectFreq);
		}
		mConn = OmnNew OmnTcpCommClt(mRemoteAddr, mRemotePort, 1, eAosTLT_FirstFourHigh);
		rslt = mConn->connect(errmsg);
	}
	while(!rslt);
	OmnCommListenerPtr thisPtr(this, false);
	mConn->startReading(thisPtr);
}


bool
AosTransPest::reconnect()
{
	// If the connection was created, it close the connection. 
	// It then connects to the server. If the connection can be
	// created and ok, it returns true. Otherwise, it will close
	// the connection and reconnect again.
	if (mConn) 
	{
		mConn->closeConn();
		OmnCommListenerPtr thisPtr(this, false);
		mConn->stopReading(thisPtr);
	}
	connect();
	if (!mConn) return false;
	return true;
}


void 
AosTransPest::sendHeartbeat()
{
	//the function keep the connection avaliable
	OmnString errmsg, resp;
	//OmnString req = "<Request heartbeat=\"true\"/>";
	OmnString req = "isHeartbeat";
OmnScreen << req <<endl;
	mConn->sendTo(req.data(),req.length());
}


bool
AosTransPest::msgRead(const OmnConnBuffPtr &buff)
{
	//The response formate shoud be:
	//<Response zky_tid=xxxx/>
	//
	//when we read the response .we check the response is which trans
	//then we remove the the trans form procQueue
	char *data = buff->getData();
	AosXmlParser parser;
	AosXmlTagPtr root = parser.parse(data, "");
	AosXmlTagPtr child = root->getFirstChild();
	aos_assert_r(child, false);

	u64 tid = child->getAttrU64("zky_tid", 0);
	aos_assert_r(tid, false);	

	mLock->lock();
	const Trans *tr = mProcQueue[tid];
	aos_assert_rl(tr, mLock, false);

	delete []tr;
	mProcQueue.erase(tid);
	mLock->unlock();

	return true;
}


bool    
AosTransPest::saveTrans(const OmnString &trans)
{
	// This function is called when someone has a 
	// transaction that needs to be handled by this
	// class. 
	int tid = getTransId();
	Trans* tr = OmnNew Trans(trans, tid);
	mLock->lock();
	OmnFilePtr file  = getFile(tr);
	aos_assert_rl(file, mLock, false);
	bool rslt = tr->saveToFile(file);
	aos_assert_rl(rslt, mLock, false);
	mSendQueue.push(tr);
	mCondVar->signal();
	mLock->unlock();
	return true;
}


int 
AosTransPest::getTransId()
{
	// "0" is reserved num
	// we used it to error checking
	u32 id = 0;
	sLock.lock();
	id = sCrtTransId++;
	if (!id) id = sCrtTransId++;
	sLock.unlock();
	return id; 
}


OmnFilePtr 
AosTransPest::getFile(Trans *trans, bool readOnly)
{
	if (mFile)
	{                                       
		mFileSize += trans->mFileSize;
		if (readOnly || mFileSize < eMaxFileSize)
			return mFile;
	}

	OmnString fname = mDirName;
	fname << "/" << mFileName << "_" << mCrtSeqno++;
	mFile = OmnNew OmnFile(fname, OmnFile::eReadWrite);
	if (!mFile->isGood())
	{
		    // The file has not been created yet. Create it.
		mFile = OmnNew OmnFile(fname, OmnFile::eCreate);
		aos_assert_r(mFile->isGood(), NULL);
	}
	mFileSize = mFile->getLength();
	return mFile;
}
