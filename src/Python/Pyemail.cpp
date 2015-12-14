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
// This file is generated automatically by the ProgramAid facility. 
//
// Modification History:
// 3/23/2007: Created by cding
////////////////////////////////////////////////////////////////////////////
#include "Python/Pyemail.h"


#include "Alarm/Alarm.h"
#include "alarm_c/alarm.h"
#include "Debug/Debug.h"
#include "Python/Python.h"
#include "Rundata/Rundata.h"
#include "SEUtil/Ptrs.h"
#include "SingletonClass/SingletonImpl.cpp"
#include "Util/OmnNew.h"
#include "Util/HashUtil.h"
#include "XmlUtil/XmlTag.h"
#include "XmlUtil/XmlDoc.h"
#include "XmlUtil/SeXmlParser.h"
#include "XmlInterface/XmlRc.h"

#include <python2.6/Python.h>

static OmnString sgForgotSender = "zykier@163.com";
static OmnString sgForgotPasswd = "lindalin";
static hash_map<OmnString, OmnString, Omn_Str_hash, compare_str> sgServerMap;
static bool sgServerInitFlag = false;

OmnSingletonImpl(AosPyemailSingleton,
				AosPyemail,
				AosPyemailSelf,
				"AosPyemail");

AosPyemail::AosPyemail()
:
mLock(OmnNew OmnMutex()),
mCondVar(OmnNew OmnCondVar()),
mNumReqs(0)
{
	OmnThreadedObjPtr thisPtr(this, false);
	mThread = OmnNew OmnThread(thisPtr, "PyemailThrd", 0, true, true, __FILE__, __LINE__);
	mThread->start();
}

AosPyemail::~AosPyemail()
{
}

bool
AosPyemail::start()
{
	return true;
}

bool
AosPyemail::stop()
{
	return true;
}

bool
AosPyemail::config(const AosXmlTagPtr &config)
{
	return true;
}

bool
AosPyemail::threadFunc(OmnThrdStatus::E &state,const OmnThreadPtr &thread)
{
	while(state == OmnThrdStatus::eActive)
	{
		mLock->lock();
		if (mNumReqs == 0)
		{
			// There is no more cached entries. Start a timer to
			// wake this thread up later.
			mCondVar->wait(mLock);
			mLock->unlock();
			continue;
		}
		
		AosStRequestPtr request = getRequest();
		aos_assert_rl(request, mLock, false);
		mNumReqs --;

		
		aos_assert_rl(request->sender != "", mLock, false);
		aos_assert_rl(request->sender_passwd != "", mLock, false);
		aos_assert_rl(request->receiver != "", mLock, false);
		aos_assert_rl(request->subject != "", mLock, false);
		aos_assert_rl(request->server != "", mLock, false);
		AosRundataPtr rdata = request->rdata;
		aos_assert_rl(rdata, mLock, false);

		bool rslt = AosPythonSelf->sendEmail(
				request->receiver, request->sender, request->sender_passwd, 
				request->subject, request->contents, request->server, 
				request->count, rdata); 
		if (!rslt)
		{
			rdata->setError() << "Failed to send email!";
			OmnAlarm << rdata->getErrmsg() << enderr;
			mLock->unlock();
			return false;
		}

		mLock->unlock();
		return true;
	}
	return true;
}

bool
AosPyemail::signal(const int threadLogicId)
{
	return true;
}

bool
AosPyemail::checkThread(OmnString &err, const int thrdLogicId) const
{
	return true;
}

void
AosPyemail::serverInit()
{
	sgServerMap["sina"] = "smtp.sina.com"; 			//sina smtp.sina.com 
	sgServerMap["163"] = "smtp.163.com"; 			//163 smtp.163.com
	sgServerMap["gmail"] = "smtp.gmail.com"; 		//gmail smtp.gmail.com
	sgServerMap["126"] = "smtp.126.com"; 			//126 smtp.126.com
	sgServerMap["sohu"] = "smtp.sohu.com";			//sohu smtp.sohu.com
	sgServerMap["qq"] = "smtp.qq.com";				//qq smtp.qq.com
	sgServerMap["yahoo"] = "smtp.mail.yahoo.com";	//yahoo smtp.mail.yahoo.com
	sgServerMap["yeah"] = "smtp.yeah.net"; 			//yeah smtp.yeah.net
	sgServerMap["hotmail"]= "smtp.live.com";	 	// hotmail smtp.live.com
	sgServerMap["139"] = "smtp.139.com";			//139 smtp.139.com
	sgServerInitFlag = true;
}

