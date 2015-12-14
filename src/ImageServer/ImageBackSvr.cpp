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
// 	Created: 05/09/2010 by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "ImageBackSvr/ImageBackSvr.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "Debug/Debug.h"
#include "Porting/Sleep.h"
#include "Util/File.h"
#include "SEUtil/SeXmlParser.h"
#include "SEUtil/XmlTag.h"
#include "SingletonClass/SingletonImpl.cpp"
#include "Thread/Mutex.h"
#include "Thread/Thread.h"
#include "Util/OmnNew.h"
#include "sys/stat.h"
#include "UtilComm/TcpClient.h"


OmnSingletonImpl(AosImageBackSvrSingleton,
                 AosImageBackSvr,
                 AosImageBackSvrSelf,
                "AosImageBackSvr");


static OmnString sgImagePath;
const int sgGroupSize = 1000;
const int sgSleepLength = 10;

AosImageBackSvr::AosImageBackSvr()
:
mLock(OmnNew OmnMutex()),
mRemotePort(-1)
{
}


AosImageBackSvr::~AosImageBackSvr()
{
	mConn->closeConn();
}


bool      	
AosImageBackSvr::start()
{
	return true;
}


bool        
AosImageBackSvr::stop()
{
	return true;
}


OmnRslt     
AosImageBackSvr::config(const OmnXmlParserPtr &def)
{
	return true;
}


bool
AosImageBackSvr::start(const AosXmlTagPtr &config)
{
	mRemoteAddr = config->getAttrStr(AOSCONFIG_REMOTE_ADDR);
	mRemotePort = config->getAttrInt(AOSCONFIG_REMOTE_PORT, -1);
	connect();
	OmnThreadedObjPtr thisPtr(this, false);
	mHeartbeatThread = OmnNew OmnThread(thisPtr, "HeartBeatThrd", eHeartbeatThrdId, true, true, __FILE__, __LINE__);
	mReadThread = OmnNew OmnThread(thisPtr, "ReadThrd", eReadThrdId, true, true, __FILE__, __LINE__);
	mHeartbeatThread->start();
	mReadThread->start();
	return true;
}


bool    
AosImageBackSvr::signal(const int threadLogicId)
{
	return true;
}


bool    
AosImageBackSvr::checkThread(OmnString &err, const int thrdLogicId) const
{
	return true;
}


bool
AosImageBackSvr::threadFunc(
		OmnThrdStatus::E &state,
		const OmnThreadPtr &thread)
{
	while (state == OmnThrdStatus::eActive)
	{
		mLock->lock();
		if (mQueue.length() == 0)
		{
			// There is no more requests.
			mCondVar->wait(mLock);
			mLock->unlock();
			continue;
		}

		request = mQueue.pop();
		mLock->unlock();

		processReq(request);
	}

	return true;
}


bool
AosImageBackSvr::processReq(const AosImageReqPtr &req)
{
	OmnString fname = req->getFname();

	mConn;

	1. Open the file
	2. Send the request;
	3. Send the image itself;
	return true;
}


void
AosImageBackSvr::connect()
{
	OmnString errmsg;
	do
	{
		if (errmsg !="")
			OmnAlarm << errmsg << enderr;
		mConn = OmnNew OmnTcpClient("nn", mRemoteAddr, mRemotePort, 1, eAosTLT_FirstFourHigh);
		OmnSleep(1);
	}
	while(mConn->connect(errmsg));
}


bool
AosImageBackSvr::reconnect()
{
	// If the connection was created, it close the connection. 
	// It then connects to the server. If the connection can be
	// created and ok, it returns true. Otherwise, it will close
	// the connection and reconnect again.
	if (mConn) mConn->closeConn();
	mConn=NULL;
	connect();
	if (!mConn) return false;
	return true;
}


void 
AosImageBackSvr::sendHeartbeat()
{
	OmnString errmsg, resp;
	OmnString req = "<Heartbeat/>";
	mConn->smartSend(req.getBuffer(),req.length());
}


void
AosImageBackSvr::msgReceived(const OmnConnBuffPtr &buff)
{
	// The received is an xml. The message should be in the form:
	// 	<req opr="xxx" .../>
	// 1. Parse the
	
    AosXmlRc errcode = eAosXmlInt_General;
	OmnString errmsg = "";

	char *data = buff->getData();
	AosXmlParser parser;
	AosXmlTagPtr root = parser.parse(data, "");
	AosXmlTagPtr child;
	
	OmnString opr = root->getAttrStr(AOSTAG_OPR);

	if (opr == "AOSVALUE_BACKUPIMAGE")
	{
		// The message should be in the form:
		// 	<req opr=AOSVALUE_BACKUPIMAGE fname="xxx"/>
		OmnString fname = root->getAttrStr(AOSTAG_FFNAME);
		if (fname == "")
		{
			sendResponse(errcode, "Miss file name", "");
			OmnAlarm << "Missing file name: " << enderr;
			return;
		}

		OmnConnBuffPtr imgbuff;
		int bytesread = mConn->smartRead(imgbuff);
		if (bytesread <= 0)
		{
			reconnect();
			return;
		}

		// It read the image. Need to save the image to the location
		// indicated by the message
		bool rest = createFile(fname ,imgbuff);
		if(rest)
		{
			errcode = eAosXmlInt_FailedOpenFile;
			sendResponse(errcode, "Can't create file", "");
			OmnAlarm << "Can't create file" << enderr;
			return;
		}
		errcode = eAosXmlInt_Ok;
		sendResponse(errcode ,"","Success");
		return;
	}
}


bool
AosImageBackSvr::createFile(OmnString &path, const OmnConnBuffPtr buff)
{
	mkdir(path.substr(0,(u32)path.find('/',true)).getBuffer(), 0775);
	OmnFilePtr file = OmnNew OmnFile(path, OmnFile::eCreate);
	aos_assert_r(file->isGood(), false);
	char *data = buff->getBuffer();
	u32 length = *((int *)data);
	bool rest = file->put(0,&(data[4]),length,true);
	aos_assert_r(rest,true);
	return true;
}


void
AosImageBackSvr::sendResponse(
		const AosXmlRc errcode,
		const OmnString &errmsg,
		const OmnString &contents)
{
	OmnString resp = "<Response><status";
	if (errcode == eAosXmlInt_Ok) resp << "false\" code=\"200\"/>";
	else resp << "true\" code=\"" << errcode << "\"><![CDATA[" << errmsg << "]]></status>";

	if (contents != "" ) resp << contents;

	resp << "</Response>";

	mConn->smartSend(resp.getBuffer(),resp.length());
}


bool
AosImageBackSvr::addReq(const OmnString &fname)
{
	AosImageReqPtr req = OmnNew AosImageReq(fname);
	mLock->lock();
	mQueue.push(req);
	mCondVar->signal();
	mLock->unlock();
	return true;
}


