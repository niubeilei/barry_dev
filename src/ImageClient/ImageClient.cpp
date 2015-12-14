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
#include "ImageClient/ImageClient.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "Debug/Debug.h"
#include "Porting/Sleep.h"
#include "Util/File.h"
#include "SingletonClass/SingletonImpl.cpp"
#include "Thread/Mutex.h"
#include "Thread/Thread.h"
#include "Util/OmnNew.h"
#include "sys/stat.h"
#include "UtilComm/TcpClient.h"
#include "XmlUtil/SeXmlParser.h"
#include "XmlUtil/XmlTag.h"


OmnSingletonImpl(AosImageClientSingleton,
                 AosImageClient,
                 AosImageClientSelf,
                "AosImageClient");


static OmnString sgImagePath;
const int sgGroupSize = 1000;
const int sgSleepLength = 10;

AosImageClient::AosImageClient()
:
mLock(OmnNew OmnMutex()),
mRemotePort(-1)
{
}


AosImageClient::~AosImageClient()
{
	mConn->closeConn();
}


bool      	
AosImageClient::start()
{
	return true;
}


bool        
AosImageClient::stop()
{
	return true;
}


OmnRslt     
AosImageClient::config(const OmnXmlParserPtr &def)
{
	return true;
}


bool
AosImageClient::start(const AosXmlTagPtr &config)
{
	mRemoteAddr = config->getAttrStr(AOSCONFIG_REMOTE_ADDR);
	mRemotePort = config->getAttrInt(AOSCONFIG_REMOTE_PORT, -1);
	connect();
	OmnThreadedObjPtr thisPtr(this, false);
	mHeartbeatThread = OmnNew OmnThread(thisPtr, "HeartBeat", eHeartbeatThrdId, true, true, __FILE__, __LINE__);
	mReadThread = OmnNew OmnThread(thisPtr, "ReadThrd", eReadThrdId, true, true, __FILE__, __LINE__);
	mHeartbeatThread->start();
	mReadThread->start();
	return true;
}


bool    
AosImageClient::signal(const int threadLogicId)
{
	return true;
}


bool    
AosImageClient::checkThread(OmnString &err, const int thrdLogicId) const
{
	return true;
}


bool
AosImageClient::threadFunc(
		OmnThrdStatus::E &state,
		const OmnThreadPtr &thread)
{
	if (thread->getLogicId() == eHeartbeatThrdId)
	{
		return heartbeatThrdFunc(state, thread);
	}

	if (thread->getLogicId() == eReadThrdId)
	{
		return readThrdFunc(state, thread);
	}

	OmnAlarm << "Invalid thread logic id: " << thread->getLogicId() << enderr;
	return false;
}


bool
AosImageClient::heartbeatThrdFunc(
		OmnThrdStatus::E &state,
		const OmnThreadPtr &thread)
{
	while (state == OmnThrdStatus::eActive)
	{
		sendHeartbeat();
		OmnSleep(60);
	}

	return true;
}


bool
AosImageClient::readThrdFunc(
		OmnThrdStatus::E &state,
		const OmnThreadPtr &thread)
{
	// When the server wants to send an image, it will send a request
	// first. It then sends the image itself. Note that the image 
	// should start with the length indicator.
	OmnConnBuffPtr msgbuff;
	while (state == OmnThrdStatus::eActive)
	{
		int bytesread = mConn->smartRead(msgbuff);
		if (bytesread <= 0)
		{
			// Server failed. We need to reconnect. 
			reconnect();
			continue;
		}

		msgReceived(msgbuff);
	}

	return true;
}


void
AosImageClient::connect()
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
AosImageClient::reconnect()
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
AosImageClient::sendHeartbeat()
{
	OmnString errmsg, resp;
	OmnString req = "<Heartbeat/>";
	mConn->smartSend(req.getBuffer(),req.length());
}


void
AosImageClient::msgReceived(const OmnConnBuffPtr &buff)
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
AosImageClient::createFile(OmnString &path, const OmnConnBuffPtr buff)
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
AosImageClient::sendResponse(
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
