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
// This class is used as the Data Sync Client. When a requester has something
// that needs to be synched with a remote site, it calls its function. 
//
// To ensure the reliability, each transaction must be responded. Otherwise, 
// it may re-send the request. On the receiving side, it should ignore the
// re-transmitted requests. 
//
// A client may connect to multiple sites. It may sync with one or more
// remote sites. If one site fails, it may sync with the backup.
//
// Modification History:
// 10/12/2010	Created by Chen Ding
// 2014/01/28 Turned off by Chen Ding
////////////////////////////////////////////////////////////////////////////
#if 0
#include "DataSync/Conn.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "AppMgr/App.h"
#include "HealthCheck/Ptrs.h"
#include "Porting/Sleep.h"
#include "Rundata/Rundata.h"
#include "SeSysLog/SeSysLog.h"
#include "Thread/Mutex.h"
#include "Thread/Thread.h" 
#include "Thread/ThreadMgr.h"
#include "Thread/CondVar.h" 
//#include "TransUtil/TransFileMgr.h"
#include "Util/OmnNew.h"
#include "UtilComm/TcpClient.h"
#include "UtilComm/TcpCommClt.h"
#include "UtilComm/CommTypes.h"
#include "Util/File.h"
#include "Util1/Wait.h"
#include "XmlUtil/XmlTag.h"

extern AosSeSysLog sgSeSysLog;
static int sgAt = 0;

AosConn::AosConn()
:
mLock(OmnNew OmnMutex()),
mCondVar(OmnNew OmnCondVar()),
mLogid(0)
//mRundata(OmnNew AosRundata())
{
}


AosConn::~AosConn()
{
}

bool
AosConn::start(const AosXmlTagPtr &config)
{
	AosXmlTagPtr def = config->getFirstChild("DataSync");
	if(NULL == def)
	{
		OmnAlarm << "DataSync's config is not find!" << enderr;
		return false;
	}

	mClientId = def->getAttrStr(AOSMSG_CLIENTID);
	aos_assert_r(mClientId != "", false);

	mAddr = def->getAttrStr(AOSMSG_ADDR);
	aos_assert_r(mAddr!="", false);
	
	mPort = def->getAttrInt(AOSMSG_PORT, -1);
	aos_assert_r(mPort > 0, false);

	OmnThreadedObjPtr thisPtr(this, false);
	mMonitorThread = OmnNew OmnThread(thisPtr, "conn", 0, true, true, __FILE__, __LINE__);
	
	aos_assert_r(createConnection(), false);

	return true;
}

	
bool
AosConn::createConnection()
{
	mConn = OmnNew OmnTcpCommClt(mAddr, mPort, 1, eAosTLT_FirstFourHigh);
	if(NULL != mConn)
	{
		OmnCommListenerPtr thisPtr(this, false);
		mConn->startSmartReading(thisPtr);
	}

	if (mMonitorThread->getStatus() != OmnThrdStatus::eActive)
	{
		mMonitorThread->start();
	}
	
	OmnString req = "<Opr type=\"Link\"/>";
	return sendResp(req);
}


OmnString
AosConn::getCommListenerName() const
{
	return "Conn";
}


bool
AosConn::msgRead(const OmnConnBuffPtr &buff)   
{
	OmnString bf = buff->getString();
	aos_assert_r(bf.length(), false);

	// Check Response
	OmnString resp = "<response>";
	resp << buff->getData() << "</response>";
	OmnString rep = "<response></response>";
	if(strcmp(resp, rep)==0)
	{
		OmnAlarm << "buff is empty" << enderr;
		return false;
	}

	AosXmlParser parser;
	AosXmlTagPtr root = parser.parse(bf, "" AosMemoryCheckerArgs);
	AosXmlTagPtr child;
	if (!root || !(child = root->getFirstChild()))
	{
		OmnAlarm << "Failed to parse the request!" << enderr;
		OmnScreen << resp << endl;
		return false;
	}

	aos_assert_r(procResp(root, child, mRundata), false);
	return true;
}


bool
AosConn::procResp(
		const AosXmlTagPtr &root, 
		const AosXmlTagPtr &child, 
		const AosRundataPtr &rdata)
{
	OmnString errmsg;
	OmnString resp = "<Opr type=\"Result\"/>";
	
	mLock->lock();
	OmnString transid = child->getAttrStr("transid");
	OmnString type = child->getAttrStr("type");
	if (type == "Connection")
	{
		OmnApp::running();
		mLock->unlock();
		return true;
	}
	resp << "<Contents transid=\"" << transid;

	AosXmlTagPtr xmldoc = child->getFirstChild("objdef");
	if (!xmldoc)
	{
		errmsg = "Missing the object to be dataSync";
		OmnAlarm << errmsg << enderr;
		aos_assert_rl(sendResp(errmsg), mLock, false);
		mLock->unlock();
		return false;
	}

	AosXmlTagPtr doc = xmldoc->getFirstChild();
	if (!doc)
	{
		errmsg = "Missing the object to be dataSync(1)";
		OmnAlarm << errmsg << enderr;
		aos_assert_rl(sendResp(errmsg), mLock, false);
		mLock->unlock();
		return false;
	}

	u32 logid = doc->getAttrU32("logid", 0);
	resp << "\" logid=\"" << logid << "\"/>";

	if (mLogid == logid && logid != 0)
	{
		aos_assert_rl(sendResp(resp), mLock, false);
		mLock->unlock();
		return true;
	}

	bool rs = sgSeSysLog.recoverSystem(root, doc, rdata);

	if (!rs)
	{
		errmsg = "Fail To recoverSystem!~";
		OmnAlarm << errmsg << enderr;
		aos_assert_rl(sendResp(errmsg), mLock, false);
		mLock->unlock();
		return false;                                           
	}
	mLogid = logid;

OmnScreen << "~~~~~~~~~~~~~~~logid~~~~~~~~" << logid << endl;	
OmnScreen << "~~~~~~~~~~~~~~~sssss~~~~~~~~" << sgAt++ << endl;	

	aos_assert_rl(sendResp(resp), mLock, false);
	mLock->unlock();
	return true;
}

bool
AosConn::sendResp(const OmnString req)
{
	// This Fuction Can Reconnect!
	OmnRslt rslt =  mConn->sendTo(req.data(), req.length());
	while (!rslt)
	{
		rslt =  mConn->sendTo(req.data(), req.length());
		OmnSleep(eWaitTime);
	}

	return rslt;
}

bool    
AosConn::threadFunc(OmnThrdStatus::E &state, const OmnThreadPtr &thread)
{
	switch (thread->getLogicId())
	{
		case 0:
			return monitorThreadFunc(state, thread);

		default:
			OmnAlarm << "Incorrect thread id: " << thread->getLogicId() << enderr;
			return false;
	}

	return false;
}

bool
AosConn::monitorThreadFunc(OmnThrdStatus::E &state, const OmnThreadPtr &thread)
{
	OmnString req = "<Opr type=\"Link\"/>";
	while (state == OmnThrdStatus::eActive)
	{
		OmnWait::getSelf()->wait(eMonitorFreqSec, 0);
	
		mLock->lock();
		aos_assert_rl(sendResp(req), mLock, false);
		mLock->unlock();
	}
	return true;
}


bool    
AosConn::signal(const int threadLogicId)
{
	return true;
}


bool    
AosConn::checkThread(OmnString &err, const int thrdLogicId) const
{
	return true;
}
#endif
