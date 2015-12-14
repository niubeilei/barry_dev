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
#include "TransPest/TransPtSrv.h"

#include "UtilComm/TcpCommClt.h"
#include "UtilComm/TcpClient.h"
#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "Debug/Debug.h"
#include "Porting/Sleep.h"
#include "Util/File.h"
#include "Thread/Mutex.h"
#include "Thread/Thread.h"
#include "Util/OmnNew.h"
#include "sys/stat.h"
#include "XmlUtil/SeXmlParser.h"
#include "XmlUtil/XmlTag.h"


AosTransPtSrv::AosTransPtSrv()
:
mLock(OmnNew OmnMutex()),
mFileSize(0),
mCrtSeqno(0)
{
}


AosTransPtSrv::~AosTransPtSrv()
{
	mConn->stopReading();
	mConn->closeConn();
}


bool
AosTransPtSrv::init(const AosXmlTagPtr &config)
{
	mLocalAddr = config->getAttrStr(AOSCONFIG_LOCAL_ADDR);
	aos_assert_r(mLocalAddr != "",false);
	mLocalPort = config->getAttrInt(AOSCONFIG_LOCAL_PORT, -1);
	aos_assert_r(mLocalPort != -1,false);
	mFileName = config->getAttrStr(AOSCONFIG_FILENAME,"");
	aos_assert_r(mFileName != "",false);
	mDirName = config->getAttrStr(AOSCONFIG_DIRNAME, "");

	connect();

	OmnThreadedObjPtr thisPtr(this, false);
	mCheckThrd = OmnNew OmnThread(thisPtr, "CheckThrd", eCheckThrdId, true, true, __FILE__, __LINE__);
	return true;
}


bool    
AosTransPtSrv::signal(const int threadLogicId)
{
	return true;
}


bool    
AosTransPtSrv::checkThread(OmnString &err, const int thrdLogicId) const
{
	return true;
}


bool
AosTransPtSrv::threadFunc(
		OmnThrdStatus::E &state,
		const OmnThreadPtr &thread)
{
	if (thread->getLogicId() == eCheckThrdId)
	{
		return checkThrdFunc(state, thread);
	}

	OmnAlarm << "Invalid thread logic id: " << thread->getLogicId() << enderr;
	return false;
}


bool
AosTransPtSrv::sendResponse(Trans *trans)
{
	return true;	
}


void
AosTransPtSrv::connect()
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
		mConn = OmnNew OmnTcpServer(mLocalAddr, mLocalPort, 1, NULL, eAosTLT_FirstFourHigh);
		rslt = mConn->connect(errmsg);
	}
	while(!rslt);
	OmnTcpListenerPtr thisPtr(this, false);
	mConn->setListener(thisPtr);
	mConn->startReading();
}


bool
AosTransPtSrv::reconnect()
{
	// If the connection was created, it close the connection. 
	// It then connects to the server. If the connection can be
	// created and ok, it returns true. Otherwise, it will close
	// the connection and reconnect again.
	if (mConn) 
	{
		mConn->closeConn();
		mConn->stopReading();
		mConnecting = false;
	}
	connect();
	if (!mConn) return false;
	return true;
}