void
AosPyemail::addRequest(
		const OmnString &sender,
		const OmnString &senderpasswd,
		const OmnString &receiver,
		const OmnString &subject,
		const OmnString &body,
		const OmnString &server,
		const OmnString &count,
		const AosRundataPtr &rdata)
{
	AosStRequestPtr request = OmnNew AosStRequest();
	request->sender = sender;
	request->sender_passwd = senderpasswd;
	request->receiver = receiver;
	request->subject = subject;
	request->contents = body;
	request->server = server;
	request->count  = count;
	request->rdata = rdata;
	aos_assert(request);

	mLock->lock();
	mNumReqs ++;
	mQueue.push(request);
	mCondVar->signal();
	mLock->unlock();
}

AosStRequestPtr
AosPyemail::getRequest()
{
	AosStRequestPtr l = 0;
	if (!mQueue.empty())
	{
		l = mQueue.front();
		mQueue.pop();
	}
	return l;
}

bool
AosPyemail::sendmail(AosXmlTagPtr &childelem, const AosRundataPtr &rdata)
{
	// It is to send an email.
	// 'childelem' should be in the form:
	//  <operation opr="sendmail"
	//  	smailaddr ="xxx"
	//   	senderpasswd ="xxx"
	//		rmailaddr="xxx"
	//		subject="xxx"
	//		contents="xxx"
	//  </operation>
	// Chen Ding, 2011/02/05
	// Need to uncommented it when python problem is fixed.
	if (!childelem)
	{
		rdata->setError() << "Missing childelem!";
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}
	OmnString receiver = childelem->getAttrStr("rmailaddr");
	if (receiver == "")
	{
		rdata->setError() << "Missing the receiver email address!";
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}
	receiver.removeWhiteSpaces();

	OmnString sender= childelem->getAttrStr("smailaddr");
	OmnString senderpasswd = childelem->getAttrStr("senderpasswd");
	OmnString server;
	if (senderpasswd =="" && sender =="") 
	{
		senderpasswd = sgForgotPasswd;
		sender = sgForgotSender;
		server = "smtp.163.com";
	}
	
	if (sender == "")
	{
		rdata->setError() << "Missing the sender email address!";
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}
    sender.removeWhiteSpaces();

	if (senderpasswd =="")
	{
		rdata->setError() << "Missing the sender email passwd!";
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}
	
	OmnString key;
	if (server == "")
	{
		OmnStrParser1 parser(sender,"@");
		parser.nextWord();
		OmnString word = parser.nextWord();
		if (word == "")
		{
			rdata->setError() << "Missing the Sender email address!";
			OmnAlarm << rdata->getErrmsg() << enderr;
			return false;
		}

		OmnStrParser1 parser1(word,".");
		key = parser1.nextWord();
		if (key == "")
		{
			rdata->setError() << "Missing the Sender email Address!";
			OmnAlarm << rdata->getErrmsg() << enderr;
			return false;
		}
		if (!sgServerInitFlag) serverInit();
		server = sgServerMap[key];
		if(server == "")
		{
			rdata->setError() << "Missing email Server Address!";
			OmnAlarm << rdata->getErrmsg() << enderr;
			return false;
		}
	}
	server.removeWhiteSpaces();

	OmnString subject = childelem->getAttrStr("subject");
	if (subject =="")
	{
		rdata->setError() << "Missing the subject email!";
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}

	OmnString contents = childelem->getAttrStr("contents");
	// if (contents =="")
	// {
	// 	errmsg = "Missing the contents email!";
	// 	OmnAlarm << errmsg << enderr;
	// 	errcode = eAosXmlInt_General;
	// 	return false;
	// }
	
	OmnStrParser1 parser(receiver, ",");
	OmnString word;
	OmnString to;
	OmnString count;
	while((word = parser.nextWord()) != "")
	{
		if (to == "") 
		{
			to = word;
		}
		else	
		{
			to <<"," << word;
			count = "usesleep";
		}
	}
	//bool rslt = AosPythonSelf->sendEmail(to, sender, 
	//		senderpasswd, subject, contents, server, count, rdata); 
	//if (!rslt)
	//{
	//	rdata->setError() << "Failed to send email!";
	//	OmnAlarm << rdata->getErrmsg() << enderr;
	//	return false;
	//}
	addRequest(sender, senderpasswd, to, subject, contents, server, count, rdata);
	rdata->setOk();
	return true;
}

