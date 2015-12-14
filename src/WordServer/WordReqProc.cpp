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
// 10/13/2010: Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "WordServer/WordReqProc.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "AppMgr/App.h"
#include "XmlParser/XmlParser.h"
#include "UtilComm/TcpClient.h"
#include "SEUtil/DocTags.h"
#include "SEUtil/SeTypes.h"
#include "SEUtilServer/SeIdGenMgr.h"
#include "UtilComm/ConnBuff.h"
#include "HealthCheck/HealthChecker.h"
#include "HealthCheck/HealthCheckObj.h"
#include "Thread/Mutex.h"
#include "WordMgr/WordMgr1.h"
#include "XmlUtil/SeXmlParser.h"
#include "XmlUtil/XmlTag.h"

extern int gAosLogLevel;
AosWordReqProc::AosWordReqProc()
:mLock(OmnNew OmnMutex())
{
}


AosWordReqProc::~AosWordReqProc()
{
}


AosNetReqProcPtr	
AosWordReqProc::clone()
{
	return OmnNew AosWordReqProc();
}


bool
AosWordReqProc::config(const AosXmlTagPtr &config)
{
	return true;
}


bool
AosWordReqProc::stop()
{
	return true;
}


bool
AosWordReqProc::procRequest(const OmnConnBuffPtr &buff)
{
	OmnTcpClientPtr conn = buff->getConn();
	aos_assert_r(conn, false);

	AosWebRequestPtr req = OmnNew AosWebRequest(conn, buff);
	u32 transid = req->getTransId();
	if (gAosLogLevel >= 2)
	{
		OmnScreen << "\nTo process request: (transid: " << transid
			<< "):\n" << req->getData() << endl;
	}

	AosXmlRc errcode = eAosXmlInt_General;
	if (mIsStopping)
	{
		sendResp(req, errcode, "Server is stopping", "");
		return false;
	}

	OmnString errmsg;
	char *data = req->getData();

	AosXmlParser parser;
	AosXmlTagPtr root = parser.parse(data, "" AosMemoryCheckerArgs);
	AosXmlTagPtr child;
	if (!root || !(child = root->getFirstChild()))
	{
		errmsg = "Failed to parse the request!";
		OmnAlarm << errmsg << enderr;
		OmnScreen << data << endl;
		sendResp(req, errcode, errmsg, "");
		return false;
	}
	OmnString operation = root->getChildTextByAttr("name", "operation");
	mOperation = AosWordOpr::toEnum(operation);
	bool rslt = procReq(req, root, child);
	return rslt;
}


bool
AosWordReqProc::procReq(
		const AosWebRequestPtr &req, 
		const AosXmlTagPtr &root,
		const AosXmlTagPtr &child)
{
	switch (mOperation)
	{
	case AosWordOpr::eAdd:
		 return getWordid(req, root, child);

	default:
		 break;
	}

	AosXmlRc errcode = eAosXmlInt_General;
	OmnString errmsg = "Unrecognized operation: ";
	errmsg << mOperation;
	OmnAlarm << errmsg << enderr;
	sendResp(req, errcode, errmsg, "");
	return true;
}


void
AosWordReqProc::sendResp(
		const AosWebRequestPtr &req, 
		const AosXmlRc errcode,
		const OmnString &errmsg,
		const OmnString &contents) 
{
	OmnString resp = "<status ";
	resp << " error=\"";
	if (errcode == eAosXmlInt_Ok) resp << "false\" code=\"200\"/>";
	else resp << "true\" code=\"" << errcode << "\"><![CDATA[" << errmsg << "]]></status>";

	if (contents != "" ) resp << contents;

	if (gAosLogLevel >= 2)
	{
		OmnScreen << "Send response: (transid: " << req->getTransId()
			<< "):\n" << resp << endl;
	}
	req->sendResponse(resp);
}


bool
AosWordReqProc::getWordid(
		const AosWebRequestPtr &req, 
		const AosXmlTagPtr &root,
		const AosXmlTagPtr &child)
{
	// It retrieves the wordid for a given word. The request
	// should be in the form:
	// 	<request transid="xxx">
	// 		<item name='word'><![CDATA[word]]></item>
	// 	</request>
	
	mLock->lock();
	OmnString word = root->getChildTextByAttr("name", "word");
OmnScreen << "~~~~~~word~~~~" << word << endl;
	OmnString transid = root->getChildTextByAttr("name", "transid");
	
	//have 
	u64 wordid = AosWordMgr1::getSelf()->getWordId(word, false);
	if (wordid!=AOS_INVWID)
	{
		u64 iilid;
		bool rs = AosWordMgr1::getSelf()->getPtr(wordid, iilid);
		aos_assert_rl(rs, mLock, false);
		aos_assert_rl(iilid, mLock, AOS_INVWID);
		OmnString contents;
		contents << "<Contents wordid=\"" << wordid 
			<<"\" " << "word=\"" << word 
			<<"\" " << "iilid=\"" << (iilid>>32)
			<<"\" " << "transid=\"" << transid << "\"/>";
		sendResp(req, eAosXmlInt_Ok, "", contents);
		mLock->unlock();
		return true;
	}


	//add
	wordid = AosWordMgr1::getSelf()->getWordId(word, true);
	if (wordid == AOS_INVWID)
	{
		mLock->unlock();
		OmnAlarm << "Failed to resolve word: " << word << enderr;
		sendResp(req, eAosXmlInt_General, "Failed to resolve word!", "");
		return false;
	}

	u64 iilid = AosSeIdGenMgr::getSelf()->nextIILId();
	AosWordMgr1::getSelf()->savePtr1(wordid, (iilid << 32));

	// 	<Contents wordid="xxx" transid="xxx"/>
	OmnString contents;
	contents << "<Contents wordid=\"" << wordid 
		<<"\" " << "word=\"" << word 
		<<"\" " << "iilid=\"" << iilid 
		<<"\" " << "transid=\"" << transid << "\"/>";
	sendResp(req, eAosXmlInt_Ok, "", contents);
	mLock->unlock();
	return true;
}