bool
AosTransPtSrv::msgRead(const OmnConnBuffPtr &buff)
{
	////////////////////////////////////////////////
	//the message format shoud be:
	//1:
	//	<Request heartbeat="true"/>
	//	the message is used to keep connecting
	//
	//2:
	//	first send messge
	//	transaction_id\n
	//	data
	///////////////////////////////
	//	<Request zky_tid="xxx">
	//	and the send message 
	//	<Request time="xxxxxxxxxx">
	//		<Contents>
	//		...
	//		</Contents>
	//	</Request>
	
	OmnTcpClientPtr conn = buff->getConn();
	aos_assert_r(conn, false);
	if (!mConnecting)
	{
		mConnecting = true;
		mCheckThrd->start();
	}

//	char *data = buff->getData();
	
	OmnString data = buff->getString();
	if(data == "isHeartbeat") return true;

	u32 idx = data.find('\n', true);
	int tid;
	u32 tidIdx = u32(idx-1);
	data.parseInt((u32)0, tidIdx, tid);

	OmnString contents = data.substr(idx+1, data.length());
	aos_assert_r(contents, false);
	
	
	if (tid != 0)
	{
		map<u32, Trans*>::iterator trTid = mRecieveQueue.find(tid);
		if (trTid != mRecieveQueue.end() && !mRecieveQueue[tid])
		{
			//it means the transaction was saved
			//we do not save it agian.
			OmnString response = "<Response error=\"false\" zky_tid=\"";
			response << tid << "\">";
			//if (!conn->sendMsgTo(response))
			if (!conn->smartSend(response))
			{
				mConnecting = false;
				mCheckThrd->stop();
			}
			mRecieveQueue[tid]->sending = -1;
			return true;
		}
		Trans* tr = OmnNew Trans(contents, tid, tid);
		mLock->lock();
		mRecieveQueue.insert(make_pair(tid, tr));
		mLock->unlock();

		OmnFilePtr file = getFile(tr);
		aos_assert_r(file, false);
		tr->saveToFile(file);
		tr->sending = -1;
		return true;
	}

	//recieve data

	/*AosXmlParser parser;
	AosXmlTagPtr root = parser.parse(data, "");
	aos_assert_r(root, false);
	AosXmlTagPtr child = root->getFirstChild();

	// handle heartbeat
	bool isHeadbeat = child->getAttrBool("heartbeat");
	if (isHeadbeat) return true;

	//recieve trans id 
	
	
	
	int tid = child->getAttrInt("zky_tid",0);
	if (tid != 0)
	{
		map<u32, Trans*>::iterator trTid = mRecieveQueue.find(tid);
		if (trTid != mRecieveQueue.end() && !mRecieveQueue[tid])
		{
			//it means the transaction was saved
			//we do not save it agian.
			OmnString response = "<Response error=\"false\" zky_tid=\"";
			response << tid << "\">";
			//if (!conn->sendMsgTo(response))
			if (!conn->smartSend(response))
			{
				mConnecting = false;
				mCheckThrd->stop();
			}
			mRecieveQueue[tid]->sending = -1;
			mCrtTid = 0;
			return true;
		}
		Trans* tr = NULL;
		mRecieveQueue.insert(make_pair(tid, tr));
		mCrtTid = tid;
		return true;
	}

	//recieve data
	u32 time = child->getAttrU32("time", 0);

	//It mean the trans we have processed
	if (mCrtTid == 0) return true;

	if (time != 0)
	{
		AosXmlTagPtr contents = child->getNextChild();
		aos_assert_r(contents, false);
		OmnString str = contents->getNodeText();
		aos_assert_r(str != "", false);

		Trans* tr = OmnNew Trans(str, mCrtTid, time);	
		mRecieveQueue[mCrtTid] = tr;

		OmnFilePtr file = getFile(tr);
		aos_assert_r(file, false);
		tr->saveToFile(file);
		tr->sending = -1;
		
		//send confirm message
		OmnString response = "<Response error=\"false\" zky_tid=\"";
		response << mCrtTid << "\">";
		//if(!conn->sendMsgTo(response))
		if(!conn->smartSend(response))
		{
			mConnecting = false;
			mCheckThrd->stop();
		}
		return true;
	}
	*/
	return false;
}


OmnFilePtr 
AosTransPtSrv::getFile(Trans *trans, bool readOnly)
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


bool
AosTransPtSrv::checkThrdFunc(
		OmnThrdStatus::E &state,
		const OmnThreadPtr &thread)
{
	while (state == OmnThrdStatus::eActive)
	{
		mLock->lock();
		map<u32, Trans*>::iterator itr = mRecieveQueue.begin();
		while (itr != mRecieveQueue.end())
		{
			if (itr->second->sending >= -2)
			{
				itr->second->sending--;
				break;
			}

			Trans* tr = itr->second;    
			mRecieveQueue.erase(itr);
			delete []tr;
			itr++;
		}
		mLock->unlock();
		OmnSleep(eCheckThrdFreq);
	}
	return true;
}
